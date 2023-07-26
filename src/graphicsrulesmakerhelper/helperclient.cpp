#include "helperclient.h"

HelperClient::HelperClient(QObject* parent)
    : QObject(parent)
{
}

HelperClient::~HelperClient()
{
}

void HelperClient::connectToServer(const QString& name)
{
}


#include "moc_helperclient.cpp"
