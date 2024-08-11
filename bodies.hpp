#ifndef INCLUDED_BODIES_HPP
#define INCLUDED_BODIES_HPP

#include "geometry.hpp"
#include <memory>
#include <iostream>

namespace theocad {
    
class Surface {
protected:
    std::string name;
    std::vector<Triangle> mesh;
    Plane averagePlane;
    bool averagePlane_valid = false;
    // XXX bool planar
    
    void computeAveragePlane();
    
public:
    
    void invalidate() {
        averagePlane_valid = false;
    }
    
    Triangle& allocateTriangle() {
        averagePlane_valid = false;
        int ix = mesh.size();
        mesh.resize(ix+1);
        return mesh[ix];
    }
    
    const Triangle& operator[](int ix) const {
        return mesh[ix];
    }
    
    Triangle& modifyTriangle(int ix) {
        averagePlane_valid = false;
        return mesh[ix];
    }
    
    int size() const { return mesh.size(); }
    
    void deleteTriangle(int ix) {
        int last = mesh.size() - 1;
        if (ix < last) {
            mesh[ix] = mesh[last];
        }
        mesh.resize(last);
        averagePlane_valid = false;
    }
    
    const Plane& getAveragePlane() const {
        if (!averagePlane_valid) {
            Surface& self(const_cast<Surface&>(*this));
            self.computeAveragePlane();
        }
        return averagePlane;
    }
    
    const Plane& getFace() const {
        return getAveragePlane();
    }
};

class Solid;
using SolidPtr = std::shared_ptr<Solid>;

class Solid {
protected:
    std::string name;
    std::vector<Surface> surfaces;
    
public:
    virtual ~Solid() {}
    
    Surface& allocateSurface() {
        std::cout << "Allocating surface\n";
        int ix = surfaces.size();
        surfaces.resize(ix+1);
        return surfaces[ix];
    }
    
    virtual const Surface& operator[](int ix) const {
        std::cout << "Solid getting surface\n";
        return surfaces[ix];
    }
    
    Surface& modifySurface(int ix) {
        return surfaces[ix];
    }
    
    virtual int size() const { return surfaces.size(); }
    
    void deleteSurface(int ix) {
        int last = surfaces.size() - 1;
        if (ix < last) {
            surfaces[ix] = surfaces[last];
        }
        surfaces.resize(last);        
    }
    
    virtual bool inside(const Vector4r& p) = 0;
};

// A unit cube with opposing corners at <0,0,0> and <1,1,1>
class UnitCube : public Solid {
public:
    UnitCube();
    virtual bool inside(const Vector4r& p);
};

class UnitCylinder : public Solid {
public:
    UnitCylinder();
    virtual bool inside(const Vector4r& p);
};


// class Collection : public Solid {

extern SolidPtr globalUnitCubePtr;
extern SolidPtr globalUnitCylinderPtr;
    
} // namespace theocad


#endif
