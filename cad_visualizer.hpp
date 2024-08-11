#ifndef INCLUDED_CAD_VISUALIZER_HPP
#define INCLUDED_CAD_VISUALIZER_HPP

#include <QMainWindow>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/QOrbitCameraController>
#include "bodies.hpp"

namespace theocad {

class CADVisualizer : public QMainWindow {
    Q_OBJECT

public:
    CADVisualizer(SolidPtr solid);

private:
    Qt3DExtras::Qt3DWindow *view;
    Qt3DCore::QEntity *rootEntity;
    Qt3DRender::QCamera *camera;
    Qt3DExtras::QOrbitCameraController *cameraController;

    QVector3D cameraPosition;
    QVector3D cameraViewCenter;
    float cameraZoom;
    QPoint lastMousePosition;

    void setupScene();
    void addSolid(SolidPtr solid);
    void createNormalVisualization(const QVector3D& start, const QVector3D& normal);
    void setRenderMode(int mode);  // 0: Wireframe, 1: Faces, 2: Both

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

} // namespace theocad

#endif