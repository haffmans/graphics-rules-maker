#include <QApplication>

#include <QMessageBox>

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

    model->load();
    pluginFactory->loadPlugins();

    if (pluginFactory->plugins().size() == 0) {
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("No game plugins found. Please re-install the application."));
        return -1;
    }

    MainWindow window(model, database, pluginFactory);

    window.show();

    int result = app.exec();

    delete model;
    delete database;
    delete pluginFactory;

    return result;
}
