#ifndef HELPERCLIENT_H
#define HELPERCLIENT_H

#include <QObject>
#include <QLocalSocket>

/**
 * Client which handles the communication. It expects instructions from a server, after it's
 * established a connection.
 */
class HelperClient : public QObject
{
    Q_OBJECT

public:
    HelperClient(QObject* parent = nullptr);
    ~HelperClient();

    void connectToServer(const QString& name = "GraphicsRulesMaker");

private:
    QLocalSocket m_socket;

};

#endif // HELPERCLIENT_H
