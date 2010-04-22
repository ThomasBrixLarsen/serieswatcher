#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QCoreApplication::setOrganizationDomain("iksaif.net");
  QCoreApplication::setApplicationName("SeriesWatcher");

  MainWindow window;

  window.show();
  return app.exec();
}
