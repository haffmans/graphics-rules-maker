#ifndef VIDEOCARDDATABASE_H
#define VIDEOCARDDATABASE_H

#include <QtCore/QAbstractTableModel>

class VideoCardDatabase : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit VideoCardDatabase(QObject *parent = 0);

signals:

public slots:

};

#endif // VIDEOCARDDATABASE_H
