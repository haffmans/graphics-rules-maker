#include "videocarddatabase.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>
#include <QtCore/qsize.h>

VideoCardDatabase::VideoCardDatabase(QObject *parent) :
    QAbstractItemModel(parent)
{
}

QModelIndex VideoCardDatabase::index(int row, int column, const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        // Use root pointer to vendor
        if (row >= m_vendors.count()) {
            return QModelIndex();
        }
        quintptr id = devicePointer(m_vendors.at(row).vendorIds.first());
        return createIndex(row, column, id);
    }
    else if (!parent.parent().isValid()) {
        // Second level: select device
        VideoCardVendor vendor = m_vendors.at(parent.row());
        quintptr id = devicePointer(vendor.vendorIds.first(), vendor.cards.at(row).deviceId);
        return createIndex(row, column, id);
    }
    return QModelIndex();
}

quintptr VideoCardDatabase::devicePointer(quint16 vendorId, quint16 deviceId) const
{
    quintptr result = (quintptr)(((vendorId << 16) | deviceId) & 0xFFFFFFFF);

    quint16 vendorBack, deviceBack;
    int extractResult = extractDevicePointer(result, &vendorBack, &deviceBack);

    return result;
}

int VideoCardDatabase::extractDevicePointer(quintptr internalId, quint16* vendorId, quint16* deviceId) const
{
    *vendorId = (quint16)((internalId >> 16) & 0xFFFF);
    *deviceId = (quint16)(internalId);

    if (*vendorId && *deviceId) {
        return 2;
    }
    else if (*vendorId) {
        return 1;
    }
    else {
        return 0;
    }
}

int VideoCardDatabase::indexOfVendor(quint16 vendorId) const
{
    // Find vendor
    int index = 0;
    bool found = false;
    foreach(const VideoCardVendor &vendor, m_vendors) {
        if (vendor.vendorIds.contains(vendorId)) {
            found = true;
            break;
        }
        ++index;
    }

    return (found) ? index : -1;
}


QModelIndex VideoCardDatabase::parent(const QModelIndex& child) const
{
    quint16 vendorId, deviceId;

    // Must have card, otherwise we have no parent
    if (extractDevicePointer(child.internalId(), &vendorId, &deviceId) != 2) {
        return QModelIndex();
    }

    int index = indexOfVendor(vendorId);
    // Create index for vendor
    if (index != -1)  {
        return createIndex(index, 0, devicePointer(vendorId));
    }
    return QModelIndex();
}

int VideoCardDatabase::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        return m_vendors.count();
    }

    if (parent.column() > 0) {
        return 0;
    }

    quint16 vendorId, deviceId;
    if (extractDevicePointer(parent.internalId(), &vendorId, &deviceId) != 1) {
        // We have a card (i.e. no children) or something went wrong
        return 0;
    }

    int index = indexOfVendor(vendorId);
    if (index == -1) {
        return 0;
    }
    return m_vendors.at(index).cards.count();
}

int VideoCardDatabase::columnCount(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        return 4;
    }
    else if (parent.isValid() && !parent.parent().isValid()) {
        return 4;
    }
    else {
        return 0;
    }
}

QVariant VideoCardDatabase::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }

    quint16 vendorId, deviceId;

    // Extract ids from internal pointer
    if (!extractDevicePointer(index.internalId(), &vendorId, &deviceId)) {
        return QModelIndex();
    }

    // We will get a vendor here
    VideoCardVendor vendor = *(std::find_if(
        m_vendors.constBegin(), m_vendors.constEnd(),
            [vendorId] (const VideoCardVendor& v) {
                return v.vendorIds.contains(vendorId);
            }
    ));

    // Also try to get the card now
    VideoCard card;
    card.deviceId = deviceId;
    card.name = QString();
    if (deviceId) {
        card = *(std::find_if(
            vendor.cards.constBegin(), vendor.cards.constEnd(),
            [deviceId] (const VideoCard& c) {
                return c.deviceId == deviceId;
            }
        ));
    }

    // We now *always* have a vendor, possibly a card
    switch(index.column()) {
        case 0:
            return vendor.name;
        case 1: {
            QStringList vendorIds;
            foreach(quint16 id, vendor.vendorIds) {
                vendorIds << "0x" + QString::number(id, 16);
            }
            return vendorIds.join(", ");
        }
        case 2:
            return (deviceId) ? card.name : QVariant();
        case 3:
            return (deviceId) ? "0x" + QString::number(deviceId, 16) : QVariant();
    }

    return QVariant();
}

void VideoCardDatabase::loadFrom(QString fileName)
{
    qDebug() << "Opening" << fileName;

    QFile file(fileName);
    loadFrom(&file);
}

