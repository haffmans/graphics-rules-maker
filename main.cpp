#include <QApplication>

#include "devicemodel.h"
#include "videocarddatabase.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DeviceModel *model = new DeviceModel();
    VideoCardDatabase *database = new VideoCardDatabase();
    database->loadFrom("/mnt/apps/Electronic Arts/The Sims 2 Ultimate Collection/Fun with Pets/SP9/TSData/Res/Config/Video Cards.sgr");

    MainWindow window(model, database);

    model->load();
    window.show();

    return app.exec();
}
