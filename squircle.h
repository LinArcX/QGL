#ifndef SQUIRCLE_H
#define SQUIRCLE_H

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtQuick/QQuickItem>

class SquircleRenderer : public QObject, protected QOpenGLFunctions {
  Q_OBJECT
public:
  SquircleRenderer() : m_program(nullptr) {}
  ~SquircleRenderer();

  // void setT(qreal t) { m_t = t; }
  void setViewportSize(const QSize &size) { m_viewportSize = size; }
  void setWindow(QQuickWindow *window) { m_window = window; }

public slots:
  void paint();

private:
  QSize m_viewportSize;
  // qreal m_t;
  QOpenGLShaderProgram *m_program;
  QQuickWindow *m_window;
};

class Squircle : public QQuickItem {
  Q_OBJECT
  // Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

public:
  Squircle();

  // qreal t() const { return m_t; }
  // void setT(qreal t);

private slots:
  void handleWindowChanged(QQuickWindow *win);

signals:
  // void tChanged();

public slots:
  void sync();
  void cleanup();

private:
  // qreal m_t;
  SquircleRenderer *m_renderer;
};
#endif // SQUIRCLE_H
