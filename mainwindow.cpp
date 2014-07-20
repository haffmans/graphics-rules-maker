#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets/QListView>

#include "devicemodel.h"

MainWindow::MainWindow(DeviceModel *model) :
    QMainWindow(),
    ui(new Ui::MainWindow)
{
    m_model = model;
    ui->setupUi(this);
    ui->deviceSelect->setModel(m_model);

    connect(ui->deviceSelect, SIGNAL(currentIndexChanged(int)), SLOT(selectCard(int)));
}

void MainWindow::selectCard(int row)
{
    if (row >= 0) {
        GraphicsDevice dev = m_model->device(row);
        ui->memory->setText(tr("%1 Mb").arg(dev.memory / (1024*1024)));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
