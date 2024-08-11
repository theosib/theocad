#include "collections.hpp"

namespace theocad {
    
void Boolean::sliceTriangles() {
    // Cut a by b
    sliceTriangles(a, b, a_cut_surfaces);
    // Cut b by a
    sliceTriangles(b, a, b_cut_surfaces);
}

void Boolean::sliceTriangles(SurfacePtr p, SurfacePtr q, std::vector<Surface>& p_cut_surfaces) {    
    p_cut_surfaces.clear();
    
    // Iterate surfaces of p
    for (int psi = 0; psi < p->size(); psi++) {
        const Surface& p_surface = (*p)[i];
                
        // Allocate surface
        int ix = p_cut_surfaces.size();
        p_cut_surfaces.resize(ix + 1);
        Surface& p_new_surface(p_cut_surfaces[ix]);
        p_new_surface.invalidate();
        
        // Iterate over q's surfaces
        for (int qsi = 0; sqi < q->size(); sqi++) {
            const Surface& q_surface = (*q)[i];
            
            // Cut up p's surfaces according to q
            sliceTriangles(p_surface.mesh, q_surface.mesh, p_new_surface.mesh);
        }        
    }
}

void Intersection::computeBoolean() {
    surfaces.clear();
    
    // Iterate a's surfaces
    for (const Surface& as : a_cut_surfaces) {
        // Iterate a's triangles
        for (const Triangle& a_trian : as) {
            // If center is inside b, include the triangle
            bool inside = b.inside(as.center());
            if (inside) surfaces.push_back(as);
        }
    }

    // Iterate b's surfaces
    for (const Surface& bs : b_cut_surfaces) {
        // Iterate a's triangles
        for (const Triangle& b_trian : bs) {
            // If center is inside b, include the triangle
            bool inside = a.inside(bs.center());
            if (inside) surfaces.push_back(bs);
        }
    }
}
    
}