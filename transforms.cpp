#include "transforms.hpp"
#include <iostream>
#include "rational_circle.hpp"

namespace theocad {
    
void Transform::compute_inverse() {
    // LU decomposition
    Matrix4r L, U;
    L.setIdentity();
    U = affine;

    for (int i = 0; i < 4; ++i) {
        for (int j = i; j < 4; ++j) {
            L(j, i) = U(j, i);
            for (int k = 0; k < i; ++k) {
                L(j, i) -= L(j, k) * U(k, i);
            }
        }
        for (int j = i + 1; j < 4; ++j) {
            U(i, j) /= L(i, i);
            for (int k = 0; k < i; ++k) {
                U(i, j) -= L(i, k) * U(k, j) / L(i, i);
            }
        }
    }

    // Compute inverse using forward and backward substitution
    Matrix4r invL, invU;
    invL.setIdentity();
    invU.setIdentity();

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < i; ++j) {
            real sum = 0;
            for (int k = j; k < i; ++k) {
                sum += L(i, k) * invL(k, j);
            }
            invL(i, j) = -sum / L(i, i);
        }
    }

    for (int i = 3; i >= 0; --i) {
        for (int j = 3; j > i; --j) {
            real sum = 0;
            for (int k = i + 1; k <= j; ++k) {
                sum += U(i, k) * invU(k, j);
            }
            invU(i, j) = -sum;
        }
        invU(i, i) = real(1) / U(i, i);
    }

    inverse = invU * invL;
}

void Transform::transform_child() {
    surfaces.clear();

    if (!child) {
        return;
    }

    for (int i = 0; i < child->size(); ++i) {
        printf("Child surface\n");
        const Surface& childSurface = (*child)[i];
        Surface& newSurface = allocateSurface();

        for (int j = 0; j < childSurface.size(); ++j) {
            printf("Child triangle\n");
            const Triangle& childTriangle = childSurface[j];
            Triangle& newTriangle = newSurface.allocateTriangle();

            for (int k = 0; k < 3; ++k) {
                Vector4r transformedPoint = affine * childTriangle[k];
                std::cout << "Transformed " << childTriangle[k] << " to " << transformedPoint << std::endl;
                newTriangle.modifyPoint(k) = transformedPoint;
            }
        }
    }
}

void Rotate::compute_affine() {
    // Assume the axis is already normalized

    // Get rational approximation of sin and cos
    FIII rational_angle = find_rational_angle(angle);
    real cos_theta(rational_angle.c, rational_angle.d); // cos = run / hypotenuse
    real sin_theta(rational_angle.b, rational_angle.d); // sin = rise / hypotenuse

    // Compute rotation matrix using Rodrigues' rotation formula
    Matrix4r& rot = modifyAffine();
    rot.setIdentity();

    real one_minus_cos = real(1) - cos_theta;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (i == j) {
                rot(i, i) = cos_theta + axis[i] * axis[i] * one_minus_cos;
            } else {
                int k = 3 - i - j; // Third index (0, 1, or 2)
                real v = axis[i] * axis[j] * one_minus_cos;
                real s = axis[k] * sin_theta;
                rot(i, j) = v + ((i - j + 4) % 3 - 1) * s; // +s if (i,j) is (0,1), (1,2), or (2,0), else -s
            }
        }
    }
}

}
