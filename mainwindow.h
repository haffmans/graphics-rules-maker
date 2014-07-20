#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class DeviceModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(DeviceModel *model);
    ~MainWindow();

private slots:
    void selectCard(int row = 0);

private:
    Ui::MainWindow *ui;
    DeviceModel *m_model;
};

#endif // MAINWINDOW_H
