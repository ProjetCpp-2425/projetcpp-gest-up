#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include "mainwindow.h"
#include "connection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Make sure the QODBC driver is loaded
    if (!QSqlDatabase::isDriverAvailable("QODBC")) {
        qDebug() << "Error: QODBC driver not available!";
        return -1;
    }

    Connection c;
    if (c.createconnect()) {
        MainWindow w;
        w.show();
        return a.exec();
    } else {
        qDebug() << "Database connection failed.";
        return -1;
    }
}
