import java.util.Objects;

public class Matrix3 {
    private final Vector3 row0; // Represents the first row (x-axis basis)
    private final Vector3 row1; // Second row (y-axis basis)
    private final Vector3 row2; // Third row (z-axis basis)

    // Construct using three row vectors
    public Matrix3(Vector3 row0, Vector3 row1, Vector3 row2) {
        this.row0 = row0;
        this.row1 = row1;
        this.row2 = row2;
    }
    public Matrix3() {
        this.row0 = new Vector3(1,0,0);
        this.row1 = new Vector3(0,1,0) ;
        this.row2 = new Vector3(0,0,1) ;
    }

    // ===== Core Functionality =====
    public static Matrix3 identityMatrix() {
        return new Matrix3(
                new Vector3(1, 0, 0),
                new Vector3(0, 1, 0),
                new Vector3(0, 0, 1)
        );
    }

    // Matrix multiplication (this * other)
    public Matrix3 multiplyBy(Matrix3 other) {
        Vector3 otherCol0 = new Vector3(other.row0.getX(), other.row1.getX(), other.row2.getX());
        Vector3 otherCol1 = new Vector3(other.row0.getY(), other.row1.getY(), other.row2.getY());
        Vector3 otherCol2 = new Vector3(other.row0.getZ(), other.row1.getZ(), other.row2.getZ());

        return new Matrix3(
                new Vector3(row0.dot(otherCol0), row0.dot(otherCol1), row0.dot(otherCol2)),
                new Vector3(row1.dot(otherCol0), row1.dot(otherCol1), row1.dot(otherCol2)),
                new Vector3(row2.dot(otherCol0), row2.dot(otherCol1), row2.dot(otherCol2))
        );
    }

    // Transpose the matrix (swap rows and columns)
    public Matrix3 transposeMatrix() {
        return new Matrix3(
                new Vector3(row0.getX(), row1.getX(), row2.getX()),
                new Vector3(row0.getY(), row1.getY(), row2.getY()),
                new Vector3(row0.getZ(), row1.getZ(), row2.getZ())
        );
    }

    // ===== Advanced Operations =====
    public double determinant() {
        return row0.getX() * (row1.getY() * row2.getZ() - row1.getZ() * row2.getY())
                - row0.getY() * (row1.getX() * row2.getZ() - row1.getZ() * row2.getX())
                + row0.getZ() * (row1.getX() * row2.getY() - row1.getY() * row2.getX());
    }

    public Matrix3 inverse() {
        double det = determinant();
        if (det == 0) throw new ArithmeticException("Matrix is singular (non-invertible)");

        // Cofactor calculations with proper sign alternation
        double c00 =  row1.getY() * row2.getZ() - row1.getZ() * row2.getY();
        double c01 = -row1.getX() * row2.getZ() + row1.getZ() * row2.getX();
        double c02 =  row1.getX() * row2.getY() - row1.getY() * row2.getX();

        double c10 = -row0.getY() * row2.getZ() + row0.getZ() * row2.getY();
        double c11 =  row0.getX() * row2.getZ() - row0.getZ() * row2.getX();
        double c12 = -row0.getX() * row2.getY() + row0.getY() * row2.getX();

        double c20 =  row0.getY() * row1.getZ() - row0.getZ() * row1.getY();
        double c21 = -row0.getX() * row1.getZ() + row0.getZ() * row1.getX();
        double c22 =  row0.getX() * row1.getY() - row0.getY() * row1.getX();

        Matrix3 adjugate = new Matrix3(
                new Vector3(c00, c10, c20),
                new Vector3(c01, c11, c21),
                new Vector3(c02, c12, c22)
        );

        return adjugate.transposeMatrix().multiplyBy(1.0 / det);
    }

    // ===== Transformations =====
    public static Matrix3 createRotationX(double radians) {
        double cos = Math.cos(radians);
        double sin = Math.sin(radians);
        return new Matrix3(
                new Vector3(1, 0, 0),
                new Vector3(0, cos, -sin),
                new Vector3(0, sin, cos)
        );
    }

    public static Matrix3 createRotationY(double radians) {
        double cos = Math.cos(radians);
        double sin = Math.sin(radians);
        return new Matrix3(
                new Vector3(cos, 0, sin),
                new Vector3(0, 1, 0),
                new Vector3(-sin, 0, cos)
        );
    }

    public static Matrix3 createRotationZ(double radians) {
        double cos = Math.cos(radians);
        double sin = Math.sin(radians);
        return new Matrix3(
                new Vector3(cos, -sin, 0),
                new Vector3(sin, cos, 0),
                new Vector3(0, 0, 1)
        );
    }

    // ===== Vector Operations =====
    public Vector3 transform(Vector3 vec) {
        return new Vector3(
                row0.dot(vec),
                row1.dot(vec),
                row2.dot(vec)
        );
    }

    // ===== Operator Overloads =====
    public Matrix3 add(Matrix3 other) {
        return new Matrix3(
                row0.add(other.row0),
                row1.add(other.row1),
                row2.add(other.row2)
        );
    }

    public Matrix3 subtract(Matrix3 other) {
        return new Matrix3(
                row0.subtract(other.row0),
                row1.subtract(other.row1),
                row2.subtract(other.row2)
        );
    }

    public Matrix3 multiplyBy(double scalar) {
        return new Matrix3(
                row0.mul(scalar),
                row1.mul(scalar),
                row2.mul(scalar)
        );
    }

    // ===== Accessors & Utilities =====
    public Vector3[] getRows() {
        return new Vector3[]{row0, row1, row2};
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Matrix3 matrix3 = (Matrix3) o;
        return row0.equals(matrix3.row0) &&
                row1.equals(matrix3.row1) &&
                row2.equals(matrix3.row2);
    }

    @Override
    public int hashCode() {
        return Objects.hash(row0, row1, row2);
    }

    @Override
    public String toString() {
        return String.format(
                "Matrix3:\n[%s]\n[%s]\n[%s]",
                row0, row1, row2
        );
    }
}