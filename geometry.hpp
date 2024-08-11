#ifndef INCLUDED_GEOMETRY_HPP
#define INCLUDED_GEOMETRY_HPP

#include <eigen3/Eigen/Dense>
#include <boost/rational.hpp>
#include <cstdint>

/*
Notes:
- serialization
- Factory for more primitives
- 
*/

namespace theocad {

using real = boost::rational<int64_t>;
using Vector4r = Eigen::Matrix<real, 4, 1>;
using Matrix4r = Eigen::Matrix<real, 4, 4>;

inline Vector4r Point(real x, real y, real z) { return Vector4r(x, y, z, 1); }
inline Vector4r Point() { return Point(0, 0, 0); }
inline Vector4r Vector(real x, real y, real z) { return Vector4r(x, y, z, 0); }
inline Vector4r Vector() { return Vector(0, 0, 0); }

inline std::ostream& operator<<(std::ostream& os, const Vector4r& i) {
    os << '<';
    os << boost::rational_cast<float>(i[0]) << ',';
    os << boost::rational_cast<float>(i[1]) << ',';
    os << boost::rational_cast<float>(i[2]) << ',';
    os << boost::rational_cast<float>(i[3]) << '>';
    return os;
}

inline real dot(const Vector4r& a, const Vector4r& b) {
    return a.head<3>().dot(b.head<3>());
}

inline Vector4r cross(const Vector4r& a, const Vector4r& b) {
    return Vector(a.y()*b.z() - a.z()*b.y(),
                  a.z()*b.x() - a.x()*b.z(),
                  a.x()*b.y() - a.y()*b.x());
}

inline real magnitudeSquared(const Vector4r& v) {
    return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
}

real rational_sqrt(real n, real threshold);

struct Plane {
    real c[4]; // coefficients
    
    Plane() : c{0, 0, 0, 0} {}
    Plane(real a, real b, real c, real d) : c{a, b, c, d} {}
    Plane(const Vector4r& normal, real d) : c{normal[0], normal[1], normal[2], d} {}
    
    void compute(const Vector4r *points);
    
    real signedDistanceNumerator(const Vector4r& point) const {
        return c[0]*point[0] + c[1]*point[1] + c[2]*point[2] + c[3];
    }
    
    Vector4r getNormal() const {
        return Vector(c[0], c[1], c[2]);
    }
};

struct Line {
    Vector4r p[2];
    
    Line() {}
    Line(const Vector4r& start, const Vector4r& end) {
        p[0] = start;
        p[1] = end;
    }
    
    Vector4r interpolate(real t) const {
        return p[0] + t * (p[1] - p[0]);
    }
    
    Vector4r direction() const { return p[1] - p[0]; }    
    
    real distanceSquaredToPoint(const Vector4r& point) const;
    
    Vector4r& operator[](int ix) { return p[ix]; }
    const Vector4r& operator[](int ix) const { return p[ix]; }
};

inline std::ostream& operator << (std::ostream& os, const Line& l) {
    os << l.p[0] << ":" << l.p[1];
    return os;
}

class Triangle {
    enum {
        PLANE_VALID = 1,
        NORMAL_VALID = 2
    };
    
    int valid = 0;
    Vector4r points[3];
    Plane plane;
    Vector4r normal;
    
    // struct Proxy {
    //     Triangle &t;
    //     int ix;
    //     Proxy(Triangle& t_in, int ix_in) : t(t_in), ix(ix_in) {}
    //     Vector4r& operator=(const Vector4r& x) {
    //         t.valid = 0;
    //         Vector4r& p(t.points[ix]);
    //         p = x;
    //         return p;
    //     }
    //     operator Vector4r() { return t.points[ix]; }
    // };
        
public:
    Triangle() = default;
    Triangle(const Vector4r& p1, const Vector4r& p2, const Vector4r& p3) {
        points[0] = p1;
        points[1] = p2;
        points[2] = p3;
        valid = 0;
    }
    
    Vector4r center() {
        Vector4r total = Vector(0, 0, 0);
        total += points[0];
        total += points[1];
        total += points[2];
        return total / 3;
    }
    
