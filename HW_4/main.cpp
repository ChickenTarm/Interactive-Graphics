#include "img_viewer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

      // do your own argument parsing

      ImageViewer *imgViewer = new ImageViewer();
      imgViewer->show();

      return app.exec();
}
