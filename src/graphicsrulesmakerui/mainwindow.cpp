/*
 * Graphics Rules Maker
 * Copyright (C) 2014 Wouter Haffmans <wouter@simply-life.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QListView>
#include <QtCore/QBuffer>
#include <QtCore/QDateTime>
#include <QtCore/QSettings>
#include <QtCore/QDebug>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QDirModel>
#include <QtWidgets/QCompleter>
#include <QtGui/QDesktopServices>

#include "graphicsrulesmaker/devicemodel.h"
#include "graphicsrulesmaker/videocarddatabase.h"
#include "graphicsrulesmaker/gamewriterinterface.h"
#include "graphicsrulesmaker/graphicsrulesmaker_config.h"

#include "gamewriterfactory.h"
#include "manualsaveconfirmationbox.h"
#include "aboutdialog.h"

MainWindow::MainWindow(DeviceModel* model, VideoCardDatabase* videoCardDatabase, GameWriterFactory *gamePlugins) :
    QMainWindow(0, Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint),
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
    connect(ui->saveAll, SIGNAL(clicked(bool)), SLOT(save()));
    connect(ui->graphicsRulesSave, SIGNAL(clicked(bool)), SLOT(saveGraphicRules()));
    connect(ui->videoCardsSave, SIGNAL(clicked(bool)), SLOT(saveVideoCards()));
    connect(ui->cardInDb, SIGNAL(linkActivated(QString)), SLOT(addDeviceLink(QString)));
    connect(videoCardDatabase, SIGNAL(rowsInserted(QModelIndex,int,int)), SLOT(updateDeviceStatus()));
    connect(videoCardDatabase, SIGNAL(rowsRemoved(QModelIndex,int,int)), SLOT(updateDeviceStatus()));
    connect(videoCardDatabase, SIGNAL(modelReset()), SLOT(updateDeviceStatus()));
    connect(ui->aboutAction, SIGNAL(triggered()), SLOT(about()));

    ui->deviceSelect->setModel(m_model);
    if (m_model->rowCount() > 0) {
        ui->deviceStack->setCurrentWidget(ui->deviceInfo);
    }
    else {
        ui->deviceStack->setCurrentWidget(ui->noDevicesWarning);
    }

    ui->videoCardsView->setModel(m_videoCardDatabase);
    foreach(GameWriterInterface* plugin, m_gamePlugins->plugins()) {
        ui->gameSelect->addItem(plugin->displayName(), plugin->id());
    }

    QDirModel *dirModel = new QDirModel(this);
    dirModel->setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable);
    QCompleter *completer = new QCompleter(dirModel, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->gamePath->setCompleter(completer);

    // Restore state
    QSettings s;
    s.beginGroup("ui");
    restoreGeometry(s.value("window/geometry").toByteArray());
    restoreState(s.value("window/state").toByteArray());
    setLocale(s.value("window/locale", QLocale(QLocale::English, QLocale::UnitedStates)).toLocale());;
    ui->videoCardsSplitter->restoreState(s.value("videocards/splitterstate").toByteArray());
    if (s.contains("videocards/treeviewheaderstate")) {
        ui->videoCardsView->header()->restoreState(s.value("videocards/treeviewheaderstate").toByteArray());
    }
    else {
        // Custom defaults not available in designer
        ui->videoCardsView->header()->resizeSection(0, 150);
        ui->videoCardsView->header()->resizeSection(1, 100);
        ui->videoCardsView->header()->resizeSection(2, 250);
        ui->videoCardsView->header()->resizeSection(3, 100);
    }
    int gameRow = 0;
    if (s.contains("game/id")) {
        QString gameId = s.value("game/id").toString();
        // Find index of previously selected game
        for (int i = 0; i < ui->gameSelect->count(); ++i) {
            if (ui->gameSelect->itemData(i).toString() == gameId) {
                gameRow = i;
                break;
            }
        }
    }
    selectGame(gameRow);

    foreach(const QLocale &locale, appLocales()) {
        QString caption = QString("%1 (%2)").arg(
            QLocale::languageToString(locale.language()),
            locale.nativeLanguageName()
        );
        QAction *switchAction = ui->languageMenu->addAction(caption, this, SLOT(switchLocale()));
        switchAction->setData(locale);
    }

    setLocale(m_locale);
    qApp->installTranslator(&m_libraryTranslator);
    qApp->installTranslator(&m_uiTranslator);
    qApp->installTranslator(&m_pluginTranslator);

    // Call this once to set up the initial widget
    replaceWidget();
}

void MainWindow::selectCard(int row)
{
    if (row >= 0) {
        GraphicsDevice dev = m_model->device(row);
        ui->display->setText(dev.display);
        ui->deviceVendor->setText(formatId(dev.vendorId));
        ui->deviceId->setText(formatId(dev.deviceId));
        ui->driver->setText(dev.driver);
        ui->memory->setText(tr("%1 Mb").arg(dev.memory / (1024*1024)));
        updateDeviceStatus();
    }
}

void MainWindow::addDeviceLink(const QString& link)
{
    QRegExp linkCheck("addcard\\?row=(\\d+)", Qt::CaseSensitive);
    if (linkCheck.exactMatch(link)) {
        // Row id is in cap(1) - we can convert because it's a \d+ match.
        int row = linkCheck.cap(1).toInt();
        GraphicsDevice dev = m_model->device(row);
        if (!m_videoCardDatabase->contains(dev.vendorId, dev.deviceId)) {
            m_videoCardDatabase->addDevice(dev.vendorId, dev.deviceId, dev.name);
        }
    }

    // Update UI again
    updateDeviceStatus();
}

void MainWindow::updateDeviceStatus()
{
    int row = ui->deviceSelect->currentIndex();
    GraphicsDevice dev = m_model->device(row);
    if (m_videoCardDatabase->contains(dev.vendorId, dev.deviceId)) {
        ui->cardInDb->setText("<font style=\"color: green\">" + tr("Yes") + "</font>");
    }
    else {
        ui->cardInDb->setText("<font style=\"color: red\">" + tr("No") + "</font> "
            "<a href=\"addcard?row=" + QString::number(row) + "\">" +
            tr("Add now...") + "</a>"
        );
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

    // Load settings widget - this will trigger the replaceWidget() slot a bit later
    if (m_currentGameSettingsWidget) {
        ui->settingsBox->layout()->removeWidget(m_currentGameSettingsWidget);
        m_currentGameSettingsWidget->deleteLater();
    }

    // Determine game path - use previous setting if possible
    if (s.contains(m_currentPlugin->id() + "/path")) {
        ui->gamePath->setText(s.value(m_currentPlugin->id() + "/path").toString());
    }
    else {
        // We don't use this as s.value()'s default, to avoid searching the
        // game every time.
        ui->gamePath->setText(QDir::toNativeSeparators(m_currentPlugin->findGameDirectory().absolutePath()));
    }

    // Translation
    QString pluginFile = m_gamePlugins->translationFilename(m_currentPlugin->id());
    QString pluginDir = m_gamePlugins->translationDirectory(m_currentPlugin->id());
    QStringList dirs = QStringList() << pluginDir << translationDirectories();
    foreach(QString dir, dirs) {
        if (!m_pluginTranslator.load(m_locale, pluginFile, "_", dir)) {
            qDebug() << "Plugin translation not loaded :( - " << pluginFile << " in " << dir << " locale " << QLocale::languageToString(m_locale.language());
        }
    }

    // Set preview tab names
    int index = ui->mainTabs->indexOf(ui->graphicsRulesTab);
    ui->mainTabs->setTabText(index, tr("%1 Preview").arg(m_currentPlugin->rulesFileName()));
    index = ui->mainTabs->indexOf(ui->videoCardsTab);
    ui->mainTabs->setTabText(index, tr("%1 Preview").arg(m_currentPlugin->databaseFileName()));
}

void MainWindow::replaceWidget()
{
    // We do this after the widget is deleted, to make sure settings are kept if we
    // replace it with the widget from the same plugin (e.g. because we've just
    // switched locales). Settings are usually saved on the destruction of the
    // widget, hence it's in the destroyed() slot.
    qDebug() << "SETTING WIDGET";
    m_currentGameSettingsWidget = m_currentPlugin->settingsWidget(m_model, m_videoCardDatabase, ui->settingsBox);
    ui->settingsBox->layout()->addWidget(m_currentGameSettingsWidget);
    connect(m_currentGameSettingsWidget, SIGNAL(destroyed(QObject*)), SLOT(replaceWidget()));
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
    ui->saveAll->setEnabled(allok);
}

void MainWindow::tabOpen(int tabIndex)
{
    Q_UNUSED(tabIndex);

    if (ui->mainTabs->currentWidget() == ui->graphicsRulesTab && m_currentPlugin) {
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly | QIODevice::Text);
        m_currentPlugin->write(m_currentGameSettingsWidget, &buffer);
        QString plainText(buffer.data());
        buffer.close();

        ui->graphicsRulesText->setPlainText(plainText);
    }
    else if (ui->mainTabs->currentWidget() == ui->videoCardsTab) {
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly | QIODevice::Text);
        m_videoCardDatabase->write(&buffer);
        QString plainText(buffer.data());
        buffer.close();

        ui->videoCardsText->setPlainText(plainText);
    }
}

void MainWindow::save()
{
    // First make sure devices are in database
    askAddDevices();

    // TODO: Date/timestamp in suffix?
    QString bakSuffix = ".bak";
    bool manualSave = false;

    QDir gameDirectory(ui->gamePath->text());
    QFileInfo graphicsRulesFile(m_currentPlugin->rulesFileName(gameDirectory));
    QFileInfo graphicsRulesBackup = graphicsRulesFile.absoluteFilePath() + bakSuffix;
    QFileInfo graphicsRulesDir(graphicsRulesFile.absolutePath());
    QFileInfo videoCardsFile = m_currentPlugin->databaseFileName(gameDirectory);
    QFileInfo videoCardsFileBackup = videoCardsFile.absoluteFilePath() + bakSuffix;
    QFileInfo videoCardsDir(videoCardsFile.absolutePath());

    if (!graphicsRulesDir.isWritable() || !videoCardsDir.isWritable()) {
        manualSave = true;
    }

    if (!manualSave) {
        // Create backups if they don't exist yet (we won't overwrite files)
        // Note: QFile::copy() will not overwrite files, but it does return false
        //       if the file exists. Hence we check ourselves.
        bool backupOk = true;
        if (!graphicsRulesBackup.exists()) {
            backupOk = backupOk && QFile::copy(graphicsRulesFile.absoluteFilePath(), graphicsRulesBackup.absoluteFilePath());
        }
        if (!videoCardsFileBackup.exists()) {
            backupOk = backupOk && QFile::copy(videoCardsFile.absoluteFilePath(), videoCardsFileBackup.absoluteFilePath());
        }

        if (!backupOk) {
            QMessageBox::critical(this, tr("Error"), tr("Could not create back-up files. Aborting."));
            return;
        }
    }
    else {
        QMessageBox::StandardButton result = QMessageBox::information(this, tr("Saving files"),
            tr("The files cannot be saved to the game directory (it is not writable).\n\n"
               "They will instead be saved to a temporary directory."
               "You will need to copy them to the game directory manually.\n\n"
               "Alternatively it may help to restart the Graphics Rules Maker as Administrator."
            ), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok
        );
        if (result == QMessageBox::Cancel) {
            return;
        }

        gameDirectory = QDir::temp();
        if (!gameDirectory.mkpath("GraphicsRulesMaker")) {
            QMessageBox::critical(this, tr("Error"), tr("Could not create temporary location for files."));
            return;
        }
        QFileInfo tempDirInfo = QFileInfo(gameDirectory.absoluteFilePath("GraphicsRulesMaker"));
        if (!tempDirInfo.isDir() || !tempDirInfo.isWritable()) {
            QMessageBox::critical(this, tr("Error"), tr("Could not write to temporary location."));
            return;
        }
        gameDirectory = QDir(tempDirInfo.filePath());

        // Make Graphics Rules file backup
        if (graphicsRulesBackup.exists()) {
            // Copy original back-up so all files are together
            QFile::copy(graphicsRulesBackup.absoluteFilePath(), gameDirectory.absoluteFilePath(graphicsRulesBackup.fileName()));
        }
        else {
            // Copy original file as backup
            QFile::copy(graphicsRulesFile.absoluteFilePath(), gameDirectory.absoluteFilePath(graphicsRulesBackup.fileName()));
        }
        // We want to write to the temporary directory
        graphicsRulesFile = QFileInfo(gameDirectory.absoluteFilePath(graphicsRulesFile.fileName()));

        // Make Video Cards file backup
        if (videoCardsFileBackup.exists()) {
            // Copy original back-up so all files are together
            QFile::copy(videoCardsFileBackup.absoluteFilePath(), gameDirectory.absoluteFilePath(videoCardsFileBackup.fileName()));
        }
        else {
            // Copy original file as backup
            QFile::copy(videoCardsFile.absoluteFilePath(), gameDirectory.absoluteFilePath(videoCardsFileBackup.fileName()));
        }
        // We want to write to the temporary directory
        videoCardsFile = QFileInfo(gameDirectory.absoluteFilePath(videoCardsFile.fileName()));
    }

    // Backup made, file info point to correct path now. Write files.
    QFile graphicsRulesOut(graphicsRulesFile.absoluteFilePath());
    if (!graphicsRulesOut.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open Graphic Rules file for writing."));
        return;
    }
    m_currentPlugin->write(m_currentGameSettingsWidget, &graphicsRulesOut);
    graphicsRulesOut.close();

    QFile videoCardsOut(videoCardsFile.absoluteFilePath());
    if (!videoCardsOut.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open Video Cards file for writing."));
        return;
    }
    m_videoCardDatabase->write(&videoCardsOut);
    videoCardsOut.close();

    if (!manualSave) {
        QMessageBox::information(this, tr("Files Saved"),
            tr("The files have been saved. You can now run your game using the new settings.")
        );
    }
    else {
        ManualSaveConfirmationBox *confirmation = new ManualSaveConfirmationBox(this);
        connect(confirmation, SIGNAL(finished(int)), confirmation, SLOT(deleteLater()));
        connect(confirmation, SIGNAL(openDestinationDirectory()), SLOT(openDestinationDirectory()));
        connect(confirmation, SIGNAL(openTemporaryDirectory()), SLOT(openTemporaryDirectory()));
        confirmation->open();
    }
}

void MainWindow::saveGraphicRules()
{
    QDir gameDirectory(ui->gamePath->text());
    QFileInfo gameFile = m_currentPlugin->rulesFileName(gameDirectory);
    QString startDir = gameFile.absoluteFilePath();
    QString defaultName = m_currentPlugin->rulesFileName();
    if (gameFile.exists()) {
        defaultName = gameFile.fileName();
    }
    else { // Not found -> use current path + default filename
        startDir = gameDirectory.absoluteFilePath(defaultName);
    }

    QString destination = QFileDialog::getSaveFileName(this, tr("Save %1").arg(defaultName),
                                 startDir,
                                 tr("SimCity Graphic Rules files (*.sgr);;All files (*.*)")
    );

    if (destination.isEmpty()) {
        // No file was chosen
        return;
    }

    QFile dst(destination);
    if (!dst.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("File '%1' cannot be written to.").arg(destination));
        return;
    }

    m_currentPlugin->write(m_currentGameSettingsWidget, &dst);
    dst.close();

    QMessageBox::information(this, tr("File saved"), tr("Graphics rules saved to '%1'.").arg(destination));
}

void MainWindow::saveVideoCards()
{
    QDir gameDirectory(ui->gamePath->text());
    QFileInfo gameFile = m_currentPlugin->databaseFileName(gameDirectory);
    QString startDir = gameFile.absoluteFilePath();
    QString defaultName = m_currentPlugin->databaseFileName();
    if (gameFile.exists()) {
        defaultName = gameFile.fileName();
    }
    else { // Not found -> use current path + default filename
        startDir = gameDirectory.absoluteFilePath(defaultName);
    }

    QString destination = QFileDialog::getSaveFileName(this, tr("Save %1").arg(defaultName),
                                 gameFile.absoluteFilePath(),
                                 tr("SimCity Graphic Rules files (*.sgr);;All files (*.*)")
    );

    if (destination.isEmpty()) {
        // No file was chosen
        return;
    }

    QFile dst(destination);
    if (!dst.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("File '%1' cannot be written to.").arg(destination));
        return;
    }

    m_currentPlugin->write(m_currentGameSettingsWidget, &dst);
    dst.close();

    QMessageBox::information(this, tr("File saved"), tr("Video cards database saved to '%1'.").arg(destination));
}

void MainWindow::openDestinationDirectory()
{
    // Game dir
    QDir gameDirectory(ui->gamePath->text());
    QFileInfo graphicsRulesFile(m_currentPlugin->rulesFileName(gameDirectory));
    // Use "path()" here for the directory containing the file
    QUrl fileUrl = QUrl::fromLocalFile(graphicsRulesFile.path());
    QDesktopServices::openUrl(fileUrl);
}

void MainWindow::openTemporaryDirectory()
{
    QFileInfo tempDirInfo = QFileInfo(QDir::temp().absoluteFilePath("GraphicsRulesMaker"));
    QUrl tempDirUrl = QUrl::fromLocalFile(tempDirInfo.absoluteFilePath());
    QDesktopServices::openUrl(tempDirUrl);
}

void MainWindow::askAddDevices()
{
    QList<GraphicsDevice> missingDevices;
    QStringList missingDeviceNames;
    for (int i = 0; i < m_model->rowCount(); ++i) {
        GraphicsDevice device = m_model->device(i);
        if (!m_videoCardDatabase->contains(device.vendorId, device.deviceId)) {
            missingDevices.append(device);
            missingDeviceNames.append(device.name);
        }
    }

    if (missingDevices.isEmpty()) {
        // Nothing to do
        return;
    }
    QString question = tr("The following devices are still missing in the video card database. Do you want to add them?")
        + "\n\n"
        + missingDeviceNames.join("\n");

    QMessageBox::StandardButton result = QMessageBox::question(this, tr("Add devices?"), question, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

    if (result == QMessageBox::Yes) {
        foreach(const GraphicsDevice &device, missingDevices) {
            m_videoCardDatabase->addDevice(device.vendorId, device.deviceId, device.name);
        }
    }
}

QString MainWindow::formatId(quint16 id) const
{
    return QString("0x%1").arg(id, 4, 16, QChar('0'));
}

QStringList MainWindow::translationDirectories() const
{
    QStringList result;
    result << QString(GRAPHICSRULESMAKER_TRANSLATIONS_PATH);

#ifdef Q_OS_WIN32
    // Scan in directory of self application
    QDir appDir(qApp->applicationDirPath());
    result.append(appDir.absoluteFilePath("../share"));
    result.append(appDir.absoluteFilePath("../share/graphicsrulesmaker"));
#endif
    return result;
}

QList< QLocale > MainWindow::appLocales() const
{
    QList<QLocale> result;
    result.append(QLocale(QLocale::English, QLocale::UnitedStates));

    foreach(const QString &path, translationDirectories()) {
        QDir dir(path);
        QStringList fileNames = dir.entryList(QStringList("*.qm"), QDir::Files, QDir::Name);
        foreach(QString name, fileNames) {
            // Keep stripping begin underscores, until we have a proper locale
            while (!name.isEmpty()) {
                QLocale testLocale(name);
                if (testLocale.language() == QLocale::C) {
                    int split = name.indexOf('_');
                    if (split == -1) {
                        break;
                    }
                    name = name.mid(split + 1);
                }
                else {
                    if (!result.contains(testLocale)) {
                        result.append(testLocale);
                    }
                    break; // Continue loop
                }
            }
        }
    }

    return result;
}

void MainWindow::switchLocale()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QLocale locale = action->data().toLocale();
    if (locale.language() != QLocale::C) {
        qDebug() << "Switching to" << QLocale::languageToString(locale.language());
        setLocale(locale);
    }
}

void MainWindow::setLocale(const QLocale& locale)
{
    m_locale = locale;
    setLocale(locale, "GraphicsRulesMaker", &m_libraryTranslator);
    setLocale(locale, "GraphicsRulesMakerUi", &m_uiTranslator);
    this->ui->retranslateUi(this);

    // Recreate plugin widget to make sure it's translated
    selectGame(this->ui->gameSelect->currentIndex());
}

void MainWindow::setLocale(const QLocale& locale, const QString& prefix, QTranslator* translator)
{
    foreach(const QString path, translationDirectories()) {
        qDebug() << "TRYING translation with prefix " << prefix << " from dir " << path << " for locale " << QLocale::languageToString(locale.language());
        if (translator->load(locale, prefix, "_", path)) {
            qDebug() << "LOADED translation with prefix " << prefix << " from dir " << path << " for locale " << QLocale::languageToString(locale.language());
            return;
        }
    }
    qDebug() << "FAILED loading translation with prefix " << prefix << " for locale " << QLocale::languageToString(locale.language());
}

void MainWindow::about()
{
    AboutDialog *dialog = new AboutDialog(this, Qt::Dialog);
    connect(dialog, SIGNAL(accepted()), dialog, SLOT(deleteLater()));
    dialog->show();
}

MainWindow::~MainWindow()
{
    // Save state
    QSettings s;
    s.beginGroup("ui");
    s.setValue("window/geometry", saveGeometry());
    s.setValue("window/state", saveState());
    s.setValue("window/locale", m_locale);
    s.setValue("videocards/splitterstate", ui->videoCardsSplitter->saveState());
    s.setValue("videocards/treeviewheaderstate", ui->videoCardsView->header()->saveState());
    s.setValue("game/id", m_currentPlugin->id());
    delete ui;
}
