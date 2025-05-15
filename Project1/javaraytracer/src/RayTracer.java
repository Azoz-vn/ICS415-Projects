import org.joml.Matrix4d;
import org.joml.Vector4d;

import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.concurrent.ThreadLocalRandom;
import java.util.stream.IntStream;

public class RayTracer {

    record SurfaceAndIntersection(Surface surface , Double doubleValue) {}

    int Cw; // Canvas width
    int Ch; // Canvas height
    int samplingFactor;
    Color BACKGROUND_COLOR;

    private Surface.Sphere[] spheres;

    private Light[] lights;

    private Camera camera;

    private Surface.Triangle[] triangles;

    private Surface.Cylinder[] cylinders;

    public RayTracer(int Cw, int Ch, int SuperSamplingLevel, Camera camera,World data){
            this.Cw = Cw;
            this.Ch = Ch;
            this.camera = camera;
            this.samplingFactor = SuperSamplingLevel;
            this.lights = data.getLights() != null ? data.getLights() : new Light[] {
                    new Light(0.2)
            };
            this.BACKGROUND_COLOR = data.getBackgroundColor() != null ? data.getBackgroundColor() : Color.CYAN;
            this.spheres   = data.getSpheres() != null ? data.getSpheres() : new Surface.Sphere[0];
            this.triangles = data.getTriangles() != null ? data.getTriangles() : new Surface.Triangle[0];
            this.cylinders = data.getCylinders() != null ? data.getCylinders() : new Surface.Cylinder[0];
    }

