#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include <QMatrix4x4>

#include "openglwindow.h"

extern "C" {
#include "../../parser/s21_parser.h"
}

enum ProjectionType {
    Orthographic,
    Perspective
};

enum LineType {
    Solid,
    Dashed,
    Dotted
};

enum PointType {
    Square,
    Circle,
    None
};

enum DisplayMethod {
    DisplayTriangles,
    DisplayQuads,
    DisplayPoints,
    DisplayLines
};

struct Controls {
    bool W,A,S,D,Up,Down;
};

class ViewerWindow : public OpenGLWindow {
public:
    explicit ViewerWindow(QWindow *parent = nullptr);
    using OpenGLWindow::OpenGLWindow;

    void initialize() override;

    void render() override;

    void update_model();

    void update_view();

    void update_projection();

    void load_model(QString path);

    // model
    QVector3D position = QVector3D(0.0f, 0.0f, 0.0f);
    QVector3D rotation = QVector3D(0.1f, 0.5f, 0.0f);
    QVector3D scale = QVector3D(0.5f, 0.5f, 0.5f);
    float angle = 0.0f;
    float angleSpeed = 12.f / 1000.f;
    bool showcaseRotate = false;

    // customization settings
    QColor backgroundColor = QColor(0, 0, 0, 255);
    QColor lineColor = QColor(255, 255, 255, 255);
    QColor pointColor = QColor(255, 255, 255, 255);

    ProjectionType projectionType = ProjectionType::Orthographic;

    float lineWidth = 1.0f;
    float pointSize = 5.0f;
    LineType lineType = LineType::Solid;
    PointType pointType = PointType::None;

    // camera & controls
    float positionSpeed = 1.f / 1000.f;
    float rotationAlpha = 1.f / 50.f; // mouse rotation coefficient
    float parallelWidth = 5.0f;
    float parallelHeight = 2.5f;

    float camYaw = 0.f;
    float camPitch = 0.f;
    QVector3D camPosition = {0.0f, 0.0f, -4.0f};

    bool dragging = false;
    QPoint lastMousePos;
    Controls controls;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void update_MVP();

    void set_model();

    void set_view();

    void set_projection();

    void load_default_square();

    int m_frame = 0;
    QMatrix4x4 m_model = QMatrix4x4();
    QMatrix4x4 m_view = QMatrix4x4();
    QMatrix4x4 m_projection = QMatrix4x4();
    QMatrix4x4 m_MVP = QMatrix4x4();

    const char *default_obj_path = "/Users/yuehbell/dev/C8_3DViewer_v1.0-0/src/models/Female.obj";
    VertexBuffer vertex_buffer = {};
};

#endif // VIEWERWINDOW_H
