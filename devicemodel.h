#ifndef DEVICEMODEL_H
#define DEVICEMODEL_H

#include <QtCore/QAbstractTableModel>
#include <QtCore/QList>

struct GraphicsDevice
{
    QString name;
    QString driver;
    quint64 memory;
    quint16 vendorId;
    quint16 deviceId;
};

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
        return 5;
    }

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    GraphicsDevice device(int row) const { return m_devices.at(row); }

signals:
    void loaded();
    void loadFailed();

public slots:
    void load();
};

#endif // DEVICEMODEL_H
