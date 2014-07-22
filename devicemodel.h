#ifndef DEVICEMODEL_H
#define DEVICEMODEL_H

#include <QtCore/QAbstractTableModel>
#include <QtCore/QList>

struct GraphicsMode
{
    quint16 width;
    quint16 height;
    quint16 refreshRate;
    bool operator ==(const GraphicsMode &other) const;
    bool operator <(const GraphicsMode &other) const;
};

struct GraphicsDevice
{
    QString name;
    QString driver;
    QString display;
    quint64 memory;
    quint16 vendorId;
    quint16 deviceId;
    QList<GraphicsMode> modes;
};

uint qHash(GraphicsMode key, uint seed = 0);

class DeviceModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    QList<GraphicsDevice> m_devices;

public:
    explicit DeviceModel(QObject *parent = 0);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const {
        if (parent.isValid()) {
            return 0;
        }
        return m_devices.count();
    }
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const {
        Q_UNUSED(parent);
        return 7;
    }

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    GraphicsDevice device(int row) const { return m_devices.at(row); }
    QList<GraphicsMode> allModes();

signals:
    void loaded();
    void loadFailed();

public slots:
    void load();
};

#endif // DEVICEMODEL_H
