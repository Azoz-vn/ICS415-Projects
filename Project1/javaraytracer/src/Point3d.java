public class Point3d {
    Double x;
    Double y;
    Double z;

    Point3d(Double x, Double y, Double z){
        this.x = x;
        this.y = y;
        this.z = z;
    }
    
    Point3d(Vector3 g){
        this.x = g.getX();
        this.y = g.getY();
        this.z = g.getZ();
    }
    Vector3 subtract(Point3d point) {
        return new Vector3(this.x-point.x,this.y-point.y,this.z-point.z);
    }
    Vector3 subtract(Vector3 point) {
        return new Vector3(this.x-point.getX(),this.y-point.getY(),this.z-point.getZ());
    }

    public Double getX() {
        return x;
    }

    public Double getY() {
        return y;
    }

    public Double getZ() {
        return z;
    }
}