    public void render() {

        // Rabbit reading
        if (false) {
            try {
                triangles = OBJReader.loadOBJ("bunny.obj").toArray(new Surface.Triangle[0]);
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }

        long startTime = System.nanoTime(); // Capture start time
        Matrix4d Rotation = new Matrix4d().translate(
                0,0,-5
        );

        BufferedImage img = new BufferedImage(Cw+ 1, Ch+ 1, BufferedImage.TYPE_INT_RGB);
        Frame frame = new Frame();
        IntStream.range(-Cw/2, Cw/2)
                .parallel()  // process x values in parallel
                .forEach(x -> {
                    IntStream.range(-Ch/2, Ch/2)
                            .forEach(y -> {
                                Color color = SuperSampling(camera, x, y, samplingFactor);
                                putPixel(x, y, img, color);
                            });
                });


        long endTime = System.nanoTime(); // Capture end time
        long duration = (endTime - startTime); // Calculate duration in nanoseconds
        double durationInSeconds = duration / 1_000_000_000.0; // Convert to seconds

        System.out.println("Time taken: " + durationInSeconds + " seconds");
        // Save image
        try {
            ImageIO.write(img, "PNG", new File("output2.png"));
            System.out.println("Image saved.");
        } catch (IOException fa) {}


    }

    private Color SuperSampling(Camera camera, int x, int y, int samplingFactor) {
        int red = 0;
        int green = 0;
        int blue = 0;
        int totalSamples = samplingFactor * samplingFactor;

        for (int dx = 0; dx < samplingFactor; dx++) {
            for (int dy = 0; dy < samplingFactor; dy++) {
                // Jittered sampling with ThreadLocalRandom
                double jitterX = ThreadLocalRandom.current().nextDouble(0.5);
                double jitterY = ThreadLocalRandom.current().nextDouble(0.5);
                double sampleX = x + (dx + jitterX) / samplingFactor;
                double sampleY = y + (dy + jitterY) / samplingFactor;

                // Compute ray direction and trace
                Vector4d data  = camera.getorientation().transform(
                        CanvasToViewport(sampleX, sampleY)
                );
                Vector3 direction = new Vector3(data);
                Color color = traceRay(camera.getPosition(), direction, 1.0, Double.MAX_VALUE, 3);

                // Accumulate the color values
                red += color.getRed();
                green += color.getGreen();
                blue += color.getBlue();
            }
        }
        // Gamma correction parameters
        final double invTotal = 1.0 / totalSamples;

        // Process each channel with proper gamma correction
        int avgRed = processChannel(red, invTotal);
        int avgGreen = processChannel(green, invTotal);
        int avgBlue = processChannel(blue, invTotal);
        Color color = new Color(clamp(avgRed), clamp(avgGreen), clamp(avgBlue));

        return color;
    }

    private int processChannel(int channelSum, double invTotal) {
        // Normalize -> Gamma correct -> Scale to 8-bit
        double normalized = channelSum * invTotal / 255.0;
        return (int) Math.round(normalized * 255);
    }

    // Modified CanvasToViewport to support sub-pixel precision
    private Vector3 CanvasToViewport(double x, double y) {
        final double aspectRatio = (double)Cw / Ch;
        final double viewportWidth = 1.0;
        final double viewportHeight = 1.0 / aspectRatio;
        final double distance = 1.0;

        return new Vector3(
                x * viewportWidth / Cw,
                y * viewportHeight / Ch,
                distance
        );
    }

    private void putPixel(int x, int y, BufferedImage img, Color color) {
        int px = (Cw / 2) + x;
        int py = (Ch / 2) - y;
        img.setRGB(px, py, color.getRGB());
    }

    private Color traceRay(Vector3 cameraPosition, Vector3 d, double t_min, double t_max, int recursion_depth) {
        Vector3 P = null;
        Vector3 N= null;
        SurfaceAndIntersection closestIntersection = ClosestIntersection(cameraPosition, d, t_min, t_max);
        double closest_t = closestIntersection.doubleValue;
        Surface closest_surface = closestIntersection.surface;

        if (closest_surface == null)
            return BACKGROUND_COLOR;

        if (closest_surface instanceof Surface.Cylinder) {
            P = cameraPosition.add(d.mul(closest_t));
            N = new Vector3(P.getX() - ((Surface.Cylinder) closest_surface).getCenter().getX(), 0, P.getZ() - ((Surface.Cylinder) closest_surface).getCenter().getZ()).normalize();
        }
        if (closest_surface instanceof Surface.Triangle){
            P = cameraPosition.add(d.mul(closest_t));
            N = P.subtract(((Surface.Triangle) closest_surface).getTriEdgeNormal());
        }
        if (closest_surface instanceof Surface.Sphere) {
            P = cameraPosition.add(d.mul(closest_t));
            N = P.subtract(((Surface.Sphere) closest_surface).getCenter()).normalize();
        }

        // Compute lighting and local color
        double lighting = ComputeLighting(P, N, d.mul(-1), closest_surface.getSpecular());
        Color local_color = scaleColor(closest_surface.getColor(), lighting);

        // Base case: no recursion or non-reflective/transparent object
        double reflectivity = closest_surface.getReflective();
        double transparency = closest_surface.getTransparency();
        if (recursion_depth <= 0 || (reflectivity <= 0 && transparency <= 0)) {
            return local_color;
        }

        Color final_color = local_color;

        // Reflection
        if (reflectivity > 0) {
            Vector3 R = ReflectRay(d.mul(-1), N);
            //Vector3 RefractedOrigin = P.add(N.mul(0.001));
            Color reflected_color = traceRay(P, R, 0.001, Double.MAX_VALUE, recursion_depth - 1);
            final_color = blendColors(final_color, reflected_color, reflectivity);
        }

        // Refraction (Transparency)
        if (transparency > 0) {
            double eta = closest_surface.getRefractiveIndex();
            Vector3 refractedDir = RefractRay(d, N, eta);
            if (refractedDir != null) {
                // Offset origin to avoid self-intersection
                //Vector3 refractedOrigin = P.add(N.mul(-0.001)); // Flip normal for exit
                Color refracted_color = traceRay(P, refractedDir, 0.001, Double.MAX_VALUE, recursion_depth - 1);
                final_color = blendColors(final_color, refracted_color, transparency);
            }
        }

        return final_color;
        // ******************* Surface.Sphere ********************** //
    }
    private SurfaceAndIntersection ClosestIntersection(Vector3 cameraPosition, Vector3 d, double t_min, double t_max) {
        double closest_t = Double.MAX_VALUE;
        Surface closest_surface = null;

        // Check intersections with spheres
        for (Surface.Sphere sphere : spheres) {
            ArrayList<Double> allT = IntersectRaySphere(cameraPosition, d, sphere);
            double t1 = allT.get(0);
            double t2 = allT.get(1);

            if (t_min < t1 && t1 < t_max && t1 < closest_t) {
                closest_t = t1;
                closest_surface = sphere;
            }
            if (t_min < t2 && t2 < t_max && t2 < closest_t) {
                closest_t = t2;
                closest_surface = sphere;
            }
        }

        // Check intersections with triangles
        for (Surface.Triangle triangle : triangles) {
            Double t = IntersectTriangle(cameraPosition, d, triangle);
            if (t != null && t_min < t && t < t_max && t < closest_t) {
                closest_t = t;
                closest_surface = triangle;
            }
        }

        // Check intersections with cylinders
        for (Surface.Cylinder cylinder : cylinders) {
            ArrayList<Double> allT = IntersectRayCylinder(cameraPosition, d, cylinder);
            double t1 = allT.get(0);
            double t2 = allT.get(1);
            if (t_min < t1 && t1 < t_max && t1 < closest_t) {
                closest_t = t1;
                closest_surface= cylinder;
            }
            if (t_min < t2 && t2 < t_max && t2 < closest_t) {
                closest_t = t2;
                closest_surface= cylinder;
            }
        }

        return new SurfaceAndIntersection(closest_surface, closest_t);
    }

    private ArrayList<Double> IntersectRayCylinder(Vector3 rayOrigin, Vector3 rayDir, Surface.Cylinder cylinder) {
        ArrayList<Double> tCandidates = new ArrayList<>();

        double radius = cylinder.getRadius();
        double height = cylinder.getHeight();
        Vector3 center = cylinder.getCenter();

        // Extract coordinates for clarity.
        double cx = center.getX();
        double cy = center.getY();
        double cz = center.getZ();

        double ox = rayOrigin.getX();
        double oy = rayOrigin.getY();
        double oz = rayOrigin.getZ();

        double dx = rayDir.getX();
        double dy = rayDir.getY();
        double dz = rayDir.getZ();

        // Define the y-boundaries of the finite cylinder.
        double yTop = cy + height / 2.0;
        double yBottom = cy - height / 2.0;


        double A = dx * dx + dz * dz;
        double B = 2 * ((ox - cx) * dx + (oz - cz) * dz);
        double Ccoef = (ox - cx) * (ox - cx) + (oz - cz) * (oz - cz) - radius * radius;

        // Only solve the quadratic if A is not nearly zero (i.e. ray not parallel to the cylinder’s side).
        if (Math.abs(A) > 1e-6) {
            double discriminant = B * B - 4 * A * Ccoef;
            if (discriminant >= 0) {
                double sqrtDisc = Math.sqrt(discriminant);
                double t1 = (-B - sqrtDisc) / (2 * A);
                double t2 = (-B + sqrtDisc) / (2 * A);

                // For each solution, check that the y coordinate lies within the cylinder’s height.
                double y1 = oy + t1 * dy;
                if (y1 >= yBottom && y1 <= yTop) {
                    tCandidates.add(t1);
                }

                double y2 = oy + t2 * dy;
                if (y2 >= yBottom && y2 <= yTop) {
                    tCandidates.add(t2);
                }
            }
        }


        if (Math.abs(dy) > 1e-6) {
            // Top cap at y = yTop.
            double tTop = (yTop - oy) / dy;
            double xTop = ox + tTop * dx;
            double zTop = oz + tTop * dz;
            if ((xTop - cx) * (xTop - cx) + (zTop - cz) * (zTop - cz) <= radius * radius) {
                tCandidates.add(tTop);
            }

            // Bottom cap at y = yBottom.
            double tBottom = (yBottom - oy) / dy;
            double xBottom = ox + tBottom * dx;
            double zBottom = oz + tBottom * dz;
            if ((xBottom - cx) * (xBottom - cx) + (zBottom - cz) * (zBottom - cz) <= radius * radius) {
                tCandidates.add(tBottom);
            }
        }

        Collections.sort(tCandidates);


        ArrayList<Double> result = new ArrayList<>();
        if (tCandidates.size() >= 2) {
            result.add(tCandidates.get(0));
            result.add(tCandidates.get(1));
        } else if (tCandidates.size() == 1) {
            result.add(tCandidates.getFirst());
            result.add(Double.POSITIVE_INFINITY);
        } else {
            result.add(Double.POSITIVE_INFINITY);
            result.add(Double.POSITIVE_INFINITY);
        }

        return result;
    }

    private Double IntersectTriangle(Vector3 cameraPosition, Vector3 d, Surface.Triangle triangle){
        double n_dot_d = triangle.getTriEdgeNormal().dot(d);
        if (n_dot_d == 0){
            return Double.MAX_VALUE;
        }

        double n_dot_ps = triangle.getTriEdgeNormal().dot(triangle.getPointA().subtract(cameraPosition));
        double t = n_dot_ps / n_dot_d;

        Point3d plane_point = new Point3d(cameraPosition.add(d.mul(t)));

        Vector3 AtoPoint = plane_point.subtract(triangle.getPointA());
        Vector3 BtoPoint = plane_point.subtract(triangle.getPointB());
        Vector3 CtoPoint = plane_point.subtract(triangle.getPointC());

        Vector3 AtestVec = triangle.getAtoB_edge().cross(AtoPoint);
        Vector3 BtestVec = triangle.getBtoC_edge().cross(BtoPoint);
        Vector3 CtestVec = triangle.getCtoA_edge().cross(CtoPoint);

        Boolean AtestVec_matchesNormal = AtestVec.dot(triangle.getTriEdgeNormal())  > 0.;
        Boolean BtestVec_matchesNormal = BtestVec.dot(triangle.getTriEdgeNormal())  > 0.;
        Boolean CtestVec_matchesNormal = CtestVec.dot(triangle.getTriEdgeNormal())  > 0.;

        boolean hitTriangle = AtestVec_matchesNormal && BtestVec_matchesNormal && CtestVec_matchesNormal;
        if (hitTriangle) {
            return t;
        }
        return Double.MAX_VALUE;
    }

    private ArrayList<Double> IntersectRaySphere(Vector3 cameraPosition, Vector3 d, Surface.Sphere sphere) {
        ArrayList<Double> AllT = new ArrayList<>();
        double r = sphere.getRadius();
        Vector3 CO = cameraPosition.subtract(sphere.getCenter());

        double a = d.dot(d);
        double b = 2 * CO.dot(d); // Note the factor of 2
        double c = CO.dot(CO) - r * r;

        double discriminant = b * b - 4 * a * c;

        if (discriminant < 0) {
            AllT.add(Double.MAX_VALUE);
            AllT.add(Double.MAX_VALUE);
            return AllT;
        }

        double sqrtDisc = Math.sqrt(discriminant);
        AllT.add((-b + sqrtDisc) / (2 * a));
        AllT.add((-b - sqrtDisc) / (2 * a));

        return AllT;
    }

    private Color blendColors(Color base, Color added, double coefficient) {
        int r = (int) (base.getRed() * (1 - coefficient) + added.getRed() * coefficient);
        int g = (int) (base.getGreen() * (1 - coefficient) + added.getGreen() * coefficient);
        int b = (int) (base.getBlue() * (1 - coefficient) + added.getBlue() * coefficient);
        return new Color(clamp(r), clamp(g), clamp(b));
    }

    private Color scaleColor(Color color, double factor) {
        int r = (int) (color.getRed() * factor);
        int g = (int) (color.getGreen() * factor);
        int b = (int) (color.getBlue() * factor);
        return new Color(clamp(r), clamp(g), clamp(b));
    }

    private int clamp(int value) {
        return Math.min(255, Math.max(0, value));
    }

    private Vector3 RefractRay(Vector3 direction, Vector3 normal, double refractionIndex) {
        // Calculate the refractive index ratio (n1 / n2)
        double refractiveIndexRatio =  1 / refractionIndex;

        // Calculate the dot product between the direction of the ray and the normal
        double cosI = direction.negate().dot(normal);

        // Calculate the angle using Snell's Law to determine refraction
        double sinT2 = refractiveIndexRatio * refractiveIndexRatio * (1 - cosI * cosI);

        // If total internal reflection occurs, return the reflected direction instead
        if (sinT2 > 1) {
            // Total internal reflection, reflect the ray instead
            return ReflectRay(direction, normal);
        }

        // Calculate cosT (the cosine of the angle of refraction)
        double cosT = Math.sqrt(1.0 - sinT2);

        // Compute the refracted ray direction using Snell's Law
        Vector3 refractedDirection = direction.mul(refractiveIndexRatio).subtract(
                normal.mul(refractiveIndexRatio * cosI + cosT)
        );

        return refractedDirection.normalize();
    }

    private Double ComputeLighting(Vector3 P, Vector3 N,Vector3 V, double s) {
        double i = 0.0;
        double t_max;
        Vector3 L;
        for (Light light : lights){
            if (light.getLightType() == Light.LightType.Ambient) {
                i += light.getIntensity();
            } else {
                if (light.getLightType() == Light.LightType.Point) {
                    L = light.getsourcePosition().subtract(P);
                    t_max = 1.0;
                } else {
                    L = light.getDirection();
                    t_max = Double.MAX_VALUE;
                }


                // Shadow Check
                SurfaceAndIntersection shadowChecks = ClosestIntersection(P,L,0.001, t_max);
                Surface shadow_surface = shadowChecks.surface;
                if (shadow_surface != null ) {
                    continue;
                }
                // Diffuse
                double n_dot_l = N.dot(L);
                if (n_dot_l > 0) {
                    i += light.getIntensity() * n_dot_l/(N.length() * L.length());
                }

                if (s != -1) {
                    Vector3 R = ReflectRay(L,N);
                    double r_dot_v = R.dot(V);
                    if (r_dot_v > 0) {
                        i += light.getIntensity() * Math.pow(r_dot_v/(R.length() * V.length()),s);
                    }
                }
            }
        }
        return i;
    }

    private Vector3 ReflectRay(Vector3 I, Vector3 N){
        return N.mul(2*N.dot(I)).subtract(I);
    }

}