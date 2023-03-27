#include "openglwindow.h"

#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QPainter>

using namespace std::chrono;
//! [1]
OpenGLWindow::OpenGLWindow(QWindow *parent) : QWindow(parent) {
  setSurfaceType(QWindow::OpenGLSurface);
  connect(&m_timer, SIGNAL(timeout()), this, SLOT(renderNow()));

  if (format().swapInterval() == -1) {
    m_timer.setInterval(17);  // no vsync
  } else {
    m_timer.setInterval(0);
  }
  m_timer.start();
}
//! [1]

OpenGLWindow::~OpenGLWindow() { delete m_device; }

//! [2]
void OpenGLWindow::render(QPainter *painter) { Q_UNUSED(painter); }

void OpenGLWindow::initialize() {}

void OpenGLWindow::render() {
  if (!m_device) m_device = new QOpenGLPaintDevice;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  m_device->setSize(size() * devicePixelRatio());
  m_device->setDevicePixelRatio(devicePixelRatio());

  QPainter painter(m_device);
  render(&painter);
}
//! [2]

//! [3]
void OpenGLWindow::renderLater() { requestUpdate(); }

bool OpenGLWindow::event(QEvent *event) {
  switch (event->type()) {
    case QEvent::UpdateRequest:
      renderNow();
      return true;
    default:
      return QWindow::event(event);
  }
}

void OpenGLWindow::exposeEvent(QExposeEvent *event) {
  Q_UNUSED(event);

  if (isExposed()) renderNow();
}
//! [3]

//! [4]
void OpenGLWindow::renderNow() {
  if (!isExposed()) return;

  bool needsInitialize = false;

  if (!m_context) {
    m_context = new QOpenGLContext(this);
    m_context->setFormat(requestedFormat());
    m_context->create();

    needsInitialize = true;
  }

  m_context->makeCurrent(this);

  if (needsInitialize) {
    initializeOpenGLFunctions();
    initialize();
  }

  uint64_t millis =
      duration_cast<milliseconds>(system_clock::now().time_since_epoch())
          .count();
  m_delta = millis - m_lastFrame;
  m_lastFrame = millis;

  render();

  m_context->swapBuffers(this);

  if (m_animating) renderLater();
}
//! [4]

//! [5]
void OpenGLWindow::setAnimating(bool animating) {
  m_animating = animating;

  if (animating) renderLater();
}
//! [5]

uint64_t OpenGLWindow::lastFrame() { return m_lastFrame; }

uint64_t OpenGLWindow::delta() { return m_delta; }
