#include "geometry.hpp"
#include "transforms.hpp"
#include <iostream>

using namespace theocad;

#if 0
int main()
{
    // Line line({Point(0, 0, 0), Point(1, 1, 1)});
    // Plane plane({1, 1, 1, -1});  // x + y + z = 1
    // Triangle triangle({Point(0, 0, 0), Point(1, 0, 0), Point(0, 1, 0)});
    //
    // Intersection planeIntersection = intersect(line, plane);
    // Intersection triangleIntersection = intersect(line, triangle);
    //
    // std::cout << planeIntersection << std::endl;
    // std::cout << triangleIntersection << std::endl;
    
    // Line line1(Point(0, 0, 0), Point(1, 1, 0));
    // Line line2(Point(1, 0, 0), Point(0, 1, 0));
    // LineIntersection inter = lineIntersection(line1, line2);
    // std::cout << inter << std::endl;
    
    
    Triangle trian1(Point(0, 0, 0), Point(1, 1, 0), Point(0, 0, 1));
    Triangle trian2(Point(1, 0, 0), Point(0, 1, 0), Point(0, 1, 1));
    Line inter = planeIntersection(trian1.getPlane(), trian2.getPlane());
    std::cout << inter << std::endl;
}
#endif

#include <QApplication>
#include "cad_visualizer.hpp"
#include "collections.hpp"

#if 0
int main() {
    real sorta_zero;
    int64_t actually_zero;
    
    actually_zero = 0;
    sorta_zero.assign(0, -2341624772761848151);
    
    std::cout << sorta_zero << std::endl;
    
    std::cout << "a=b: " << (sorta_zero == 0) << std::endl;
    std::cout << "b=a: " << (0 == sorta_zero) << std::endl;
}
#endif

#if 1
int main(int argc, char *argv[]) {
    std::cout << "Initializing app\n";
    QApplication app(argc, argv);

    // Create your solid here
    std::cout << "Creating solid\n";
    theocad::SolidPtr cyl = theocad::globalUnitCylinderPtr;
    theocad::SolidPtr cube = theocad::globalUnitCubePtr;
    std::shared_ptr<theocad::Translate> trans = std::make_shared<theocad::Translate>();
    trans->setShift(theocad::Vector(0, 0, 0));
    trans->setChild(cube);
    
    // std::shared_ptr<theocad::Collection> col = std::make_shared<theocad::Collection>();
    // col->addChild(cyl);
    // col->addChild(trans);

    std::shared_ptr<theocad::Intersection> col = std::make_shared<theocad::Intersection>();
    col->setChildA() = (cyl);
    col->setChildB() = (trans);

#if 0
    // Create a Transform object
    std::cout << "Creating transform\n";
    std::shared_ptr<theocad::Transform> transform = std::make_shared<theocad::Transform>();

    // Set the cylinder as the child of the transform
    std::cout << "Setting child\n";
    transform->modifyChild() = solid;

    // Create a 90-degree rotation matrix around the x-axis
    std::cout << "Creating rotation matrix\n";
    theocad::Matrix4r rotationMatrix;
    rotationMatrix.setIdentity();
    
    // Assuming we're using rational numbers, we'll use 0 and 1 for sin and cos
    // sin(90°) = 1, cos(90°) = 0
    // rotationMatrix(1, 1) = 0;  // cos(90°)
    // rotationMatrix(1, 2) = -1; // -sin(90°)
    // rotationMatrix(2, 1) = 1;  // sin(90°)
    // rotationMatrix(2, 2) = 0;  // cos(90°)
    // rotationMatrix(1,1) = 2;
    // rotationMatrix(2,2) = 2;
    // rotationMatrix(3,3) = 2;
    // rotationMatrix(0,0) = 2;

    // Apply the rotation matrix to the transform
    std::cout << "Applying rotation\n";
    transform->modifyAffine() = rotationMatrix;
#endif
    
    // std::shared_ptr<theocad::Rotate> rotate = std::make_shared<theocad::Rotate>();
    // rotate->modifyChild() = solid;
    // rotate->setAngle(45);
    // rotate->modifyAxis() = theocad::Vector(0, 1, 0);
    //
    // // Print out some information about the transformed solid
    // std::cout << "Transformed solid information:\n";
    // std::cout << "Number of surfaces: " << rotate->size() << std::endl;

    std::cout << "Creating visualizer\n";
    theocad::CADVisualizer visualizer(col);
    std::cout << "Resize\n";
    visualizer.resize(800, 600);
    std::cout << "Show\n";
    visualizer.show();

    std::cout << "Exec\n";
    return app.exec();
}
#endif