#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

class VideoCardDatabase;
namespace Ui {
class MainWindow;
}

class DeviceModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(DeviceModel *model, VideoCardDatabase *videoCardDatabase);
    ~MainWindow();

private slots:
    void selectCard(int row = 0);
    void tabOpen(int tabIndex = 0);

private:
    Ui::MainWindow *ui;
    DeviceModel *m_model;
    VideoCardDatabase *m_videoCardDatabase;
};

#endif // MAINWINDOW_H
