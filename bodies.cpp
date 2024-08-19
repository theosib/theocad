#include "bodies.hpp"
#include "transforms.hpp"
#include <iostream>
#include "rational_circle.hpp"
#include <stdexcept>

namespace theocad {


Vector4r sumNormals(const std::vector<Triangle>& triangles) {
    Vector4r sumNormal(0, 0, 0, 0);

    for (const Triangle& triangle : triangles) {
        // Vector4r edge1 = triangle[1] - triangle[0];
        // Vector4r edge2 = triangle[2] - triangle[0];
        // Vector4r normal = cross(edge1, edge2);
        // sumNormal += normal;
        sumNormal += triangle.getNormal();
    }

    return sumNormal;
}

void Surface::computeAveragePlane() {
    Vector4r sumNormal = sumNormals(mesh);
    
    // Compute centroid
    Vector4r centroid(0, 0, 0, 0);
    int totalVertices = 0;
    for (const Triangle& triangle : mesh) {
        for (int i = 0; i < 3; ++i) {
            centroid += triangle[i];
        }
        totalVertices += 3;
    }
    centroid /= totalVertices;

    // Define plane
    real A = sumNormal[0];
    real B = sumNormal[1];
    real C = sumNormal[2];
    real D = -(A * centroid[0] + B * centroid[1] + C * centroid[2]);

    averagePlane = Plane(A, B, C, D);
    averagePlane_valid = true;
}

UnitCube::UnitCube() {
    // Define the vertices of the cube
    static Vector4r vertices[8] = {
        Point(0, 0, 0), Point(1, 0, 0), Point(1, 1, 0), Point(0, 1, 0),
        Point(0, 0, 1), Point(1, 0, 1), Point(1, 1, 1), Point(0, 1, 1)
    };

    // Define the faces of the cube
    static int faces[6][4] = {
        // {0, 1, 2, 3}, // Bottom face
        // {4, 5, 6, 7}, // Top face
        // {0, 1, 5, 4}, // Front face
        // {2, 3, 7, 6}, // Back face
        // {0, 3, 7, 4}, // Left face
        // {1, 2, 6, 5}  // Right face
        {0, 3, 2, 1}, // Bottom face (reversed)
        {4, 5, 6, 7}, // Top face (correct)
        {0, 1, 5, 4}, // Front face (correct)
        {2, 3, 7, 6}, // Back face (correct)
        {0, 4, 7, 3}, // Left face (reversed)
        {1, 2, 6, 5}  // Right face (correct)
    };

    // Create surfaces and triangles for each face
    for (int i = 0; i < 6; ++i) {
        Surface& surface = allocateSurface();
        
        // Create two triangles for each face
        Triangle& t1 = surface.allocateTriangle();
        t1.modifyPoint(0) = vertices[faces[i][0]];
        t1.modifyPoint(1) = vertices[faces[i][1]];
        t1.modifyPoint(2) = vertices[faces[i][2]];
        if (!t1.isValid()) {
            std::cout << "i=" << i << std::endl;
            std::cout << t1 << std::endl;
            throw std::runtime_error("bad triangle");
        }

        Triangle& t2 = surface.allocateTriangle();
        t2.modifyPoint(0) = vertices[faces[i][0]];
        t2.modifyPoint(1) = vertices[faces[i][2]];
        t2.modifyPoint(2) = vertices[faces[i][3]];
        if (!t2.isValid()) {
            std::cout << "i=" << i << std::endl;
            std::cout << t2 << std::endl;
            throw std::runtime_error("bad triangle");
        }
    }
}

bool UnitCube::inside(const Vector4r& p) {
    for (int i=0; i<3; i++) {
        if (p[i] < 0 || p[i] > 1) return false;
    }
    return true;
}

UnitCylinder::UnitCylinder() {
    int step = 5;
    
    Surface& top_surface = allocateSurface();
    for (int a=0; a<360; a+=step) {
        int b = a+step;
        FIII a_fiii = find_rational_angle(a);
        FIII b_fiii = find_rational_angle(b);
        Triangle& t = top_surface.allocateTriangle();
        t.modifyPoint(0) = Point(real(a_fiii.c, a_fiii.d), real(a_fiii.b, a_fiii.d), 1);
        t.modifyPoint(1) = Point(real(b_fiii.c, b_fiii.d), real(b_fiii.b, b_fiii.d), 1);
        t.modifyPoint(2) = Point(0, 0, 1);
        if (!t.isValid()) throw std::runtime_error("top surface");
    }
    Surface& bot_surface = allocateSurface();
    for (int a=0; a<360; a+=step) {
        int b = a-step;
        FIII a_fiii = find_rational_angle(a);
        FIII b_fiii = find_rational_angle(b);
        Triangle& t = bot_surface.allocateTriangle();
        t.modifyPoint(0) = Point(real(a_fiii.c, a_fiii.d), real(a_fiii.b, a_fiii.d), 0);
        t.modifyPoint(1) = Point(real(b_fiii.c, b_fiii.d), real(b_fiii.b, b_fiii.d), 0);
        t.modifyPoint(2) = Point(0, 0, 0);
        if (!t.isValid()) throw std::runtime_error("bot surface");
    }
    Surface& outer_surface = allocateSurface();
    for (int a=0; a<360; a+=step) {
        int b = a+step;
        FIII a_fiii = find_rational_angle(a);
        FIII b_fiii = find_rational_angle(b);
        Triangle& t1 = outer_surface.allocateTriangle();
        t1.modifyPoint(0) = Point(real(b_fiii.c, b_fiii.d), real(b_fiii.b, b_fiii.d), 1);
        t1.modifyPoint(1) = Point(real(a_fiii.c, a_fiii.d), real(a_fiii.b, a_fiii.d), 1);
        t1.modifyPoint(2) = Point(real(a_fiii.c, a_fiii.d), real(a_fiii.b, a_fiii.d), 0);
        if (!t1.isValid()) throw std::runtime_error("side surface");
        Triangle& t2 = outer_surface.allocateTriangle();
        t2.modifyPoint(0) = Point(real(a_fiii.c, a_fiii.d), real(a_fiii.b, a_fiii.d), 0);
        t2.modifyPoint(1) = Point(real(b_fiii.c, b_fiii.d), real(b_fiii.b, b_fiii.d), 0);
        t2.modifyPoint(2) = Point(real(b_fiii.c, b_fiii.d), real(b_fiii.b, b_fiii.d), 1);
        if (!t2.isValid()) throw std::runtime_error("side surface");
    }
}

bool UnitCylinder::inside(const Vector4r& p) {
    // Check the vertical dimension
    if (p[2] < 0 || p[2] > 1) return false;
    
    // Simple check if the point it outside of an ideal circle
    // The vertices of the outer curve lie on an idea circle, which means
    // that the edges lie slightly inside. We can therefore accurately exclude points
    // that lie outside of the idea circle.
    real sqd = p[0]*p[0] + p[1]*p[1];
    if (sqd > 1) return false;
    
    // Ray from center out to point
    Line ray(Point(0, 0, 0), Point(p[0], p[1], 0));
    
    // Iterate all the edges of the bottom triangle looking to see if a
    // line from the center intersects an outer edge
    const Surface& bot(surfaces[0]);
    for (int i=0; i<bot.size(); i++) {
        const Triangle& tri(bot[i]);
        Line tri_edge = tri.getEdge(0);
        LineIntersection li = lineIntersection(ray, tri_edge);
        
        // Sanity check
        if (!li.exists) continue;
        if (!li.coplanar) std::cout << "Cylinder inclusion bug\n";
        
        // Skip this line if the triangle edge is not intersected
        if (!li.inside_line[1]) continue;
        
        // If the intersection is inside the ray, then the point is outside
        return !li.inside_line[0];
    }
    
    return false;
}

bool Transform::inside(const Vector4r& p) {
    return child->inside(getInverse() * p);
}


// In the cpp file, define the global instances
SolidPtr globalUnitCubePtr = std::make_shared<UnitCube>();
SolidPtr globalUnitCylinderPtr = std::make_shared<UnitCylinder>();


} // namespace theocad