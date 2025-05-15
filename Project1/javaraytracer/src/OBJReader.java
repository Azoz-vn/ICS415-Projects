import org.joml.Matrix4d;

import java.awt.Color;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class OBJReader {

    public static List<Surface.Triangle> loadOBJ(String filePath) throws IOException {
        List<Point3d> vertexList = new ArrayList<>();
        List<Surface.Triangle> triangleList = new ArrayList<>();

        try (BufferedReader br = new BufferedReader(new FileReader(filePath))) {
            String line;
            while ((line = br.readLine()) != null) {
                if (line.startsWith("v ")) {
                    // Parse vertex
                    String[] parts = line.split("\\s+");
                    double x = Double.parseDouble(parts[1]);
                    double y = Double.parseDouble(parts[2]);
                    double z = Double.parseDouble(parts[3]);
                    vertexList.add(new Point3d(x, y, z));
                } else if (line.startsWith("f ")) {
                    // Parse face (assuming triangleList)
                    String[] parts = line.split("\\s+");
                    int v1 = Integer.parseInt(parts[1].split("/")[0]) - 1;
                    int v2 = Integer.parseInt(parts[2].split("/")[0]) - 1;
                    int v3 = Integer.parseInt(parts[3].split("/")[0]) - 1;

                    // Create a triangle from the vertexList
                    Point3d pointA = vertexList.get(v1);
                    Point3d pointB = vertexList.get(v2);
                    Point3d pointC = vertexList.get(v3);

                    // Assuming default material properties for the triangle
                    Surface.Triangle triangle = new Surface.Triangle(Color.RED, -1, 0., 1, 1, pointA, pointB, pointC);
                    triangle.scale(8,8,8);
                    Matrix4d translate = new Matrix4d().translate(-0.7,-1,3);
                    Matrix4d rotate = new Matrix4d().rotateY(Math.toRadians(180));
                    triangle.transform(translate.mul(rotate));
                    triangleList.add(triangle);
                }
            }
        }

        return triangleList;
    }

    public static void main(String[] args) {
        try {
            List<Surface.Triangle> triangleList = loadOBJ("bunny.obj");

            for (Surface.Triangle triangle : triangleList) {
                System.out.println("Surface.Triangle: " + triangle.getPointA() + ", " + triangle.getPointB() + ", " + triangle.getPointC());
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
