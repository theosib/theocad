#include "collections.hpp"

namespace theocad {
    
void Boolean::sliceTriangles() {
    // Cut a by b
    sliceTriangles(a, b, a_cut_surfaces);
    // Cut b by a
    sliceTriangles(b, a, b_cut_surfaces);
}

void Boolean::sliceTriangles(SolidPtr p, SolidPtr q, std::vector<Surface>& p_cut_surfaces) {    
    p_cut_surfaces.clear();
    
    // Iterate surfaces of p
    for (int psi = 0; psi < p->size(); psi++) {
        const Surface& p_surface = (*p)[psi];
                
        // Allocate surface
        int ix = p_cut_surfaces.size();
        p_cut_surfaces.resize(ix + 1);
        Surface& p_new_surface(p_cut_surfaces[ix]);
        p_new_surface.invalidate();
        
        // Iterate over q's surfaces
        for (int qsi = 0; qsi < q->size(); qsi++) {
            const Surface& q_surface = (*q)[qsi];
                                                
            // Cut up p's surfaces according to q
            theocad::sliceTriangles(p_surface.getMesh(), q_surface.getMesh(), p_new_surface.setMesh());
        }        
    }
}

void Intersection::computeBoolean() {
    check_slices();
    
    // Store the result in the base class (Solid)
    clearSurfaces();
    
    // Iterate a's surfaces
    for (const Surface& as : a_cut_surfaces) {
        Surface a_surf(allocateSurface());
        // Iterate a's triangles
        for (const Triangle& a_trian : as.getMesh()) {
            // If center is inside b, include the triangle
            bool inside = b->inside(a_trian.center());
            if (inside) a_surf.allocateTriangle() = a_trian;
        }
    }

    // Iterate b's surfaces
    for (const Surface& bs : b_cut_surfaces) {
        Surface b_surf(allocateSurface());
        // Iterate a's triangles
        for (const Triangle& b_trian : bs.getMesh()) {
            // If center is inside b, include the triangle
            bool inside = a->inside(b_trian.center());
            if (inside) b_surf.allocateTriangle() = b_trian;
        }
    }
    
    // TODO: Identify and eliminate identical triangles
}
    
}