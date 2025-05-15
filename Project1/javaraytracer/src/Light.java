public class Light {
    enum LightType {
        Ambient,
        Point,
        Directional
    }
    private double intensity;
    private Vector3 sourcePosition;
    private Vector3 direction;
    private LightType LightType;

    public Light(double intensity) {
        this.LightType = LightType.Ambient;
        this.intensity = intensity;
    }

    //point
    public Light(double intensity, Vector3 sourcePosition) {
        this.LightType = LightType.Point;
        this.sourcePosition = sourcePosition;
        this.intensity = intensity;
    }
    public Light(Vector3 direction, double intensity) {
        this.LightType = LightType.Directional;
        this.direction = direction;
        this.intensity = intensity;
    }

    public double getIntensity() {
        return intensity;
    }

    public LightType getLightType() {
        return LightType;
    }

    public Vector3 getDirection() {
        return direction;
    }

    public Vector3 getsourcePosition () {
        return sourcePosition;
    }
}
