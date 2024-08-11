#include "cad_visualizer.hpp"
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DRender/QMesh>
#include <Qt3DCore/QTransform>
#include <QVBoxLayout>
#include <QWidget>
#include <QPointLight>
#include <QWheelEvent>
#include <QRenderState>
#include <QCullFace>
#include <iostream>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QNoDepthMask>
#include <boost/rational.hpp>
#include <Qt3DExtras/QCuboidMesh>

namespace theocad {
    
CADVisualizer::CADVisualizer(SolidPtr solid)
    : QMainWindow()
{
    view = new Qt3DExtras::Qt3DWindow();
    QWidget *container = QWidget::createWindowContainer(view);
    setCentralWidget(container);

    rootEntity = new Qt3DCore::QEntity();
    view->setRootEntity(rootEntity);

    setupScene();
    addSolid(solid);
}

#if 0
void CADVisualizer::setupScene()
{
    // Camera setup
    camera = view->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 20.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // Camera controller
    cameraController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    cameraController->setLinearSpeed(50.0f);
    cameraController->setLookSpeed(180.0f);
    cameraController->setCamera(camera);

    // Light
    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(1);
    lightEntity->addComponent(light);
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(QVector3D(10, 10, 10));
    lightEntity->addComponent(lightTransform);
}
#endif

#if 1
void CADVisualizer::setupScene()
{
    // Camera setup
    camera = view->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 20.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // Camera controller
    cameraController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    cameraController->setLinearSpeed(50.0f);
    cameraController->setLookSpeed(180.0f);
    cameraController->setCamera(camera);

    // Create a light entity
    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(0.8f);
    lightEntity->addComponent(light);

    // Create a transform for the light
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightEntity->addComponent(lightTransform);

    // // Connect the camera's viewVectorChanged signal to update the light position
    // QObject::connect(camera, &Qt3DRender::QCamera::viewVectorChanged, [this, lightTransform]() {
    //     QVector3D cameraPosition = camera->position();
    //     lightTransform->setTranslation(cameraPosition);
    // });

    QVector3D lightOffset(20.0f, 20.0f, 0.0f);

    //Connect the camera's viewVectorChanged signal to update the light position
    QObject::connect(camera, &Qt3DRender::QCamera::viewVectorChanged, [this, lightTransform, lightOffset]() {
        QVector3D cameraPosition = camera->position();
        //std::cout << "Pos: " << cameraPosition.x() << ',' << cameraPosition.y() << ',' << cameraPosition.z() << std::endl;
        QVector3D cameraView = camera->viewVector();
        //std::cout << "View: " << cameraView.x() << ',' << cameraView.y() << ',' << cameraView.z() << std::endl;
        QVector3D cameraUpVector = camera->upVector();
        QVector3D cameraRightVector = QVector3D::crossProduct(camera->viewVector(), cameraUpVector).normalized();

        // Calculate the light position relative to the camera
        QVector3D lightPosition = cameraPosition +
                                  cameraUpVector * lightOffset.y() +
                                  cameraRightVector * lightOffset.x() +
                                  camera->viewVector().normalized() * -lightOffset.z();
        //std::cout << "Light: " << lightPosition.x() << ',' << lightPosition.y() << ',' << lightPosition.z() << std::endl;

        lightTransform->setTranslation(lightPosition);
    });

    QVector3D cameraPosition = camera->position();
    QVector3D cameraUpVector = camera->upVector();
    QVector3D cameraRightVector = QVector3D::crossProduct(camera->viewVector(), cameraUpVector).normalized();

    // Calculate the light position relative to the camera
    QVector3D lightPosition = cameraPosition +
                              cameraUpVector * lightOffset.y() +
                              cameraRightVector * lightOffset.x() +
                              camera->viewVector().normalized() * -lightOffset.z();

    lightTransform->setTranslation(lightPosition);
    

    // ... (rest of the setup code)
}
#endif


#if 0
// No wire frame
void CADVisualizer::addSolid(SolidPtr solid)
{
    for (int i = 0; i < solid->size(); ++i) {
        const Surface& surface = (*solid)[i];
        
        for (int j = 0; j < surface.size(); ++j) {
            const Triangle& triangle = surface[j];
            std::cout << "Adding triangle\n";
            
            // Create entity for this triangle
            Qt3DCore::QEntity *triangleEntity = new Qt3DCore::QEntity(rootEntity);
            
            // Create geometry
            Qt3DCore::QGeometry *geometry = new Qt3DCore::QGeometry(triangleEntity);
            
            // Create vertex buffer
            QByteArray bufferBytes;
            bufferBytes.resize(3 * 3 * sizeof(float)); // 3 vertices * (3 floats per vertex)
            float* positions = reinterpret_cast<float*>(bufferBytes.data());
            
            // Fill vertex buffer
            for (int k = 0; k < 3; ++k) {
                positions[k*3] = boost::rational_cast<float>(triangle[k][0]);
                positions[k*3 + 1] = boost::rational_cast<float>(triangle[k][1]);
                positions[k*3 + 2] = boost::rational_cast<float>(triangle[k][2]);
            }
            
            Qt3DCore::QBuffer *vertexBuffer = new Qt3DCore::QBuffer(geometry);
            vertexBuffer->setData(bufferBytes);
            
            // Create position attribute
            Qt3DCore::QAttribute *positionAttribute = new Qt3DCore::QAttribute(geometry);
            positionAttribute->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
            positionAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
            positionAttribute->setVertexSize(3);
            positionAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
            positionAttribute->setBuffer(vertexBuffer);
            positionAttribute->setByteStride(3 * sizeof(float));
            positionAttribute->setCount(3);
            geometry->addAttribute(positionAttribute);
            
            // Create normal attribute
            Vector4r normalVec = surface.getFace().getNormal();
            QVector3D normal(boost::rational_cast<float>(normalVec[0]),
                             boost::rational_cast<float>(normalVec[1]),
                             boost::rational_cast<float>(normalVec[2]));
            normal.normalize();
            
            QByteArray normalBufferBytes;
            normalBufferBytes.resize(3 * 3 * sizeof(float)); // 3 vertices * (3 floats per normal)
            float* normals = reinterpret_cast<float*>(normalBufferBytes.data());
            for (int k = 0; k < 3; ++k) {
                normals[k*3] = normal.x();
                normals[k*3 + 1] = normal.y();
                normals[k*3 + 2] = normal.z();
            }
            
            Qt3DCore::QBuffer *normalBuffer = new Qt3DCore::QBuffer(geometry);
            normalBuffer->setData(normalBufferBytes);
            
            Qt3DCore::QAttribute *normalAttribute = new Qt3DCore::QAttribute(geometry);
            normalAttribute->setName(Qt3DCore::QAttribute::defaultNormalAttributeName());
            normalAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
            normalAttribute->setVertexSize(3);
            normalAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
            normalAttribute->setBuffer(normalBuffer);
            normalAttribute->setByteStride(3 * sizeof(float));
            normalAttribute->setCount(3);
            geometry->addAttribute(normalAttribute);
            
            // Create mesh
            Qt3DRender::QGeometryRenderer *mesh = new Qt3DRender::QGeometryRenderer(triangleEntity);
            mesh->setGeometry(geometry);
            mesh->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
            
            // Create material
            Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial(triangleEntity);
            material->setAmbient(QColor(100, 100, 100));
            material->setDiffuse(QColor(200, 200, 200));
            
            // Add components to the entity
            triangleEntity->addComponent(mesh);
            triangleEntity->addComponent(material);
        }
    }
}
#endif

void CADVisualizer::addSolid(SolidPtr solid)
{
    // Create a single material to be shared by all triangles
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial(rootEntity);
    material->setAmbient(QColor(120, 120, 120));
    material->setDiffuse(QColor(200, 200, 200));
    material->setSpecular(QColor(255, 255, 255));
    material->setShininess(150.0f);


    // Create a wireframe material
    Qt3DExtras::QPhongMaterial *wireMaterial = new Qt3DExtras::QPhongMaterial(rootEntity);
    wireMaterial->setAmbient(QColor(0, 0, 0));
    wireMaterial->setDiffuse(QColor(0, 0, 0));

    for (int i = 0; i < solid->size(); ++i) {
        const Surface& surface = (*solid)[i];
        
        for (int j = 0; j < surface.size(); ++j) {
            const Triangle& triangle = surface[j];
            
            // Create entity for this triangle
            Qt3DCore::QEntity *triangleEntity = new Qt3DCore::QEntity(rootEntity);
            
            // Create geometry
            Qt3DCore::QGeometry *geometry = new Qt3DCore::QGeometry(triangleEntity);
            
            // Create vertex buffer
            QByteArray bufferBytes;
            bufferBytes.resize(3 * 3 * sizeof(float)); // 3 vertices * (3 floats per vertex)
            float* positions = reinterpret_cast<float*>(bufferBytes.data());
            
            // Fill vertex buffer
            for (int k = 0; k < 3; ++k) {
                positions[k*3] = boost::rational_cast<float>(triangle[k][0]);
                positions[k*3 + 1] = boost::rational_cast<float>(triangle[k][1]);
                positions[k*3 + 2] = boost::rational_cast<float>(triangle[k][2]);
            }
            
            Qt3DCore::QBuffer *vertexBuffer = new Qt3DCore::QBuffer(geometry);
            vertexBuffer->setData(bufferBytes);
            
            // Create position attribute
            Qt3DCore::QAttribute *positionAttribute = new Qt3DCore::QAttribute(geometry);
            positionAttribute->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
            positionAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
            positionAttribute->setVertexSize(3);
            positionAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
            positionAttribute->setBuffer(vertexBuffer);
            positionAttribute->setByteStride(3 * sizeof(float));
            positionAttribute->setCount(3);
            geometry->addAttribute(positionAttribute);
            
            // Create normal attribute
            Vector4r normalVec = triangle.getNormal();
            QVector3D normal(boost::rational_cast<float>(normalVec[0]),
                             boost::rational_cast<float>(normalVec[1]),
                             boost::rational_cast<float>(normalVec[2]));
            normal.normalize();
            // std::cout << "Orig: " << normalVec[0] << ',' << normalVec[1] << ',' << normalVec[2] << std::endl;
            // std::cout << "Normal: " << normal.x() << ',' << normal.y() << ',' << normal.z() << std::endl;
            
            QByteArray normalBufferBytes;
            normalBufferBytes.resize(3 * 3 * sizeof(float)); // 3 vertices * (3 floats per normal)
            float* normals = reinterpret_cast<float*>(normalBufferBytes.data());
            for (int k = 0; k < 3; ++k) {
                normals[k*3] = normal.x();
                normals[k*3 + 1] = normal.y();
                normals[k*3 + 2] = normal.z();
            }
            
            Qt3DCore::QBuffer *normalBuffer = new Qt3DCore::QBuffer(geometry);
            normalBuffer->setData(normalBufferBytes);
            
            Qt3DCore::QAttribute *normalAttribute = new Qt3DCore::QAttribute(geometry);
            normalAttribute->setName(Qt3DCore::QAttribute::defaultNormalAttributeName());
            normalAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
            normalAttribute->setVertexSize(3);
            normalAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
            normalAttribute->setBuffer(normalBuffer);
            normalAttribute->setByteStride(3 * sizeof(float));
            normalAttribute->setCount(3);
            geometry->addAttribute(normalAttribute);

            // Create solid mesh
            Qt3DRender::QGeometryRenderer *mesh = new Qt3DRender::QGeometryRenderer(triangleEntity);
            mesh->setGeometry(geometry);
            mesh->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
            
            // Create wireframe mesh
            Qt3DRender::QGeometryRenderer *wireMesh = new Qt3DRender::QGeometryRenderer(triangleEntity);
            wireMesh->setGeometry(geometry);
            wireMesh->setPrimitiveType(Qt3DRender::QGeometryRenderer::LineLoop);

            // Add components to the entity
            triangleEntity->addComponent(mesh);
            triangleEntity->addComponent(material);

            // // Create a separate entity for the wireframe
            Qt3DCore::QEntity *wireframeEntity = new Qt3DCore::QEntity(triangleEntity);
            wireframeEntity->addComponent(wireMesh);
            wireframeEntity->addComponent(wireMaterial);

            // Calculate the center of the triangle
            QVector3D center(0, 0, 0);
            for (int k = 0; k < 3; ++k) {
                center += QVector3D(boost::rational_cast<float>(triangle[k][0]),
                                    boost::rational_cast<float>(triangle[k][1]),
                                    boost::rational_cast<float>(triangle[k][2]));
            }
            center /= 3;

            createNormalVisualization(center, normal);
        }
    }
}

void CADVisualizer::createNormalVisualization(const QVector3D& start, const QVector3D& normal)
{
    QVector3D end = start + normal * 0.1f; // Scale factor for visibility

    Qt3DCore::QEntity *lineEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QGeometryRenderer *lineRenderer = new Qt3DRender::QGeometryRenderer(lineEntity);
    Qt3DCore::QGeometry *lineGeometry = new Qt3DCore::QGeometry(lineRenderer);

    // Position buffer
    QByteArray positionBufferBytes;
    positionBufferBytes.resize(2 * 3 * sizeof(float));
    float* positions = reinterpret_cast<float*>(positionBufferBytes.data());
    positions[0] = start.x(); positions[1] = start.y(); positions[2] = start.z();
    positions[3] = end.x(); positions[4] = end.y(); positions[5] = end.z();

    Qt3DCore::QBuffer *positionBuffer = new Qt3DCore::QBuffer(lineGeometry);
    positionBuffer->setData(positionBufferBytes);

    // Normal buffer (use the line direction as the normal for both vertices)
    QByteArray normalBufferBytes;
    normalBufferBytes.resize(2 * 3 * sizeof(float));
    float* normals = reinterpret_cast<float*>(normalBufferBytes.data());
    normals[0] = normal.x(); normals[1] = normal.y(); normals[2] = normal.z();
    normals[3] = normal.x(); normals[4] = normal.y(); normals[5] = normal.z();

    Qt3DCore::QBuffer *normalBuffer = new Qt3DCore::QBuffer(lineGeometry);
    normalBuffer->setData(normalBufferBytes);

    // Position attribute
    Qt3DCore::QAttribute *positionAttribute = new Qt3DCore::QAttribute(lineGeometry);
    positionAttribute->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(positionBuffer);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(2);
    lineGeometry->addAttribute(positionAttribute);

    // Normal attribute
    Qt3DCore::QAttribute *normalAttribute = new Qt3DCore::QAttribute(lineGeometry);
    normalAttribute->setName(Qt3DCore::QAttribute::defaultNormalAttributeName());
    normalAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
    normalAttribute->setVertexSize(3);
    normalAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
    normalAttribute->setBuffer(normalBuffer);
    normalAttribute->setByteStride(3 * sizeof(float));
    normalAttribute->setCount(2);
    lineGeometry->addAttribute(normalAttribute);

    lineRenderer->setGeometry(lineGeometry);
    lineRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);