void VideoCardDatabase::loadFrom(QIODevice* file)
{
    QRegExp vendorMatch("^\\s*vendor\\s+\"(.*)\"((\\s+0x[0-9A-F]{4})+)\\s*$", Qt::CaseInsensitive);
    QRegExp idMatch("0x([0-9A-F]{4})", Qt::CaseInsensitive);
    QRegExp cardMatch("^\\s*card\\s+0x([0-9A-F]{4})\\s+\"(.*)\"\\s*$", Qt::CaseInsensitive);

    // Open device if necessary, otherwise check that we can read it
    bool fileWasOpen = file->isOpen();
    if ((!file->isOpen() && !file->open(QIODevice::ReadOnly | QIODevice::Text)) ||
        (!file->isReadable() || file->atEnd())) {
        return;
    }

    beginResetModel();

    // Read file line by line
    QTextStream stream(file);
    QMutableListIterator<VideoCardVendor> vendor(m_vendors);

    while (!stream.atEnd()) {
        QString line(stream.readLine());

        if (vendorMatch.exactMatch(line)) {
            // Add new vendor
            vendor.insert(VideoCardVendor());
            vendor.value().name = vendorMatch.cap(1);

            QString vendorIds = vendorMatch.cap(2);
            int offset = 0;
            while ((offset = idMatch.indexIn(vendorIds, offset)) != -1) {
                vendor.value().vendorIds.append(idMatch.cap(1).toInt(0, 16));
                offset += idMatch.matchedLength();
            }
        }
        // Only check for card if we have a current vendor
        else if (vendor.hasPrevious() && cardMatch.exactMatch(line)) {
            VideoCard card;
            card.name = cardMatch.cap(2);
            card.deviceId = cardMatch.cap(1).toInt(0, 16);
            vendor.value().cards.append(card);
        }
    }

    endResetModel();

    // Close file again if we opened it earlier
    if (!fileWasOpen) {
        file->close();
    }
}

void VideoCardDatabase::addVendor(const QString& name, quint16 vendorId)
{
    addVendor(name, QList<quint16>() << vendorId);
}

void VideoCardDatabase::addVendor(const QString& name, const QList< quint16 >& vendorIds)
{
    // Make sure we don't already have the vendor
    foreach(quint16 vendorId, vendorIds) {
        if (indexOfVendor(vendorId) != -1) {
            return;
        }
    }

    VideoCardVendor vendor;
    vendor.name = name;
    vendor.vendorIds = vendorIds;

    beginInsertRows(QModelIndex(), m_vendors.count(), m_vendors.count());
    m_vendors.append(vendor);
    endInsertRows();
}

void VideoCardDatabase::addDevice(quint16 vendorId, quint16 deviceId, const QString& name)
{
    // Find vendor
    int vendorIndex = indexOfVendor(vendorId);
    if (vendorIndex == -1) {
        return;
    }
    VideoCardVendor vendor = m_vendors.at(vendorIndex);

    // Find card in vendor
    foreach(VideoCard findCard, vendor.cards) {
        if (findCard.deviceId == deviceId) {
            return;
        }
    }

    // Add card
    VideoCard card;
    card.deviceId = deviceId;
    card.name = name;

    QModelIndex parent = index(vendorIndex, 0);
    beginInsertRows(parent, vendor.cards.count(), vendor.cards.count());
    vendor.cards.append(card);
    m_vendors.replace(vendorIndex, vendor);
    endInsertRows();
}

QModelIndex VideoCardDatabase::findDevice(quint16 vendorId, quint16 deviceId) const
{
    // Locate vendor
    int row = indexOfVendor(vendorId);
    if (row == -1) {
        return QModelIndex();
    }
    VideoCardVendor vendor = m_vendors.at(row);

    // Locate card in vendor
    int cardIndex = 0;
    QList<VideoCard>::const_iterator iCard;
    for (iCard = vendor.cards.constBegin(); iCard != vendor.cards.constEnd(); ++iCard, ++cardIndex) {
        if (iCard->deviceId == deviceId) {
            break;
        }
    }

    if (iCard == vendor.cards.constEnd()) {
        // Not found
        return QModelIndex();
    }

    // Found at row cardIndex
    QModelIndex parent = index(row, 0);
    return index(cardIndex, 0, parent);
}

bool VideoCardDatabase::contains(quint16 vendorId, quint16 deviceId) const
{
    return findDevice(vendorId, deviceId).isValid();
}

void VideoCardDatabase::write(QIODevice* target) const
{
    bool wasOpen = target->isOpen();
    if (!wasOpen) {
        target->open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text);
    }

    QTextStream stream(target);

    stream << R"(#
# ID to video card mappings
#   maps the vendor id to vendor name,
#   maps the card id to card name
#
# Generated by GraphicRulesMaker
#   www.simsnetwork.com
#
)";

    foreach(const VideoCardVendor &vendor, m_vendors) {
        stream << "#############################################################################\n";
        stream << "vendor \"" << vendor.name << "\"";
        foreach(quint16 vendorId, vendor.vendorIds) {
            stream << " 0x" << QString::number(vendorId, 16);
        }
        stream << '\n';

        foreach(const VideoCard &card, vendor.cards) {
            stream << "  card 0x" << QString::number(card.deviceId, 16) <<
                " \"" << card.name << "\"\n";
        }

        stream << "end\n\n";
    }

    if (!wasOpen) {
        target->close();
    }
}

VideoCardDatabase::~VideoCardDatabase()
{
}
