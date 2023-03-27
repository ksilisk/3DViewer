#include "viewerwindow.h"

#include <QGuiApplication>
#include <QMainWindow>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QScreen>
#include <QtMath>

#define GL_SILENCE_DEPRECATION

ViewerWindow::ViewerWindow(QWindow *parent) : OpenGLWindow(parent) {}

void ViewerWindow::initialize() { load_default_square(); }

void ViewerWindow::load_model(QString path) {
  set_view();
  set_projection();
  char *obj_path = (char *)path.toLatin1().data();
  printf("Loading obj file: %s\n", obj_path);
  Obj *obj = parse_obj(obj_path);
  printf("Obj file loaded: %s, %p\n", obj_path, obj);
  if (obj == NULL) {
    printf("Error: failed to load obj file: %s\n", obj_path);
    load_default_square();
  } else {
    int point_count = obj->vertices->count;
    int err = 0;
    Triangles triangles = triangulate(obj, &err);
    if (!err) {
      printf("Creating vertex buffer from obj file: %s\n", obj_path);
      if (vertex_buffer.data != NULL) safe_free(vertex_buffer.data);
      vertex_buffer = create_vertex_buffer(obj, triangles, &err);
    }
    if (!err) printf("Vertex buffer created from obj file: %s\n", obj_path);
    safe_free(triangles.triangles);
    if (obj) {
      destroy_obj(obj);
      printf("Obj file destroyed: %s, %p\n", obj_path, obj);
    }
    if (err) {
      printf("Error: failed to create vertex buffer from obj file: %s\n",
             obj_path);
      load_default_square();
    } else {
      this->setTitle(QString("Vertices count: %1").arg(point_count));
    }
  }
}

void ViewerWindow::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::RightButton) {
    lastMousePos = event->pos();  // ition().toPoint()
    dragging = true;
  }
}
void ViewerWindow::mouseMoveEvent(QMouseEvent *event) {
  if ((event->buttons() & Qt::RightButton) && dragging) {
    QPoint pos = event->pos();  // ition().toPoint()
    camYaw += (pos.x() - lastMousePos.x()) * rotationAlpha * delta();
    camPitch += (pos.y() - lastMousePos.y()) * rotationAlpha * delta();
    if (camPitch > 90.f) camPitch = 90.f;
    if (camPitch < -90.f) camPitch = -90.f;
    camYaw = fmod(camYaw, 360.f);
    lastMousePos = pos;
  }
}
void ViewerWindow::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::RightButton) {
    dragging = false;
  }
}

void ViewerWindow::keyPressEvent(QKeyEvent *event) {
  int key = event->key();
  switch (key) {
    case Qt::Key_W:
      controls.W = true;
      break;
    case Qt::Key_A:
      controls.A = true;
      break;
    case Qt::Key_S:
      controls.S = true;
      break;
    case Qt::Key_D:
      controls.D = true;
      break;
    case Qt::Key_Space:
      controls.Up = true;
      break;
    case Qt::Key_C:
      controls.Down = true;
      break;
    case Qt::Key_R:
      showcaseRotate = !showcaseRotate;
      break;
    default:
      event->ignore();
  }
}

void ViewerWindow::keyReleaseEvent(QKeyEvent *event) {
  int key = event->key();
  switch (key) {
    case Qt::Key_W:
      controls.W = false;
      break;
    case Qt::Key_A:
      controls.A = false;
      break;
    case Qt::Key_S:
      controls.S = false;
      break;
    case Qt::Key_D:
      controls.D = false;
      break;
    case Qt::Key_Space:
      controls.Up = false;
      break;
    case Qt::Key_C:
      controls.Down = false;
      break;
    default:
      event->ignore();
  }
}

void ViewerWindow::render() {
  const qreal retinaScale = devicePixelRatio();
  glViewport(0, 0, width() * retinaScale, height() * retinaScale);
  glClearColor(backgroundColor.redF(), backgroundColor.greenF(),
               backgroundColor.blueF(), backgroundColor.alphaF());
  glClear(GL_COLOR_BUFFER_BIT);

  update_view();
  update_model();

  int polygonCount = vertex_buffer.count + 1;

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(VertexData), vertex_buffer.data);

  glPointSize(pointSize);

  glColor4f(lineColor.redF(), lineColor.greenF(), lineColor.blueF(),
            lineColor.alphaF());
  glLineWidth(lineWidth);
  if (lineType == LineType::Solid) {
    glDisable(GL_LINE_STIPPLE);
  } else {
    glEnable(GL_LINE_STIPPLE);
    if (lineType == LineType::Dashed)
      glLineStipple(1, 0x00FF);
    else
      glLineStipple(1, 0x0101);
  }
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawArrays(GL_TRIANGLES, 0, polygonCount);

  glColor4f(pointColor.redF(), pointColor.greenF(), pointColor.blueF(),
            pointColor.alphaF());
  glPointSize(pointSize);

  if (pointType != PointType::None) {
    if (pointType == PointType::Circle) {
      glEnable(GL_POINT_SMOOTH);
      glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    } else {
      glDisable(GL_POINT_SMOOTH);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    glDrawArrays(GL_POINTS, 0, polygonCount);
  }

  glDisableClientState(GL_VERTEX_ARRAY);

  m_frame++;
}