    Qt3DExtras::QPhongMaterial *lineMaterial = new Qt3DExtras::QPhongMaterial(lineEntity);
    lineMaterial->setAmbient(QColor(255, 0, 0));  // Red for visibility

    lineEntity->addComponent(lineRenderer);
    lineEntity->addComponent(lineMaterial);
}

void CADVisualizer::setRenderMode(int mode) {
    // Implement rendering mode changes here
}

void CADVisualizer::wheelEvent(QWheelEvent *event) {
    std::cout << "wheelEvent\n";
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        float zoomFactor = 1.0f + numSteps.y() * 0.1f;
        cameraZoom *= zoomFactor;

        QVector3D zoomVector = cameraPosition - cameraViewCenter;
        zoomVector *= zoomFactor;
        cameraPosition = cameraViewCenter + zoomVector;

        std::cout << "Camera position, zoom=" << zoomFactor << std::endl;
        //std::cout << cameraPosition << std::endl;
        camera->setPosition(cameraPosition);
    }

    event->accept();
}

void CADVisualizer::mousePressEvent(QMouseEvent *event) {
    std::cout << "Press\n";
    lastMousePosition = event->pos();
    event->accept();
}

void CADVisualizer::mouseMoveEvent(QMouseEvent *event) {
    std::cout << "Move\n";
    QPoint delta = event->pos() - lastMousePosition;
    lastMousePosition = event->pos();

    if (event->buttons() & Qt::LeftButton) {
        if (event->modifiers() & Qt::ShiftModifier) {
            std::cout << "Shift\n";
            // Pan
            float distance = (cameraPosition - cameraViewCenter).length();
            QVector3D right = QVector3D::crossProduct(camera->upVector(), camera->viewVector()).normalized();
            QVector3D up = camera->upVector();

            cameraPosition -= right * delta.x() * distance * 0.001f;
            cameraPosition -= up * delta.y() * distance * 0.001f;
            cameraViewCenter -= right * delta.x() * distance * 0.001f;
            cameraViewCenter -= up * delta.y() * distance * 0.001f;

            camera->setPosition(cameraPosition);
            camera->setViewCenter(cameraViewCenter);
        }
        else if (event->modifiers() & Qt::AltModifier) {
            std::cout << "Alt\n";
            // Rotate
            float angleX = delta.x() * 0.3f;
            float angleY = delta.y() * 0.3f;

            QVector3D viewVector = cameraPosition - cameraViewCenter;
            QMatrix4x4 rotationMatrix;
            rotationMatrix.rotate(angleX, camera->upVector());
            rotationMatrix.rotate(angleY, QVector3D::crossProduct(camera->upVector(), viewVector).normalized());

            cameraPosition = cameraViewCenter + rotationMatrix.map(viewVector);
            camera->setPosition(cameraPosition);
        }
    }

    event->accept();
}

} // namespace theocad
