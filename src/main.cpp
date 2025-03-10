#include <QApplication>
#include "Modules/Modules.h"

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  MainWindow gui(12, QSizeF(60, 60));
  ThreadPools threads;
  Modules modules(&gui, &threads);
  gui.setup(ROBOCIN_PROJECT_NAME, ROBOCIN_PROJECT_VERSION, ROBOCIN_PROJECT_PATH);
  // TODO: When there is no GUI, CTRL+C will not work and we have to kill the process manually.
  //       This is a bug of Qt. We should find a way to fix it.
  if (!QApplication::arguments().contains("-no-gui"))
    gui.show();
  InterruptRequest<SIGINT>::setup([&gui]() {
    QMetaObject::invokeMethod(&gui, &MainWindow::close, Qt::QueuedConnection);
  });
  if (QApplication::arguments().contains("-running")) {
    QMetaObject::invokeMethod(&gui, [&gui]() {
      gui.playPauseButton()->setRunning(true);
    });
  }
  return QApplication::exec();
}
