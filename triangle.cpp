
#include "geometry.hpp"
#include <iostream>

namespace theocad {


bool cutTriangleByPlane(const Triangle& p, LineIntersection *p_plane_intersections, std::vector<Triangle>& result) {
    // Check for plane_intersection being parallel to sides of p
    for (int i=0; i<3; i++) {
        int j = (i+2)%3;
        // End of one edge and start of the second edge after
        if (p_plane_intersections[i].exists && p_plane_intersections[j].exists && p_plane_intersections[i].t[0] == 1 && p_plane_intersections[j].t[0] == 0) return false;
    }
    
    // Now that we have all the interesections of plane_intersection with p, we can cut up p into pieces
    
    // Look for cases where the line cuts through a corner and also the opposite side
    for (int i=0; i<3; i++) {
        int j = (i+1)%3;
        int k = (i+2)%3;
        // Check to see if we're at the end of line i and the start of the next line (j) and also that the third side (k) is cut
        if (p_plane_intersections[i].exists && p_plane_intersections[j].exists && p_plane_intersections[k].exists && p_plane_intersections[i].t[0] == 1 && p_plane_intersections[j].t[0] == 0 && p_plane_intersections[k].inside_line[0]) {
            // Get the intersection point of the third side (k)
            Vector4r extra = p_plane_intersections[k].point[0];
            Triangle t1(p[j], p[k], extra);
            Triangle t2(extra, p[i], p[j]);
            if (t1.isValid()) result.push_back(t1);
            if (t2.isValid()) result.push_back(t2);
            return true;
        }
    }
    
    // At this point, the interesection line must cut through two of the triangle edges    
    for (int i=0; i<3; i++) {
        int j = (i+1)%3;
        //int k = (i+2)%3;
        if (p_plane_intersections[i].exists && p_plane_intersections[j].exists && p_plane_intersections[i].inside_line[0] && p_plane_intersections[j].inside_line[0]) {
            std::cout << "Cuts two i=" << i << " j=" << j << "\n";
            Triangle t1(p_plane_intersections[i].point[0], p[1], p_plane_intersections[j].point[0]);
            Triangle t2(p[0], p_plane_intersections[i].point[0], p_plane_intersections[j].point[0]);
            Triangle t3(p[0], p_plane_intersections[j].point[0], p[2]);
            if (t1.isValid()) result.push_back(t1);
            if (t2.isValid()) result.push_back(t2);
            if (t3.isValid()) result.push_back(t3);
            return true;
        }
    }
    
    // Shouldn't get here
    // throw std::runtime_error("cut fail");
    std::cout << "All triangle cut cases failed\n";
    return false;
}

bool sliceTriangleByEdge(const Triangle& p, const Line& q_edge, std::vector<Triangle>& result) {
    
    std::cout << "Cutting " << p << " with " << q_edge << std::endl;
    
    // Look for places where p it interesected by an edge of q
    LineIntersection p_plane_intersections[3];
    bool plane_intersects_p = false;
    for (int i=0; i<3; i++) {
        Line p_edge = p.getEdge(i);
        LineIntersection& inter(p_plane_intersections[i]);
        inter = lineIntersection(p_edge, q_edge);
        // TODO add sanity check
        if (inter.exists && inter.inside_line[0]) plane_intersects_p = true;
    }
    if (!plane_intersects_p) {
        // Add p to result
        return false;
    }

    // Check for plane_intersection being parallel to sides of p
    for (int i=0; i<3; i++) {
        int j = (i+2)%3;
        // End of one edge and start of the second edge after
        if (p_plane_intersections[i].exists && p_plane_intersections[j].exists && p_plane_intersections[i].t[0] == 1 && p_plane_intersections[j].t[0] == 0) return false;
    }
    
    return cutTriangleByPlane(p, p_plane_intersections, result);
}

// Cut a triangle with a coplanar triangle. We treat all three edges of the cutting triangle as cutting planes.
bool sliceTriangleCoplanar(const Triangle& p, const Triangle& q, std::vector<Triangle>& result) {
    // Check if any point of p is in q or any point of q is inside p. If not, then return p.
    if (!p.overlaps(q)) {
        return false;
        // result.push_back(p);
        // return;
    }
    
    // If p is entirely contained in q, then nothing to do to p.
    // This will also catch cases where p and q are identical.
    // p will cause q to get cut up later.
    if (q.contains(p)) {
        return false;
        // result.push_back(p);
        // return;
    }
    
    // At this point, p and q overlap and some of p is outside of q.
    // We're going to slice up p according to all sides of q.
    
    std::vector<Triangle> src[2];
    bool which_source = 0;
    src[0].push_back(p);
    for (int i=0; i<3; i++) {
        src[!which_source].clear();
        // Get an edge of q by which to cut all triangles of p
        Line q_edge = q.getEdge(i);
        // Iterate over all triangles of p
        for (const auto& pt : src[which_source]) {
            bool ok = sliceTriangleByEdge(pt, q_edge, src[!which_source]);
            if (!ok) src[!which_source].push_back(pt);
        }
        which_source = !which_source;
    }
    
    result.insert(result.end(), src[which_source].begin(), src[which_source].end());
    return true;
}

// Slice a triangle with the plane of another triangle. There will be at most one cutting plane.
bool sliceTriangleNoncoplanar(const Triangle& p, const Triangle& q, std::vector<Triangle>& result) {
    // Compute line of intersection between p and q
    std::cout << "Triangle p: " << p << std::endl;
    std::cout << "Triangle q: " << q << std::endl;
    const Plane& plane_p = p.getPlane();
    const Plane& plane_q = q.getPlane();
    Line plane_intersection = planeIntersection(plane_p, plane_q);
    
    std::cout << "Intersecting line: " << plane_intersection << std::endl;
    
    // First check to be sure this line touches q. Since q is the slicer, if the slicing line is outside of q, then q 
    // isn't cutting p.
    LineIntersection q_plane_intersections[3];
    bool plane_intersects_q = false;
    for (int i=0; i<3; i++) {
        // Get an edge and intersect with the plane intersection
        Line q_edge = q.getEdge(i);
        LineIntersection& inter(q_plane_intersections[i]);
        inter = lineIntersection(q_edge, plane_intersection);
        // TODO add sanity check
        
        std::cout << "Q Edge=" << q_edge << " inter=" << inter << std::endl;
        
        // The edge of q is line 0, so we check to see if the cutting plane cuts that edge
        if (inter.exists && inter.inside_line[0]) plane_intersects_q = true;
    }
    // If the cutting plane doesn't touch p, keep all of p.
    if (!plane_intersects_q) {
        // Add p to result
        return false;
    }
    
    // Check for plane_intersection being parallel to sides of q.
    // The cutting plane is coincident to a side if it goes through end points of the other lines
    for (int i=0; i<3; i++) {
        // 'i' is a side of the triangle, and we want to check its end point
        // 'j' the side after the nest, and we want to check its start point.
        int j = (i+2)%3;
        // End of one edge and start of the second edge after
        if (q_plane_intersections[i].exists && q_plane_intersections[j].exists && q_plane_intersections[i].t[0] == 1 && q_plane_intersections[j].t[0] == 0) return false;
    }
    
    // Similarly, check to make sure that the interesection line cuts p
    LineIntersection p_plane_intersections[3];
    bool plane_intersects_p = false;
    for (int i=0; i<3; i++) {
        Line p_edge = p.getEdge(i);
        LineIntersection& inter(p_plane_intersections[i]);
        inter = lineIntersection(p_edge, plane_intersection);
        std::cout << "P Edge=" << p_edge << " inter=" << inter << std::endl;
        
        // TODO add sanity check
        if (inter.exists && inter.inside_line[0]) plane_intersects_p = true;
    }
    if (!plane_intersects_p) {
        // Add p to result
        return false;
    }
    
    return cutTriangleByPlane(p, p_plane_intersections, result);
}
    


void sliceTriangle(const Triangle& p, const Triangle& q, std::vector<Triangle>& result) {
    std::cout << "Slicing " << p << " by " << q << std::endl;
    std::cout << "Pnormal=" << p.getNormal() << " Qnormal=" << q.getNormal() << std::endl;
    if (p.parallelTo(q)) {
        std::cout << "Parallel\n";
        if (p.coplanar(q)) {
            std::cout << "Coplanar\n";
            if (!sliceTriangleCoplanar(p, q, result)) result.push_back(p);
        } else {
            std::cout << "Just parallel\n";
            result.push_back(p);
        }
    } else {
        std::cout << "Noncoplanar\n";
        if (!sliceTriangleNoncoplanar(p, q, result)) result.push_back(p);
    }
    std::cout << "Result:";
    for (const auto& i : result) {
        std::cout << " " << i;
    }
    std::cout << std::endl;
}

void sliceTriangles(const std::vector<Triangle>& A, const std::vector<Triangle>& B, std::vector<Triangle>& result) {    
    // Iterate over all triangles in A.
    int i = 0, j;
    for (const Triangle& p_init : A) {
        int src = 0;
        std::vector<Triangle> p[2];
        // Start off with just one triangle from A
        p[src].push_back(p_init);
        // Iterate over all triangles in B
        j = 0;
        for (const Triangle& q : B) {
            std::cout << " p" << i << " q" << j << std::endl;
            p[!src].clear();
            // Slice each triangle in our temp list against q
            for (const Triangle& r : p[src]) {
                // Slice r by q, putting pieces into p
                sliceTriangle(r, q, p[!src]);
            }
            // Swap lists
            src = !src;
            j++;
        }
        // The sliced p should be in p[src]
        result.insert(result.end(), p[src].begin(), p[src].end());
        i++;
    }
    
#if 0
    // Iterate over all triangles in B.
    for (const Triangle& p_init : B) {
        int src = 0;
        std::vector<Triangle> p[2];
        // Start off with just one triangle from B
        p[src].push_back(p_init);
        // Iterate over all triangles in A
        for (const Triangle& q : A) {
            p[!src].clear();
            // Slice each triangle in our temp list against q
            for (const Triangle& r : p[src]) {
                // Slice r by q, putting pieces into p
                sliceTriangle(r, q, p[!src]);
            }
            // Swap lists
            src = !src;
        }
        // The sliced p should be in p[src]
        result.insert(result.end(), p[src].begin(), p[src].end());
    }
#endif
    
    // Boolean will have to find and deal with identical triangles
}

} // namespace theocad
