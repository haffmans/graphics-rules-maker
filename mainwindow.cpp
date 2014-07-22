#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets/QListView>
#include <QtCore/QBuffer>
#include <QtCore/QSettings>
#include <QtWidgets/QFileDialog>

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
    m_currentPlugin = nullptr;
    m_currentGameSettingsWidget = nullptr;

    ui->setupUi(this);

    connect(ui->deviceSelect, SIGNAL(currentIndexChanged(int)), SLOT(selectCard(int)));
    connect(ui->mainTabs, SIGNAL(currentChanged(int)), SLOT(tabOpen(int)));
    connect(ui->gameSelect, SIGNAL(currentIndexChanged(int)), SLOT(selectGame(int)));
    connect(ui->gamePath, SIGNAL(textChanged(QString)), SLOT(locateGameFiles(QString)));
    connect(ui->browseFilesButton, SIGNAL(clicked(bool)), SLOT(browseGame()));

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

void MainWindow::browseGame()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Find the installation path of %1").arg(m_currentPlugin->displayName()),
        ui->gamePath->text()
    );

    if (!dir.isEmpty()) {
        ui->gamePath->setText(dir);
    }
}

void MainWindow::selectGame(int row)
{
    // Save path of current selection first
    QSettings s;
    if (m_currentPlugin) {
        s.setValue(m_currentPlugin->id() + "/path", ui->gamePath->text());
    }

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
    if (s.contains(m_currentPlugin->id() + "/path")) {
        ui->gamePath->setText(s.value(m_currentPlugin->id() + "/path").toString());
    }
    else {
        // We don't use this as s.value()'s default, to avoid searching the
        // game every time.
        ui->gamePath->setText(m_currentPlugin->findGameDirectory().absolutePath());
    }
}

void MainWindow::locateGameFiles(const QString& directory)
{
    if (!m_currentPlugin) {
        setStatus(tr("No game selected"), false);
        return;
    }

    QDir gameDirectory(directory);
    if (!gameDirectory.exists()) {
        setStatus(tr("Directory does not exist."), false);
        return;
    }

    QFileInfo gameExeFile = m_currentPlugin->gameExecutable(gameDirectory);
    if (!gameExeFile.exists()) {
        setStatus(tr("Cannot find game application."), false);
        return;
    }
    if (!gameExeFile.isFile() || !gameExeFile.isReadable()) {
        setStatus(tr("Game application file is not an executable file."), false);
        return;
    }

    QFileInfo graphicsRulesFile = m_currentPlugin->rulesFileName(gameDirectory);
    if (!graphicsRulesFile.exists()) {
        setStatus(tr("Cannot find Graphics Rules files."), false);
        return;
    }
    if (!graphicsRulesFile.isFile() || !graphicsRulesFile.isReadable()) {
        setStatus(tr("Graphics Rules file is not a readable file."), false);
        return;
    }

    QFileInfo videoCardsFile = m_currentPlugin->databaseFileName(gameDirectory);
    if (!videoCardsFile.exists()) {
        setStatus(tr("Cannot find Video Cards database."), false);
        return;
    }
    if (!videoCardsFile.isFile() || !videoCardsFile.isReadable()) {
        setStatus(tr("Video Cards database file is not a readable file."), false);
        return;
    }

    m_videoCardDatabase->loadFrom(videoCardsFile.absoluteFilePath());
    setStatus(tr("Game found, video cards database loaded."), true);
}

void MainWindow::setStatus(const QString& text, bool allok)
{
    QString color = (allok) ? "green" : "red";
    ui->status->setText("<font style=\"color: " + color + "\">" + text + "</font>");
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
