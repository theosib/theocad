#include "geometry.hpp"
#include <iostream>

namespace theocad {
    
real rational_sqrt(real n, real threshold) {
    if (n < 0) return real();
    real low = 0, high = n;
    while (high - low > threshold) {
        real mid = (low + high) / 2;
        real val = mid * mid;
        if (val <= n) {
            low = mid;
        }
        else {
            high = mid;
        }
    }
    return high;
} 


bool Line::parallelTo(const Line& that) const {
    Vector4r dir1 = direction();
    Vector4r dir2 = that.direction();

    // Check if either line is actually a point
    if (dir1 == Vector(0,0,0) || dir2 == Vector(0,0,0)) {
        return false;
    }

    // Compute the cross product
    Vector4r cross_product = cross(dir1, dir2);

    // Check if all components of the cross product are exactly zero
    return cross_product == Vector(0,0,0);
}


void Plane::compute(const Vector4r *points) {
    Vector4r v1 = points[1] - points[0];
    Vector4r v2 = points[2] - points[0];
    Vector4r n = cross(v1, v2);
    c[0] = n.x();
    c[1] = n.y();
    c[2] = n.z();
    c[3] = -dot(n, points[0]);
    
    std::cout << "Compute plane pts=" << points[0] << "," << points[1] << "," << points[2] << " v1=" << v1 << " v2=" << v2 << " n=" << n << " ";
    
    std::cout << boost::rational_cast<float>(c[0]) << ' ' << boost::rational_cast<float>(c[1]) << ' ' << boost::rational_cast<float>(c[2])
        << ' ' << boost::rational_cast<float>(c[3]) << std::endl;
}

bool Triangle::containsPoint(const Vector4r& p) const {
    std::cout << "Checking if point " << p << " in " << *this << std::endl;
    // Implement barycentric coordinate test
    // This is a simplified version and may need adjustment for exact arithmetic
    Vector4r v0 = points[2] - points[0];
    Vector4r v1 = points[1] - points[0];
    Vector4r v2 = p - points[0];

    real d00 = dot(v0, v0);
    real d01 = dot(v0, v1);
    real d11 = dot(v1, v1);
    real d20 = dot(v2, v0);
    real d21 = dot(v2, v1);

    real denom = d00 * d11 - d01 * d01;
    real v = (d11 * d20 - d01 * d21) / denom;
    real w = (d00 * d21 - d01 * d20) / denom;
    real u = 1 - v - w;

    return (u >= 0) && (v >= 0) && (w >= 0);
}

real Line::distanceSquaredToPoint(const Vector4r& point) const {
    Vector4r v = p[1] - p[0];
    Vector4r w = point - p[0];
    real c1 = dot(w, v);
    real c2 = dot(v, v);
    real b_num = c1;
    real b_denom = c2;
    Vector4r pb = p[0] * b_denom + v * b_num;
    return magnitudeSquared((point * b_denom - pb));
}

// https://www.songho.ca/math/line/line.html#google_vignette
LineIntersection lineIntersection(const Line& a, const Line& b) {
    LineIntersection result;

    Vector4r da = a.direction();  // Direction vector of line a
    Vector4r db = b.direction();  // Direction vector of line b
    Vector4r r = a.p[0] - b.p[0];   // Vector between start points
    std::cout << "da=" << da << std::endl;
    std::cout << "db=" << db << std::endl;
    std::cout << "r=" << r << std::endl;

    Vector4r n = cross(da, db);     // Normal vector to both lines
    std::cout << "n=" << n << std::endl;
    real n_mag_sq = dot(n, n);      // Squared magnitude of n
    std::cout << "nmag=" << n_mag_sq << std::endl;

    if (n_mag_sq == 0) {
        printf("nmag is zero\n");
        // Lines are parallel
        Vector4r cross_r_db = cross(r, db);
        real cross_r_db_mag_sq = dot(cross_r_db, cross_r_db);
        // std::cout << "Parallel, cross=" << cross_r_db << " sqmag=" << cross_r_db_mag_sq << std::endl;
        
        if (cross_r_db_mag_sq == 0) {
            // Lines are coplanar and parallel
            result.coplanar = true;
            
            // Check if lines are coincident
            Vector4r cross_r_da = cross(r, da);
            if (dot(cross_r_da, cross_r_da) == 0) {
                result.coincident = true;
                result.exists = true;
                // Compute overlap if needed
            }
        } else {
            // Lines are skew (parallel but not coplanar)
            result.skew = true;
        }
        return result;
    }

    printf("nmag is NOT zero\n");
    
    // Lines are not parallel
    result.exists = true;
    result.coplanar = true;  // Non-parallel lines in 3D always lie in a plane

    // Compute intersection parameters
    //     Vector3 b = (q - p).cross(u);      // cross product
    //     float t = b.dot(a) / dot;
    result.t[0] = -dot(cross(r, db), n) / n_mag_sq;
    result.t[1] = -dot(cross(r, da), n) / n_mag_sq;
    std::cout << "t=" << result.t[0] << "," << result.t[1] << std::endl;

    // Check if intersection is within line segments
    result.inside_line[0] = (result.t[0] >= 0 && result.t[0] <= 1);
    result.inside_line[1] = (result.t[1] >= 0 && result.t[1] <= 1);

    // Compute intersection point
    Vector4r intersection_a = a.p[0] + result.t[0] * da;
    Vector4r intersection_b = b.p[0] + result.t[1] * db;
    result.point.push_back(intersection_a);
    result.point.push_back(intersection_b);
    
    std::cout << "inter=" << intersection_a << ", " << intersection_b << std::endl;

    // Check if the computed intersection points are close enough
    if (intersection_a != intersection_b) {
        // If intersection points are not close enough, lines are skew
        result.skew = true;
        result.coplanar = false;
        result.exists = false;
        result.inside_line[0] = false;
        result.inside_line[1] = false;
    }

    return result;
}

// Invalid if coplanar or parallel
Line planeIntersection(const Plane& plane1, const Plane& plane2) {
    Line result;

    // Normals of the planes
    Vector4r n1 = Vector(plane1.c[0], plane1.c[1], plane1.c[2]);
    Vector4r n2 = Vector(plane2.c[0], plane2.c[1], plane2.c[2]);

    std::cout << "n1=" << n1 << std::endl;
    std::cout << "n2=" << n2 << std::endl;

    // Direction of the intersection line
    Vector4r direction = cross(n1, n2);
    std::cout << "Direction: " << direction << std::endl;
    std::cout << "Dir:";
    for (int i=0; i<3; i++) std::cout << " " << direction[i];
    std::cout << std::endl;
    std::cout << "Abs:";
    for (int i=0; i<3; i++) std::cout << " " << abs(direction[i]);
    std::cout << std::endl;

    // Find a point on the intersection line
    // We'll use the method of choosing the largest component of the direction vector
    int i;
    if (abs(direction[0]) >= abs(direction[1]) && abs(direction[0]) >= abs(direction[2]))
        i = 0;
    else if (abs(direction[1]) >= abs(direction[2]))
        i = 1;
    else
        i = 2;

    int j = (i + 1) % 3;
    int k = (i + 2) % 3;
    
    result.p[0][i] = 0;
    result.p[0][j] = (plane1.c[k] * plane2.c[3] - plane2.c[k] * plane1.c[3]) / 
                      (plane1.c[j] * plane2.c[k] - plane2.c[j] * plane1.c[k]);
    result.p[0][k] = (plane2.c[j] * plane1.c[3] - plane1.c[j] * plane2.c[3]) / 
                      (plane1.c[j] * plane2.c[k] - plane2.c[j] * plane1.c[k]);
    result.p[0][3] = 1;  // Homogeneous coordinate
    result.p[1] = result.p[0] + direction;

    return result;
}

} // namespace theocad