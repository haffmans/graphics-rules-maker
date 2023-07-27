#include "graphicsruleswriter.h"

#include <QtDebug>
#include <QSettings>

#include "gamewriterfactory.h"
#include "gamewriterinterface.h"

GraphicsRulesWriter::GraphicsRulesWriter(GameWriterFactory* factory, VideoCardDatabase* cardDatabase, QObject* parent)
    : QObject(parent)
    , m_plugin(nullptr)
    , m_factory(factory)
    , m_cardDatabase(cardDatabase)
    , m_gamePath()
{
}

GraphicsRulesWriter::~GraphicsRulesWriter()
{
}

GameWriterInterface* GraphicsRulesWriter::plugin() const
{
    return m_plugin;
}

bool GraphicsRulesWriter::loadPlugin(const QString& gameId)
{
    auto result = m_factory->plugin(gameId);
    if (result != nullptr) {
        m_plugin = result;
        loadSettings();
        emit pluginLoaded(gameId);
        return true;
    }
    return false;
}

VideoCardDatabase* GraphicsRulesWriter::videoCardDatabase() const
{
    return m_cardDatabase;
}


QDir GraphicsRulesWriter::gamePath() const
{
    return m_gamePath;
}

void GraphicsRulesWriter::setGamePath(const QDir& path)
{
    if (path != m_gamePath) {
        m_gamePath = path;

        if (m_plugin) {
            m_cardDatabase->loadFrom(m_plugin->databaseFileName(m_gamePath).filePath());
        }

        emit gamePathChanged(m_gamePath);
    }
}

QVariantMap GraphicsRulesWriter::currentSettings() const
{
    return m_currentSettings;
}

void GraphicsRulesWriter::loadSettings()
{
    if (m_plugin == nullptr) {
        return;
    }

    QSettings s;
    QVariantMap map;
    s.beginGroup(m_plugin->id());
    map = recursiveLoadSettings(&s);
    s.endGroup();

    // Special treatment for the installation (game) path
    if (map.contains("path")) {
        auto gameDir = map["path"].toString();
        qDebug() << "Use path from saved settings:" << gameDir;
        setGamePath(gameDir);
        map.remove("path");
    }
    else {
        // Auto-locate game
        qDebug() << "Locate game";
        setGamePath(m_plugin->findGameDirectory());
    }

    m_currentSettings = map;
    emit currentSettingsChanged(m_currentSettings);
}

QVariantMap GraphicsRulesWriter::recursiveLoadSettings(QSettings* settings)
{
    QVariantMap result;

    for(const auto& item: settings->childKeys()) {
        result[item] = settings->value(item);
    }

    for(const auto& group: settings->childGroups()) {
        settings->beginGroup(group);
        result[group] = recursiveLoadSettings(settings);
        settings->endGroup();
    }

    return result;
}

void GraphicsRulesWriter::saveSettings(const QVariantMap& settings)
{
    if (m_plugin == nullptr) {
        return;
    }

    m_currentSettings = settings;

    auto persistSettings = settings;
    persistSettings["path"] = QDir::toNativeSeparators(m_gamePath.absolutePath()); // force this, regardless of input

    QSettings s;
    s.beginGroup(m_plugin->id());
    recursiveSaveSettings(persistSettings, &s);
    s.endGroup();

    emit currentSettingsChanged(m_currentSettings);
}

void GraphicsRulesWriter::recursiveSaveSettings(const QVariantMap& map, QSettings* settings) const
{
    for (auto item = map.cbegin(); item != map.cend(); ++item) {
        auto& key = item.key();
        auto& value = item.value();

        if (value.type() == QVariant::Map) {
            settings->beginGroup(key);
            recursiveSaveSettings(value.toMap(), settings);
            settings->endGroup();
        }
        else {
            settings->setValue(key, value);
        }
    }
}
bool GraphicsRulesWriter::createBackups()
{
    const QDir graphicsRulesDestination = m_plugin->rulesFileName(gamePath()).dir();
    const QDir videoCardsDestination = m_plugin->databaseFileName(gamePath()).dir();

    return backupTo(graphicsRulesDestination, videoCardsDestination);
}

bool GraphicsRulesWriter::createBackupsAt(const QDir& destination)
{
    return backupTo(destination, destination);
}

