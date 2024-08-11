#ifndef INCLUDED_COLLECTIONS_HPP
#define INCLUDED_COLLECTIONS_HPP

#include "bodies.hpp"

namespace theocad {
    
class Collection : public Solid {
    std::vector<SolidPtr> children;
public:
    void addChild(SolidPtr c) {
        children.push_back(c);
    }
    
    virtual const Surface& operator[](int ix) const {
        unsigned int n = 0;
        while (n<children.size() && ix >= children[n]->size()) ix -= children[n++]->size();
        return (*children[n])[ix];
    }
    
    virtual int size() const { 
        int n = 0;
        for (const auto& c : children) {
            std::cout << "Child has " << c->size() << " elements\n";
            n += c->size();
        }
        return n;
    }
};

class Boolean : public Solid {
    SolidPtr a, b;
    std::vector<Surface> a_cut_surfaces, b_cut_surfaces;
    bool cuts_valid = false;
    
    void sliceTriangles();    
    void sliceTriangles(SurfacePtr p, SurfacePtr q, std::vector<Surface>& p_cut_surfaces);
    
    void check_slices() {
        if (!cuts_valid) {
            sliceTriangles();
            cuts_valid = true;
        }
    }
    
public:
    SolidPtr& setChildA() { cuts_valid = false; return a; }
    SolidPtr& setChildB() { cuts_valid = false; return b; }
    
    virtual int size() const { 
        const_cast<Boolean*>(this)->check_slices();
        return Solid::size(); 
    }

    virtual const Surface& operator[](int ix) const {
        const_cast<Boolean*>(this)->check_slices();
        return Solid::operator[](ix);
    }
};

class Intersection : public Boolean {
    bool boolean_valid = false;
    
    void computeBoolean();
    
    void check_slices() {
        if (!boolean_valid) {
            computeBoolean();
            boolean_valid = true;
        }
    }
    
public:
    
    virtual int size() const { 
        check_cache();
        return Solid::size(); 
    }

    virtual const Surface& operator[](int ix) const {
        check_cache();
        return surfaces[ix];
    }
    
};

}

#endif