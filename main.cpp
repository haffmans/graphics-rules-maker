#include <QApplication>

#include "devicemodel.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DeviceModel *model = new DeviceModel();
    MainWindow window(model);

    model->load();
    window.show();

    return app.exec();
}
