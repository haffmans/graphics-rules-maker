#include <QApplication>

#include "devicemodel.h"
#include "videocarddatabase.h"
#include "gamewriterfactory.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("GraphicRulesMaker");
    app.setApplicationDisplayName("GraphicRules Maker");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("SimsNetwork");
    app.setOrganizationDomain("simsnetwork.com");

    DeviceModel *model = new DeviceModel();
    VideoCardDatabase *database = new VideoCardDatabase();
    GameWriterFactory *pluginFactory = new GameWriterFactory();

    pluginFactory->loadPlugins();

    MainWindow window(model, database, pluginFactory);

    model->load();
    window.show();

    int result = app.exec();

    delete model;
    delete database;
    delete pluginFactory;

    return result;
}