void ViewerWindow::set_model() {
  m_model.setToIdentity();

  if (showcaseRotate) angle += angleSpeed * delta();
  angle = fmod(angle, 360.f);

  m_model.translate(position);
  m_model.rotate(rotation.y() + angle, 0.f, 1.f, 0.f);
  m_model.rotate(rotation.x(), 1.f, 0.f, 0.f);
  m_model.rotate(rotation.z(), 0.f, 0.f, 1.f);
  m_model.scale(scale);
}

void ViewerWindow::set_view() {
  m_view.setToIdentity();

  float dx = 0.f, dy = 0.f, dz = 0.f;

  if (controls.W) dz += 1;
  if (controls.A) dx += 1;
  if (controls.S) dz -= 1;
  if (controls.D) dx -= 1;
  if (controls.Up) dy -= 1;
  if (controls.Down) dy += 1;

  QMatrix4x4 rot;
  rot.setToIdentity();
  rot.rotate(camYaw, 0.f, 1.f, 0.f);

  camPosition += QVector3D(dx, dy, dz) * rot * (positionSpeed * delta());

  m_view.rotate(camPitch, 1.f, 0.f, 0.f);
  m_view.rotate(camYaw, 0.f, 1.f, 0.f);

  m_view.translate(camPosition);
}

void ViewerWindow::set_projection() {
  m_projection.setToIdentity();
  if (projectionType == ProjectionType::Orthographic)
    m_projection.ortho(-parallelWidth / 2.f, parallelWidth / 2.f,
                       -parallelHeight / 2.f, parallelHeight / 2.f, 0.1f,
                       100.0f);
  else
    m_projection.perspective(90.0f, (GLfloat)width() / (GLfloat)height(), 0.1f,
                             100.0f);
}

void ViewerWindow::update_model() {
  set_model();
  update_MVP();
}

void ViewerWindow::update_view() {
  set_view();
  update_MVP();
}

void ViewerWindow::update_projection() {
  set_projection();
  update_MVP();
}

void ViewerWindow::update_MVP() {
  m_MVP = m_projection * m_view * m_model;
  glLoadMatrixf(m_MVP.data());
}

void ViewerWindow::load_default_square() {
  // set default vertex buffer triangle
  printf("load_default_square\n");
  vertex_buffer.count = 6;
  safe_free(vertex_buffer.data);
  printf("load_default_square 2\n");
  vertex_buffer.data =
      (VertexData *)calloc(sizeof(VertexData), vertex_buffer.count);
  printf("load_default_square 3\n");
  vertex_buffer.data[0] = {.position = {0.5f, 0.5f, 0.5f, 1.0f},
                           .texture = {0.0f, 0.0f, 0.0f},
                           .normal = {0.0f, 0.0f, 0.0f}};
  vertex_buffer.data[1] = {.position = {0.5f, -0.5f, 0.5f, 1.0f},
                           .texture = {0.0f, 0.0f, 0.0f},
                           .normal = {0.0f, 0.0f, 0.0f}};
  vertex_buffer.data[2] = {.position = {-0.5f, -0.5f, 0.5f, 1.0f},
                           .texture = {0.0f, 0.0f, 0.0f},
                           .normal = {0.0f, 0.0f, 0.0f}};
  vertex_buffer.data[3] = {.position = {0.5f, 0.5f, 0.5f, 1.0f},
                           .texture = {0.0f, 0.0f, 0.0f},
                           .normal = {0.0f, 0.0f, 0.0f}};
  vertex_buffer.data[4] = {.position = {-0.5f, -0.5f, 0.5f, 1.0f},
                           .texture = {0.0f, 0.0f, 0.0f},
                           .normal = {0.0f, 0.0f, 0.0f}};
  vertex_buffer.data[5] = {.position = {-0.5f, 0.5f, 0.5f, 1.0f},
                           .texture = {0.0f, 0.0f, 0.0f},
                           .normal = {0.0f, 0.0f, 0.0f}};
  printf("load_default_square end\n");
}