bool GraphicsRulesWriter::backupTo(const QDir& graphicsRulesDestinationDir, const QDir& videoCardsDestinationDir)
{
    const QString bakSuffix(".bak");

    // Either copy existing (in the source directory) back-up files,
    const QFileInfo graphicsRulesFile(m_plugin->rulesFileName(graphicsRulesDestinationDir));
    const QFileInfo graphicsRulesBackup = graphicsRulesFile.absoluteFilePath() + bakSuffix;
    const QFileInfo graphicsRulesDestination = graphicsRulesDestinationDir.absoluteFilePath(graphicsRulesBackup.fileName());

    const QFileInfo videoCardsFile = m_plugin->databaseFileName(videoCardsDestinationDir);
    const QFileInfo videoCardsBackup = videoCardsFile.absoluteFilePath() + bakSuffix;
    const QFileInfo videoCardsDestination = videoCardsDestinationDir.absoluteFilePath(videoCardsBackup.fileName());

    // Copy existing back-up files if they exist; the original Graphics Rules file otherwise
    const auto graphicsRulesSource = (graphicsRulesBackup.exists()) ? graphicsRulesBackup : graphicsRulesFile;
    const auto videoCardsSource = (videoCardsBackup.exists()) ? videoCardsBackup : videoCardsFile;

    // Create backups if they don't exist yet (we won't overwrite files).
    // Note: QFile::copy() will not overwrite files, but it does return false
    //       if the file exists. Hence we check ourselves.
    bool backupOk = true;
    if (!graphicsRulesDestination.exists()) {
        // graphicsRulesSource != graphicsRulesDestination here (because the source exists)
        qDebug() << "- Creating Graphics Rules backup to" << graphicsRulesDestination.absoluteFilePath();
        backupOk = backupOk && QFile::copy(graphicsRulesSource.absoluteFilePath(), graphicsRulesDestination.absoluteFilePath());
    }
    if (!videoCardsDestination.exists()) {
        // videoCardsSource != videoCardsDestination here (because the source exists)
        qDebug() << "- Creating Video Cards backup to" << videoCardsDestination.absoluteFilePath();
        backupOk = backupOk && QFile::copy(videoCardsSource.absoluteFilePath(), videoCardsDestination.absoluteFilePath());
    }

    qDebug() << "- Backups in place:" << (backupOk ? "Yes" : "No");

    return backupOk;
}

bool GraphicsRulesWriter::writeFiles(const QVariantMap& settings)
{
    const QDir graphicsRulesDestination = m_plugin->rulesFileName(gamePath()).dir();
    const QDir videoCardsDestination = m_plugin->databaseFileName(gamePath()).dir();

    saveSettings(settings);

    return writeFilesTo(graphicsRulesDestination, videoCardsDestination, settings);
}

bool GraphicsRulesWriter::writeFiles(const QDir& destination, const QVariantMap& settings)
{
    return writeFilesTo(destination, destination, settings);
}

bool GraphicsRulesWriter::writeFilesTo(const QDir& graphicsRulesDestinationDir, const QDir& videoCardsDestinationDir, const QVariantMap& settings)
{
    const QFileInfo graphicsRulesDestination = graphicsRulesDestinationDir.absoluteFilePath(m_plugin->rulesFileName());
    const QFileInfo videoCardsDestination = videoCardsDestinationDir.absoluteFilePath(m_plugin->databaseFileName());

    qDebug() << "- Open Graphics Rules file" << graphicsRulesDestination.absoluteFilePath();
    QFile graphicsRulesOut(graphicsRulesDestination.absoluteFilePath());
    if (!graphicsRulesOut.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    qDebug() << "- Call plugin to write contents";
    m_plugin->write(settings, &graphicsRulesOut);
    graphicsRulesOut.close();

    qDebug() << "- Open Video Cards file" << videoCardsDestination.absoluteFilePath();
    QFile videoCardsOut(videoCardsDestination.absoluteFilePath());
    if (!videoCardsOut.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    qDebug() << "- Call cards database to write contents";
    m_cardDatabase->write(&videoCardsOut);
    videoCardsOut.close();

    return true;
}

#include "moc_graphicsruleswriter.cpp"