    // Proxy operator[](int ix) { return Proxy(*this, ix); }
    const Vector4r& operator[](int ix) const { return points[ix]; }
    Vector4r& modifyPoint(int ix) {
        valid = 0;
        return points[ix]; 
    }
    
    const Plane& getPlane() const {
        if (!(valid & PLANE_VALID)) {
            // Memoize the plane
            Triangle& self(const_cast<Triangle&>(*this));
            self.plane.compute(points);
            self.valid |= PLANE_VALID;
        }
        return plane;
    }
    
    Vector4r getNormal() const {
        return getPlane().getNormal();
    }
    
    bool containsPoint(const Vector4r& p) const;
    
    Line getSide(int a) const {
        int b = (a+1) % 3;
        return Line(points[a], points[b]);
    }
    Line getEdge(int a) const { return getSide(a); }
    
    bool parallelTo(const Triangle& that) const {
        Vector4r this_normal = getNormal();
        Vector4r that_normal = that.getNormal();
        Vector4r cross_product = cross(this_normal, that_normal);
        real cross_magnitude_squared = dot(cross_product, cross_product);
        return cross_magnitude_squared == 0;
    }
    
    bool coplanar(const Triangle& that) const {
        // First, check if the triangles are parallel
        if (!parallelTo(that)) return false;

        // If they're parallel, check if a point from t2 lies on the plane of t1
        Plane plane = getPlane();
        Vector4r point_on_t2 = that[0];  // Take any point from t2

        // Compute the signed distance from the point to the plane
        real distance = plane.signedDistanceNumerator(point_on_t2);
        return distance == 0;
    }
    
    // Only valid if triangles are coplanar
    bool overlaps(const Triangle& that) const {
        for (int i=0; i<3; i++) {
            if (containsPoint(that.points[i])) return true;
        }
        for (int i=0; i<3; i++) {
            if (that.containsPoint(points[i])) return true;
        }
        return false;
    }
    
    // Only valid if triangles are coplanar
    // Checks if 'that' is completely contained in this.
    bool contains(const Triangle& that) const {
        for (int i=0; i<3; i++) {
            if (!containsPoint(that.points[i])) return false;
        }
        return true;
    }
    
    bool operator==(const Triangle& that) const {
        if (points[0] == that.points[0]) {
            if (points[1] == that.points[1] && points[2] == that.points[2]) return true;
            if (points[1] == that.points[2] && points[2] == that.points[1]) return true;
            return false;
        } else if (points[0] == that.points[1]) {
            if (points[1] == that.points[0] && points[2] == that.points[2]) return true;
            if (points[1] == that.points[2] && points[2] == that.points[0]) return true;
            return false;
        } else if (points[0] == that.points[2]) {
            if (points[1] == that.points[0] && points[2] == that.points[1]) return true;
            if (points[1] == that.points[1] && points[2] == that.points[0]) return true;
            return false;
        }
        return false;
    }
};

// Invalid if coplanar or parallel
Line planeIntersection(const Plane& plane1, const Plane& plane2);

struct LineIntersection {
    real t[2]; // Intersection parameters for lines a and b
    bool inside_line[2]; // True if the parameter lies within the bounds of a and b
    std::vector<Vector4r> point; // Actual points of intersection    
    bool exists, coplanar, skew, coincident;
    
    LineIntersection() : t{real(), real()}, inside_line{false, false}, exists(false), coplanar(false), skew(false), coincident(false) {}
};

// LineTriangleIntersection intersect(const Line& line, const Triangle& triangle);
LineIntersection lineIntersection(const Line& a, const Line& b);


inline std::ostream& operator<<(std::ostream& os, const LineIntersection& i) {
    os << "exists=" << i.exists << " coincident=" << i.coincident << " skew=" << i.skew << " coplanar=" << i.coplanar << " in_line=" << i.inside_line[0] << "," << i.inside_line[1] << " t=" << i.t[0] << "," << i.t[1];
    os << " point=";
    bool prior_point = false;
    for (auto const& p : i.point) {
        if (prior_point) os << ",";
        os << p;
        prior_point = true;
    }
    return os;
}

} // namespace theocad


#endif
