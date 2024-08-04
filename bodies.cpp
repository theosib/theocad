#include "bodies.hpp"
#include <iostream>

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
        /*{0, 1, 2, 3}, // Bottom face
        {4, 5, 6, 7}, // Top face
        {0, 1, 5, 4}, // Front face
        {2, 3, 7, 6}, // Back face
        {0, 3, 7, 4}, // Left face
        {1, 2, 6, 5}  // Right face*/        
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

        Triangle& t2 = surface.allocateTriangle();
        t2.modifyPoint(0) = vertices[faces[i][0]];
        t2.modifyPoint(1) = vertices[faces[i][2]];
        t2.modifyPoint(2) = vertices[faces[i][3]];
    }
}

// In the cpp file, define the global instances
SolidPtr globalUnitCubePtr = std::make_shared<UnitCube>();


} // namespace theocad