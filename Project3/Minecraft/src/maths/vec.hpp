#ifndef VEC_HPP
#define VEC_HPP

#include <cmath>
#include <array>
#include <type_traits>

template <size_t N, typename T = float>
struct Vec {
    static_assert(N > 0, "Vector must have at least one component.");
    std::array<T, N> data;

    // Constructors
    Vec() {
        data.fill(static_cast<T>(0));
    }

    template<typename... Args, typename = std::enable_if_t<sizeof...(Args) == N>>
    Vec(Args... args) : data{ static_cast<T>(args)... } {}

    T& operator[](size_t i) { return data[i]; }
    const T& operator[](size_t i) const { return data[i]; }

    Vec operator+(const Vec& rhs) const {
        Vec result;
        for (size_t i = 0; i < N; ++i)
            result[i] = data[i] + rhs[i];
        return result;
    }

    Vec operator-(const Vec& rhs) const {
        Vec result;
        for (size_t i = 0; i < N; ++i)
            result[i] = data[i] - rhs[i];
        return result;
    }

    Vec operator*(T scalar) const {
        Vec result;
        for (size_t i = 0; i < N; ++i)
            result[i] = data[i] * scalar;
        return result;
    }

    Vec operator/(T scalar) const {
        Vec result;
        for (size_t i = 0; i < N; ++i)
            result[i] = data[i] / scalar;
        return result;
    }

    T length() const {
        T lenSquared = static_cast<T>(0);
        for (size_t i = 0; i < N; ++i)
            lenSquared += data[i] * data[i];
        return std::sqrt(lenSquared);
    }

    Vec normalize() const {
        T len = length();
        return len > static_cast<T>(0) ? (*this) / len : Vec();
    }

    static T dot(const Vec& a, const Vec& b) {
        T result = static_cast<T>(0);
        for (size_t i = 0; i < N; ++i)
            result += a[i] * b[i];
        return result;
    }
};

// Specialized cross product for 3D
template <typename T>
Vec<3, T> cross(const Vec<3, T>& a, const Vec<3, T>& b) {
    return Vec<3, T>(
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]
    );
}

// Convenience aliases
using Vec2f = Vec<2, float>;
using Vec3f = Vec<3, float>;
using Vec4f = Vec<4, float>;
using Vec2i = Vec<2, int>;
using Vec3i = Vec<3, int>;

#endif // VEC_HPP
