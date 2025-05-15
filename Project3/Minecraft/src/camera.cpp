#include "camera.hpp"
#include "utils.hpp"

#ifndef M_PI
#define M_PI 3.14159265f
#endif

Camera::Camera(float fovDegrees, float aspectRatio, float nearZ, float farZ)
    : aspect(aspectRatio), nearPlane(nearZ), farPlane(farZ) {

    fovRad = fovDegrees * (M_PI / 180.0f);
    projMatrix = Mat4::perspective(fovDegrees, aspectRatio, nearZ, farZ);

    updateViewMatrix();
}

void Camera::setPosition(float x, float y, float z) {
    position = Vec3f{x, y, z};
    updateViewMatrix();
}

Vec3f& Camera::getPosition() {
    return position;
}

void Camera::setRotation(float pitchDeg, float yawDeg) {
    pitch = pitchDeg * (M_PI / 180.0f);
    yaw = yawDeg * (M_PI / 180.0f);
    updateViewMatrix();
}

Vec3f Camera::getForward() const {
    return forward;
}

Vec3f Camera::getRight() const {
    return right;
}

Vec3f Camera::getUp() const {
    return up;
}

void Camera::move(float dx, float dy, float dz) {
    float cp = std::cos(pitch), sp = std::sin(pitch);
    float cy = std::cos(yaw), sy = std::sin(yaw);

    Vec3f fwd = Vec3f{cp * sy, sp, cp * cy}.normalize();
    Vec3f right = cross(Vec3f{0, 1, 0}, fwd).normalize();
    Vec3f up = cross(fwd, right);

    position = position + right * dx + up * dy + fwd * dz;
}

void Camera::rotate(float dPitch, float dYaw) {
    pitch += dPitch * (M_PI / 180.0f);
    pitch = CLAMP(pitch, -M_PI / 2.0f, M_PI / 2.0f);
    yaw += dYaw * (M_PI / 180.0f);
    updateViewMatrix();
}

Mat4 Camera::getViewMatrix() const {
    return viewMatrix;
}

Mat4 Camera::getProjectionMatrix() const {
    return projMatrix;
}

void Camera::updateViewMatrix() {
    float cp = std::cos(pitch), sp = std::sin(pitch);
    float cy = std::cos(yaw), sy = std::sin(yaw);

    forward = Vec3f{cp * sy, sp, cp * cy}.normalize();
    Vec3f baseUp{0, 1, 0};
    right = cross(baseUp, forward).normalize();
    up = cross(forward, right);

    viewMatrix[0] = right[0]; viewMatrix[1] = up[0]; viewMatrix[2] = -forward[0]; viewMatrix[3] = 0;
    viewMatrix[4] = right[1]; viewMatrix[5] = up[1]; viewMatrix[6] = -forward[1]; viewMatrix[7] = 0;
    viewMatrix[8] = right[2]; viewMatrix[9] = up[2]; viewMatrix[10] = -forward[2]; viewMatrix[11] = 0;
    viewMatrix[12] = -Vec3f::dot(right, position);
    viewMatrix[13] = -Vec3f::dot(up, position);
    viewMatrix[14] = Vec3f::dot(forward, position);
    viewMatrix[15] = 1;
}

