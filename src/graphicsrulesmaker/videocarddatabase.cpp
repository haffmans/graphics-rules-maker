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

#include "videocarddatabase.h"

#include "pci.vendors.h"

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>
#include <QtCore/QSize>
#include <QtCore/QRegularExpression>

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

QVariant VideoCardDatabase::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case 0:
                return tr("Vendor Name");
            case 1:
                return tr("Vendor Id");
            case 2:
                return tr("Card Name");
            case 3:
                return tr("Card Id");
        }
    }

    if (orientation == Qt::Horizontal && role == Qt::SizeHintRole) {
        switch(section) {
            case 0:
            case 2:
                return QSize(200, 20);
            case 1:
            case 3:
                return QSize(30, 20);
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant VideoCardDatabase::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::ToolTipRole)) {
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
                vendorIds << formatId(id);
            }
            return vendorIds.join(", ");
        }
        case 2:
            return (deviceId) ? card.name : QVariant();
        case 3:
            return (deviceId) ? formatId(deviceId) : QVariant();
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
    QRegularExpression vendorRe("^\\s*vendor\\s+\"(.*)\"((\\s+0x[0-9A-F]{4})+)\\s*$", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression idRe("0x([0-9A-F]{4})", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression cardRe("^\\s*card\\s+0x([0-9A-F]{4})\\s+\"(.*)\"\\s*$", QRegularExpression::CaseInsensitiveOption);

    // Open device if necessary, otherwise check that we can read it
    bool fileWasOpen = file->isOpen();
    if ((!file->isOpen() && !file->open(QIODevice::ReadOnly | QIODevice::Text)) ||
        (!file->isReadable() || file->atEnd())) {
        qCritical() << "Could not open file!";
        return;
    }

    qDebug() << "Reset model";
    beginResetModel();

    m_vendors.clear();

    // Read file line by line
    QTextStream stream(file);
    QMutableListIterator<VideoCardVendor> vendor(m_vendors);

    while (!stream.atEnd()) {
        QString line(stream.readLine());

        auto vendorMatch = vendorRe.match(line);
        auto idMatch = idRe.match(line);
        auto cardMatch = cardRe.match(line);

        if (vendorMatch.hasMatch()) {
            qDebug() << "- Vendor" << vendorMatch.captured(1);
            // Add new vendor
            vendor.insert(VideoCardVendor());
            vendor.value().name = vendorMatch.captured(1);

            QString vendorIds = vendorMatch.captured(2);
            for (const auto& id: idRe.globalMatch(vendorIds)) {
                vendor.value().vendorIds.append(id.captured(1).toInt(0, 16));
            }
        }
        // Only check for card if we have a current vendor
        if (vendor.hasPrevious() && cardMatch.hasMatch()) {
            VideoCard card;
            card.name = cardMatch.captured(2);
            card.deviceId = cardMatch.captured(1).toInt(0, 16);
            vendor.value().cards.append(card);
        }
    }

    endResetModel();
    qDebug() << "Video cards loaded";

    // Close file again if we opened it earlier
    if (!fileWasOpen) {
        qDebug() << "Closing video cards file";
        file->close();
    }
}

void VideoCardDatabase::addVendor(quint16 vendorId)
{
    if (GraphicsRulesMaker::vendorIds.contains(vendorId)) {
        addVendor(GraphicsRulesMaker::vendorIds.value(vendorId), vendorId);
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
        addVendor(vendorId);
    }
    // Try again
    vendorIndex = indexOfVendor(vendorId);
    if (vendorIndex == -1) {
        // Adding must've failed -> abort
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
# Generated by GraphicsRulesMaker
#   www.simsnetwork.com
#
)";

    foreach(const VideoCardVendor &vendor, m_vendors) {
        stream << "#############################################################################\n";
        stream << "vendor \"" << vendor.name << "\"";
        foreach(quint16 vendorId, vendor.vendorIds) {
            stream << " " << formatId(vendorId);
        }
        stream << '\n';

        foreach(const VideoCard &card, vendor.cards) {
            stream << "  card " << formatId(card.deviceId) <<
                " \"" << card.name << "\"\n";
        }

        stream << "end\n\n";
    }

    if (!wasOpen) {
        target->close();
    }
}

QString VideoCardDatabase::formatId(quint16 id) const
{
    return QString("0x%1").arg(id, 4, 16, QChar('0'));
}

VideoCardDatabase::~VideoCardDatabase()
{
}

#include "moc_videocarddatabase.cpp"
