#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets/QListView>
#include <QtCore/QBuffer>
#include <QtCore/QSettings>

#include "devicemodel.h"
#include "videocarddatabase.h"
#include "gamewriterfactory.h"
#include "gamewriterinterface.h"

MainWindow::MainWindow(DeviceModel* model, VideoCardDatabase* videoCardDatabase, GameWriterFactory *gamePlugins) :
    QMainWindow(),
    ui(new Ui::MainWindow)
{
    m_model = model;
    m_videoCardDatabase = videoCardDatabase;
    m_gamePlugins = gamePlugins;
    m_currentGameSettingsWidget = 0;

    ui->setupUi(this);

    connect(ui->deviceSelect, SIGNAL(currentIndexChanged(int)), SLOT(selectCard(int)));
    connect(ui->mainTabs, SIGNAL(currentChanged(int)), SLOT(tabOpen(int)));
    connect(ui->gameSelect, SIGNAL(currentIndexChanged(int)), SLOT(selectGame(int)));

    ui->deviceSelect->setModel(m_model);
    ui->videoCardsView->setModel(m_videoCardDatabase);
    foreach(GameWriterInterface* plugin, m_gamePlugins->plugins()) {
        ui->gameSelect->addItem(plugin->displayName(), plugin->id());
    }

    selectGame(0);
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

void MainWindow::selectGame(int row)
{
    // Load plugin
    QString id = ui->gameSelect->currentData().toString();
    m_currentPlugin = m_gamePlugins->plugin(id);
    if (!m_currentPlugin) {
        return;
    }

    // Load settings widget
    if (m_currentGameSettingsWidget) {
        ui->settingsBox->layout()->removeWidget(m_currentGameSettingsWidget);
        m_currentGameSettingsWidget->deleteLater();
    }

    m_currentGameSettingsWidget = m_currentPlugin->settingsWidget(ui->settingsBox);
    ui->settingsBox->layout()->addWidget(m_currentGameSettingsWidget);

    // Determine game path - use previous setting if possible
    QSettings s;
    if (s.contains(m_currentPlugin->id() + "/path")) {
        ui->gamePath->setText(s.value(m_currentPlugin->id() + "/path").toString());
    }
    else {
        // We don't use this as s.value()'s default, to avoid searching the
        // game every time.
        ui->gamePath->setText(m_currentPlugin->findGameDirectory().absolutePath());
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
