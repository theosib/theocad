#include "geometry.hpp"
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
#include "bodies.hpp"

int main(int argc, char *argv[]) {
    std::cout << "Initializing app\n";
    QApplication app(argc, argv);

    // Create your solid here
    std::cout << "Creating solid\n";
    theocad::SolidPtr solid = theocad::globalUnitCubePtr;  // For now, just use the unit cube

    std::cout << "Creating visualizer\n";
    theocad::CADVisualizer visualizer(solid);
    std::cout << "Resize\n";
    visualizer.resize(800, 600);
    std::cout << "Show\n";
    visualizer.show();

    std::cout << "Exec\n";
    return app.exec();
}