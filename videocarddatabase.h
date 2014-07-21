#ifndef VIDEOCARDDATABASE_H
#define VIDEOCARDDATABASE_H

#include <QtCore/QAbstractItemModel>

#include <QtCore/QUrl>

#define MAX_VENDOR_IDS 8

struct VideoCard {
    quint16 deviceId;
    QString name;
};

struct VideoCardVendor {
    QList<quint16> vendorIds;
    QString name;
    QList<VideoCard> cards;
};

class VideoCardDatabase : public QAbstractItemModel
{
    Q_OBJECT

private:
    QList<VideoCardVendor> m_vendors;

    quintptr devicePointer(quint16 vendorId, quint16 deviceId = 0x00) const;
    // 0: fail
    // 1: only vendor
    // 2: vendor + device
    int extractDevicePointer(quintptr internalId, quint16* vendorId, quint16* deviceId) const;

    int indexOfVendor(quint16 vendorId) const;

public:
    explicit VideoCardDatabase(QObject *parent = 0);

    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& child) const;

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    virtual ~VideoCardDatabase();

    virtual void write(QIODevice *target) const;

public slots:
    void loadFrom(QString fileName);
    void loadFrom(QIODevice *file);
};

#endif // VIDEOCARDDATABASE_H
