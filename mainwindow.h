#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

namespace Ui {
class MainWindow;
}

class DeviceModel;
class GameWriterFactory;
class GameWriterInterface;
class VideoCardDatabase;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(DeviceModel* model, VideoCardDatabase* videoCardDatabase, GameWriterFactory* gamePlugins);
    ~MainWindow();

private slots:
    void selectCard(int row = 0);
    void selectGame(int row = 0);
    void tabOpen(int tabIndex = 0);
    void locateGameFiles(const QString &directory);
    void setStatus(const QString &text, bool allok);

private:
    Ui::MainWindow *ui;
    DeviceModel *m_model;
    VideoCardDatabase *m_videoCardDatabase;
    GameWriterFactory *m_gamePlugins;
    GameWriterInterface *m_currentPlugin;
    QWidget *m_currentGameSettingsWidget;
};

#endif // MAINWINDOW_H
