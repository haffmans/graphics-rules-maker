#ifndef GRAPHICSRULESWRITER_H
#define GRAPHICSRULESWRITER_H

#include <QObject>
#include <QDir>
#include <memory>

#include "graphicsrulesmaker/videocarddatabase.h"
#include "graphicsrulesmaker/graphicsrulesmaker_config.h"

class GameWriterInterface;
class GameWriterFactory;
class VideoCardDatabase;

/**
 * This manages the active plugin, writes graphics rules files etc.
 */
class GRAPHICSRULESMAKER_EXPORT GraphicsRulesWriter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(GameWriterInterface* plugin READ plugin NOTIFY pluginLoaded)
    Q_PROPERTY(VideoCardDatabase* videoCardDatabase READ videoCardDatabase CONSTANT)
    Q_PROPERTY(QDir gamePath READ gamePath WRITE setGamePath NOTIFY gamePathChanged)

public:
    GraphicsRulesWriter(GameWriterFactory* factory, VideoCardDatabase* cardDatabase, QObject* parent = nullptr);
    ~GraphicsRulesWriter();

    GameWriterInterface* plugin() const;
    VideoCardDatabase* videoCardDatabase() const;

    QDir gamePath() const;
    void setGamePath(const QDir& path);

public slots:
    bool loadPlugin(const QString& gameId);
    bool createBackups();
    bool createBackupsAt(const QDir& destination);
    bool writeFiles(const QVariantMap& settings);
    bool writeFiles(const QDir& destination, const QVariantMap& settings);

signals:
    void pluginLoaded(const QString& gameId);
    void gamePathChanged(const QDir& gamePath);

private:
    // Helpers with split destinations (they'll usually be the same, but could technically be different)
    bool backupTo(const QDir& graphicsRulesDestinationDir, const QDir& videoCardsDestinationDir);
    bool writeFilesTo(const QDir& graphicsRulesDestinationDir, const QDir& videoCardsDestinationDir, const QVariantMap& settings);

    GameWriterFactory* m_factory;
    GameWriterInterface* m_plugin;
    VideoCardDatabase* m_cardDatabase;
    QDir m_gamePath;
};

#endif // GRAPHICSRULESWRITER_H
