#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>
#include <QString>

class Connection
{
public:
    Connection();              // Constructor
    bool createconnect();      // Method to create the connection

private:
    QSqlDatabase db;           // Database object
};

#endif // CONNECTION_H
