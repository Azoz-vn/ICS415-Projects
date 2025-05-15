import java.awt.*;

public class World {
    private Surface.Sphere[] spheres;
    private Surface.Triangle[] triangles;
    private Surface.Cylinder[] cylinders;
    private Color BackgroundColor;
    private Light[] lights;  // New field for lights

    // Constructors
    public World() {
    }

    public World(Surface.Sphere[] spheres) {
        this.spheres = spheres;
    }

    public World(Surface.Triangle[] triangles) {
        this.triangles = triangles;
    }

    public World(Surface.Sphere[] spheres, Surface.Triangle[] triangles) {
        this.spheres = spheres;
        this.triangles = triangles;
    }

    public World(Surface.Cylinder[] cylinders) {
        this.cylinders = cylinders;
    }

    public World(Surface.Sphere[] spheres, Surface.Cylinder[] cylinders) {
        this.spheres = spheres;
        this.cylinders = cylinders;
    }

    public World(Surface.Triangle[] triangles, Surface.Cylinder[] cylinders) {
        this.triangles = triangles;
        this.cylinders = cylinders;
    }

    public World(Surface.Sphere[] spheres, Surface.Triangle[] triangles, Surface.Cylinder[] cylinders) {
        this.spheres = spheres;
        this.triangles = triangles;
        this.cylinders = cylinders;
    }

    // Getters for surfaces
    public Surface.Sphere[] getSpheres() {
        return spheres;
    }

    public Surface.Triangle[] getTriangles() {
        return triangles;
    }

    public Surface.Cylinder[] getCylinders() {
        return cylinders;
    }

    // Getter for lights
    public Light[] getLights() {
        return lights;
    }

    public Color getBackgroundColor() {
        return BackgroundColor;
    }

    // --- ADD methods for surfaces ---

    public void add(Surface.Sphere sphere) {
        if (this.spheres == null) {
            this.spheres = new Surface.Sphere[] { sphere };
        } else {
            int len = this.spheres.length;
            Surface.Sphere[] newSpheres = new Surface.Sphere[len + 1];
            System.arraycopy(this.spheres, 0, newSpheres, 0, len);
            newSpheres[len] = sphere;
            this.spheres = newSpheres;
        }
    }

    public void add(Surface.Triangle triangle) {
        if (this.triangles == null) {
            this.triangles = new Surface.Triangle[] { triangle };
        } else {
            int len = this.triangles.length;
            Surface.Triangle[] newTriangles = new Surface.Triangle[len + 1];
            System.arraycopy(this.triangles, 0, newTriangles, 0, len);
            newTriangles[len] = triangle;
            this.triangles = newTriangles;
        }
    }

    public void add(Surface.Cylinder cylinder) {
        if (this.cylinders == null) {
            this.cylinders = new Surface.Cylinder[] { cylinder };
        } else {
            int len = this.cylinders.length;
            Surface.Cylinder[] newCylinders = new Surface.Cylinder[len + 1];
            System.arraycopy(this.cylinders, 0, newCylinders, 0, len);
            newCylinders[len] = cylinder;
            this.cylinders = newCylinders;
        }
    }

    // --- ADD method for Light ---
    public void add(Light light) {
        if (this.lights == null) {
            this.lights = new Light[] { light };
        } else {
            int len = this.lights.length;
            Light[] newLights = new Light[len + 1];
            System.arraycopy(this.lights, 0, newLights, 0, len);
            newLights[len] = light;
            this.lights = newLights;
        }
    }

    // --- REMOVE methods for surfaces ---

    public void remove(Surface.Sphere sphere) {
        if (this.spheres == null) {
            return; // Nothing to remove.
        }
        int index = -1;
        for (int i = 0; i < this.spheres.length; i++) {
            if (this.spheres[i].equals(sphere)) {
                index = i;
                break;
            }
        }
        if (index == -1) {
            return; // Sphere not found.
        }
        Surface.Sphere[] newSpheres = new Surface.Sphere[this.spheres.length - 1];
        System.arraycopy(this.spheres, 0, newSpheres, 0, index);
        if (index < this.spheres.length - 1) {
            System.arraycopy(this.spheres, index + 1, newSpheres, index, this.spheres.length - index - 1);
        }
        this.spheres = newSpheres;
    }

    public void remove(Surface.Triangle triangle) {
        if (this.triangles == null) {
            return;
        }
        int index = -1;
        for (int i = 0; i < this.triangles.length; i++) {
            if (this.triangles[i].equals(triangle)) {
                index = i;
                break;
            }
        }
        if (index == -1) {
            return; // Triangle not found.
        }
        Surface.Triangle[] newTriangles = new Surface.Triangle[this.triangles.length - 1];
        System.arraycopy(this.triangles, 0, newTriangles, 0, index);
        if (index < this.triangles.length - 1) {
            System.arraycopy(this.triangles, index + 1, newTriangles, index, this.triangles.length - index - 1);
        }
        this.triangles = newTriangles;
    }

    public void remove(Surface.Cylinder cylinder) {
        if (this.cylinders == null) {
            return;
        }
        int index = -1;
        for (int i = 0; i < this.cylinders.length; i++) {
            if (this.cylinders[i].equals(cylinder)) {
                index = i;
                break;
            }
        }
        if (index == -1) {
            return; // Cylinder not found.
        }
        Surface.Cylinder[] newCylinders = new Surface.Cylinder[this.cylinders.length - 1];
        System.arraycopy(this.cylinders, 0, newCylinders, 0, index);
        if (index < this.cylinders.length - 1) {
            System.arraycopy(this.cylinders, index + 1, newCylinders, index, this.cylinders.length - index - 1);
        }
        this.cylinders = newCylinders;
    }

    // --- REMOVE method for Light ---
    public void remove(Light light) {
        if (this.lights == null) {
            return; // Nothing to remove.
        }
        int index = -1;
        for (int i = 0; i < this.lights.length; i++) {
            if (this.lights[i].equals(light)) {
                index = i;
                break;
            }
        }
        if (index == -1) {
            return; // Light not found.
        }
        Light[] newLights = new Light[this.lights.length - 1];
        System.arraycopy(this.lights, 0, newLights, 0, index);
        if (index < this.lights.length - 1) {
            System.arraycopy(this.lights, index + 1, newLights, index, this.lights.length - index - 1);
        }
        this.lights = newLights;
    }

    public void setBackgroundColor(Color backgroundColor) {
        BackgroundColor = backgroundColor;
    }

    public void setSpheres(Surface.Sphere[] spheres) {
        this.spheres = spheres;
    }

    public void setTriangles(Surface.Triangle[] triangles) {
        this.triangles = triangles;
    }

    public void setCylinders(Surface.Cylinder[] cylinders) {
        this.cylinders = cylinders;
    }

    public void setLights(Light[] lights) {
        this.lights = lights;
    }
}
