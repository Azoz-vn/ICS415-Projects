#ifndef MAT4_HPP
#define MAT4_HPP

#include "vec.hpp"
#include <cmath>
#include <stdexcept>

struct Mat4 {
    float m[16];

    Mat4() { identity(); }
    explicit Mat4(const float values[16]) { for (int i = 0; i < 16; ++i) m[i] = values[i]; }

    void identity() {
        // The identify matrix just consists of a full grid of zeros exepct down the diognal.
        for (int i = 0; i < 16; ++i) m[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    }

    Mat4 operator*(const Mat4& other) const {
        Mat4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) {
                result.m[i * 4 + j] = 0.0f;
                for (int k = 0; k < 4; ++k)
                    result.m[i * 4 + j] += m[i * 4 + k] * other.m[k * 4 + j];
            }
        return result;
    }

    Vec4f operator*(const Vec4f& v) const {
        Vec4f result;
        for (int i = 0; i < 4; ++i)
            result[i] = m[i * 4 + 0] * v[0] + m[i * 4 + 1] * v[1] +
                        m[i * 4 + 2] * v[2] + m[i * 4 + 3] * v[3];
        return result;
    }

    float &operator[](size_t index) {
        return (index < 16) ? m[index] : m[0];
    }

    static Mat4 translation(float x, float y, float z) {
        Mat4 result;
        result.m[12] = x;
        result.m[13] = y;
        result.m[14] = z;
        return result;
    }

    static Mat4 scale(float x, float y, float z) {
        Mat4 result;
        result.m[0 * 4 + 0] = x;
        result.m[1 * 4 + 1] = y;
        result.m[2 * 4 + 2] = z;
        return result;
    }

    static Mat4 rotateX(float angle) {
        Mat4 result;
        float c = cos(angle), s = sin(angle);
        result.m[1 * 4 + 1] = c;
        result.m[1 * 4 + 2] = -s;
        result.m[2 * 4 + 1] = s;
        result.m[2 * 4 + 2] = c;
        return result;
    }

    static Mat4 rotateY(float angle) {
        Mat4 result;
        float c = cos(angle), s = sin(angle);
        result.m[0 * 4 + 0] = c;
        result.m[0 * 4 + 2] = s;
        result.m[2 * 4 + 0] = -s;
        result.m[2 * 4 + 2] = c;
        return result;
    }

    static Mat4 rotateZ(float angle) {
        Mat4 result;
        float c = cos(angle), s = sin(angle);
        result.m[0 * 4 + 0] = c;
        result.m[0 * 4 + 1] = -s;
        result.m[1 * 4 + 0] = s;
        result.m[1 * 4 + 1] = c;
        return result;
    }

    static Mat4 perspective(float fov, float aspect, float near, float far) {
        Mat4 result;
        float tanHalfFov = tan(fov / 2.0f);
        result.m[0 * 4 + 0] = 1.0f / (aspect * tanHalfFov);
        result.m[1 * 4 + 1] = 1.0f / tanHalfFov;
        result.m[2 * 4 + 2] = -(far + near) / (far - near);
        result.m[2 * 4 + 3] = -1.0f;
        result.m[3 * 4 + 2] = -(2.0f * far * near) / (far - near);
        result.m[3 * 4 + 3] = 0.0f;
        return result;
    }

    static Mat4 lookAt(const Vec3f& direction, const Vec3f& up) {
        // Compute the right and trueUp vectors
        Vec3f right = cross(up, direction).normalize();
        Vec3f trueUp = cross(direction, right).normalize();

        Mat4 result;
        result[0] = right[0];
        result[4] = right[1];
        result[8] = right[2];
        result[12] = 0.0f;

        result[1] = trueUp[0];
        result[5] = trueUp[1];
        result[9] = trueUp[2];
        result[13] = 0.0f;

        result[2] = -direction[0];
        result[6] = -direction[1];
        result[10] = -direction[2];
        result[14] = 0.0f;

        result[3] = 0.0f;
        result[7] = 0.0f;
        result[11] = 0.0f;
        result[15] = 1.0f;

        return result;
    }


    float cofactor(int row, int col) const {
        float sub[9];
        int index = 0;
        for (int i = 0; i < 4; ++i) {
            if (i == row) continue;
            for (int j = 0; j < 4; ++j) {
                if (j == col) continue;
                sub[index++] = m[i * 4 + j];
            }
        }

        // Determinant of 3x3 matrix
        return sub[0] * (sub[4] * sub[8] - sub[5] * sub[7])
             - sub[1] * (sub[3] * sub[8] - sub[5] * sub[6])
             + sub[2] * (sub[3] * sub[7] - sub[4] * sub[6]);
    }

    float determinant() const {
        float det = 0.0f;
        for (int i = 0; i < 4; ++i) {
            float sign = (i % 2 == 0) ? 1.0f : -1.0f;
            det += sign * m[0 * 4 + i] * cofactor(0, i);
        }
        return det;
    }

    Mat4 inverse() const {
        float det = determinant();
        if (std::abs(det) < 1e-6f)
            throw std::runtime_error("Matrix is singular and cannot be inverted.");

        Mat4 inv;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) {
                float sign = ((i + j) % 2 == 0) ? 1.0f : -1.0f;
                inv.m[j * 4 + i] = sign * cofactor(i, j); // Transpose during assignment
            }

        float invDet = 1.0f / det;
        for (int i = 0; i < 16; ++i)
            inv.m[i] *= invDet;

        return inv;
    }
};

#endif
