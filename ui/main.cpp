#include <QApplication>

#include "main/mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  QSurfaceFormat format;
  format.setSwapInterval(1);
  format.setSamples(16);

  MainWindow w;
  w.show();

  return a.exec();
}
