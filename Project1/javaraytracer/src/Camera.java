import org.joml.Matrix4d;

public class Camera {
    private Vector3 position;
    private Matrix4d orientation; // Represent orientation as a 3x3 matrix

    // Default constructor: position at (0,0,0), orientation as identity matrix (no orientation)
    public Camera() {
        this.position = new Vector3(0, 0, 0);
        this.orientation = new Matrix4d(); // Assume Matrix3 has an identity() method
    }

    // Constructor with position and default orientation
    public Camera(Vector3 position) {
        this.position = position;
        this.orientation = new Matrix4d(); // Assume Matrix3 has an identity() method
    }

    // Constructor with both position and orientation
    public Camera(Vector3 position, Matrix4d orientation) {
        this.position = position;
        this.orientation = orientation;
    }

    // Return the orientation matrix (not a Double!)
    public Matrix4d getorientation() {
        return orientation;
    }

    // Setter for orientation (optional)
    public void setorientation(Matrix4d orientation) {
        this.orientation = orientation;
    }

    // Getter for position
    public Vector3 getPosition() {
        return position;
    }
}