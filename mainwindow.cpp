#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets/QListView>
#include <QtCore/QBuffer>

#include "devicemodel.h"
#include "videocarddatabase.h"

MainWindow::MainWindow(DeviceModel* model, VideoCardDatabase* videoCardDatabase) :
    QMainWindow(),
    ui(new Ui::MainWindow)
{
    m_model = model;
    m_videoCardDatabase = videoCardDatabase;

    ui->setupUi(this);
    ui->deviceSelect->setModel(m_model);

    ui->videoCardsView->setModel(videoCardDatabase);

    connect(ui->deviceSelect, SIGNAL(currentIndexChanged(int)), SLOT(selectCard(int)));

    connect(ui->mainTabs, SIGNAL(currentChanged(int)), SLOT(tabOpen(int)));
}

void MainWindow::selectCard(int row)
{
    if (row >= 0) {
        GraphicsDevice dev = m_model->device(row);
        ui->display->setText("TODO");
        ui->deviceVendor->setText("0x" + QString::number(dev.vendorId, 16));
        ui->deviceId->setText("0x" + QString::number(dev.deviceId, 16));
        ui->driver->setText(dev.driver);
        ui->memory->setText(tr("%1 Mb").arg(dev.memory / (1024*1024)));
    }
}

void MainWindow::tabOpen(int tabIndex)
{
    Q_UNUSED(tabIndex);

    if (ui->mainTabs->currentWidget() == ui->videoCardsTab) {
        QBuffer buffer;
        m_videoCardDatabase->write(&buffer);
        QString plainText(buffer.data());

        ui->videoCardsText->setPlainText(plainText);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
