#include "squircle.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[]) {
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app(argc, argv);

  qmlRegisterType<Squircle>("Squircle", 1, 0, "Squircle");

  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  if (engine.rootObjects().isEmpty())
    return -1;

  return app.exec();
}
