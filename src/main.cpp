#include <iostream>
#include <QApplication>
#include "widgets/viewport.h"
#include "widgets/mainwindow.h"
#include <omp.h>

int main(int argc, char **argv) {
    QSurfaceFormat glFormat;
    glFormat.setVersion(3,3);
    glFormat.setProfile(QSurfaceFormat::CoreProfile);
    glFormat.setSamples(4);
    QSurfaceFormat::setDefaultFormat(glFormat);

    QApplication app(argc, argv);
    MainWindow win;
    win.show();
    return app.exec();
}
