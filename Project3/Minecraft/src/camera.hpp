#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "maths/vec.hpp"
#include "maths/mat4.hpp"

class Camera {
public:
    Camera(float fovDegrees, float aspect, float nearZ, float farZ);

    void setPosition(float x, float y, float z);
    Vec3f& getPosition();
    void setRotation(float pitchDeg, float yawDeg);
    Vec3f getForward() const;
    Vec3f getRight() const;
    Vec3f getUp() const;
    void move(float dx, float dy, float dz);
    void rotate(float dPitch, float dYaw);

    Mat4 getViewMatrix() const;
    Mat4 getProjectionMatrix() const;

    void updateViewMatrix();

private:
    Vec3f position{}, forward{}, right{}, up{};
    float pitch = 0, yaw = 0;

    Mat4 viewMatrix;
    Mat4 projMatrix;

    float fovRad, aspect, nearPlane, farPlane;
};

#endif
