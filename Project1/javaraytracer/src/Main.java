import org.joml.Matrix4d;
import org.joml.Vector3d;

import java.awt.*;

public class Main {
    public static void main(String[] args) {
        World world = new World();
        world.setBackgroundColor(new Color(74, 177, 250));
        world.add(new Surface.Sphere(new Vector3(0.0,-1000.0,0.0),1000,0.0, new Color(0.5f,0.5f,0.5f),4,0,0));

        world.add(new Light(0.3));/* This is Ambient */
        world.add(new Light(0.4,new Vector3(0,4,0))); /* This is point */
        world.add(new Light(new Vector3(0,1,0), 0.5));/* This is Directional */

        for (int i = -11; i < 11; i++) {
            for (int j = -11; j < 11; j++) {
                double chooseMaterial = 3*Math.random();
                Vector3 center = new Vector3(i+ 0.9*Math.random(),0.2, j + j*Math.random());

                if (center.subtract(new Point3d(4.,0.2,0.)).length() > 0.9) {
                    if (chooseMaterial < 1.3) {
                        Color randomColor = new Color((int) (Math.random() * 205), (int) (Math.random() * 255), (int) (Math.random() * 200));
                        world.add(Surface.Sphere.lambertian(center,0.2,randomColor));
                    } else if (chooseMaterial <= 2.9) {
                        Color randomColor = new Color((int) (Math.random() * 200), (int) (Math.random() * 255), (int) (Math.random() * 255));
                        double fuzz = Math.random()*0.5;
                        world.add(Surface.Sphere.metal(center,0.2,randomColor,fuzz));

                    } else if (chooseMaterial < 3) {
                        world.add(Surface.Sphere.dielectric(center,0.2,1.5));
                    }
                }
            }
        }

        world.add(Surface.Sphere.dielectric(new Vector3(0,1,0),1,1.5));
        world.add(Surface.Sphere.lambertian(new Vector3(-4,1,0),1,new Color(0.4f,0.2f,0.1f)));
        world.add(Surface.Sphere.metal(new Vector3(4,1,0),1,new Color(0.7f,0.6f,0.5f),0.0));

        // Setting Camera Up
        // Define your camera's desired position, target, and up vector.
        Vector3d eye = new Vector3d(0,0, 0);
        Vector3d center = new Vector3d(13, 2, 3);
        Vector3d up = new Vector3d(0, 1, 0);

// Compute the view matrix. This matrix transforms world coordinates into camera (view) coordinates.
        Matrix4d viewMatrix = new Matrix4d().lookAt(eye, center, up);

// Invert the view matrix to obtain the world transformation (which gives the camera's orientation).
        Matrix4d worldMatrix = new Matrix4d(viewMatrix.invert());

        Camera camera = new Camera(new Vector3(new Matrix4d().scale(0.8).transform(new Vector3(13, 2, -3))), worldMatrix.rotationY(-Math.PI/2.3).rotateX(0.15))
                ;
        //Camera camera = new Camera(new Vector3(13,2,3), new Matrix4d().rotateY(Math.PI-0.1).rotateX(Math.PI/4));

        RayTracer renderer = new RayTracer(1200,675,4,camera,world);
        renderer.render();
    }
}
