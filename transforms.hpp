#ifndef INCLUDED_TRANSFORMS_HPP
#define INCLUDED_TRANSFORMS_HPP

#include "bodies.hpp"

namespace theocad {

class Transform : public Solid {
protected:
    SolidPtr child;
    Matrix4r affine, inverse;
    bool inverse_valid = false;
    bool cache_valid = false;

    void compute_inverse();
    void transform_child();

public:
    Transform() {
        affine.setIdentity();
        inverse.setIdentity();
        inverse_valid = true;
    }
    virtual ~Transform() {}

    virtual const Matrix4r& getAffine() const { return affine; }
    Matrix4r& modifyAffine() { 
        inverse_valid = false;
        cache_valid = false;
        return affine;
    }

    const Matrix4r& getInverse() const {
        if (!inverse_valid) {
            Transform& self(const_cast<Transform&>(*this));
            self.compute_inverse();
            self.inverse_valid = true;
        }
        return inverse;
    }

    const SolidPtr& getChild() const { return child; }
    SolidPtr& modifyChild() { 
        cache_valid = false;
        return child; 
    }
    void setChild(SolidPtr p) {
        cache_valid = false;
        child = p;
    }

    void check_cache() const {
        if (!cache_valid) {
            Transform& self(const_cast<Transform&>(*this));
            self.transform_child();
            self.cache_valid = true;
        }
    }

    virtual int size() const { 
        check_cache();
        return Solid::size(); 
    }

    virtual const Surface& operator[](int ix) const {
        check_cache();
        return surfaces[ix];
    }
};

class Rotate : public Transform {
protected:
    bool affine_valid = false;
    Vector4r axis;
    float angle;

    void compute_affine();

public:
    virtual ~Rotate() {}

    float getAngle() { return angle; }
    const Vector4r& getAxis() { return axis; }

    void setAngle(float a) {
        angle = a;
        affine_valid = false;
    }
    Vector4r& modifyAxis() { 
        affine_valid = false;
        return axis;
    }

    void check_affine() const {
        if (!cache_valid) {
            Rotate& self(const_cast<Rotate&>(*this));
            self.compute_affine();
            self.affine_valid = true;
        }
    }

    virtual int size() const { 
        check_affine();
        return Transform::size(); 
    }

    virtual const Surface& operator[](int ix) const {
        check_affine();
        return Transform::operator[](ix);
    }    
    
    virtual bool inside(const Vector4r& p);
};

class Translate : public Transform {
protected:
    Vector4r shift;
    bool affine_valid = false;

    void compute_affine() {
        Matrix4r& trans = modifyAffine();
        trans.setIdentity();
        trans(0, 3) = shift[0];
        trans(1, 3) = shift[1];
        trans(2, 3) = shift[2];
        affine_valid = true;
    }

    void check_affine() {
        if (!affine_valid) {
            compute_affine();
        }
    }

public:
    Translate(const Vector4r& s = Point(0, 0, 0)) : shift(s) {}

    virtual ~Translate() {}

    const Vector4r& getShift() const { return shift; }

    void setShift(const Vector4r& s) {
        shift = s;
        affine_valid = false;
    }

    Vector4r& modifyShift() {
        affine_valid = false;
        return shift;
    }

    virtual int size() const { 
        const_cast<Translate*>(this)->check_affine();
        return Transform::size(); 
    }

    virtual const Surface& operator[](int ix) const {
        const_cast<Translate*>(this)->check_affine();
        return Transform::operator[](ix);
    }
};

class Scale : public Transform {
protected:
    Vector4r factors;
    bool affine_valid = false;

    void compute_affine() {
        Matrix4r& scale = modifyAffine();
        scale.setIdentity();
        scale(0, 0) = factors[0];
        scale(1, 1) = factors[1];
        scale(2, 2) = factors[2];
        affine_valid = true;
    }

public:
    Scale(const Vector4r& f = Vector(1, 1, 1)) : factors(f) {}

    virtual ~Scale() {}

    const Vector4r& getFactors() const { return factors; }

    void setFactors(const Vector4r& f) {
        factors = f;
        affine_valid = false;
    }

    Vector4r& modifyFactors() {
        affine_valid = false;
        return factors;
    }

    void check_affine() {
        if (!affine_valid) {
            compute_affine();
        }
    }

    virtual int size() const { 
        const_cast<Scale*>(this)->check_affine();
        return Transform::size(); 
    }

    virtual const Surface& operator[](int ix) const {
        const_cast<Scale*>(this)->check_affine();
        return Transform::operator[](ix);
    }
};

} // namespace theocad

#endif
