#ifndef FINANCEOP_H
#define FINANCEOP_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QVariant>
#include <QDebug>
#include <QDate>
#include<QSqlError>
#include <QSqlQueryModel>
#include <QTableView>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QTextBrowser>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QMessageBox>

class financeop {
public:
    financeop();
    financeop(const QString& dbPath);
    bool open();
    void close();
    bool insertTransaction( const QString& modePaiement, const QString& type,
                           const QString& categorieTransaction, const QDate& date, float montant);
    void displayTransactions(QTableView *tableView);
    bool deleteTransaction(int idTransaction);
    int sort=0;
    int sort2=0;
    int ID_sort=0;
    QDate date_sort=QDate::fromString("02/02/2024","dd/MM/yyyy");
    float montant;
    // Function to update a transaction by ID
    bool updateTransaction(int idTransaction, const QString &modePaiement, const QString &type,
                           const QString &categorieTransaction, const QDate &date, float montant);
    void exportToCSV(QTableView *tableView);
    int countTransactions();
    double totalMontant();
    double totaldepense();
    double totalrevenue();
    int countTransactions(QString y);
    double totalMontant(QString y);
    double totaldepense(QString y);
    double totalrevenue(QString y);
    void updateLog(const QString &filePath, const QString &operationType, const QString &newLogEntry);
    void showhistorique(QTextBrowser *tt);
    void sendWindowsNotification(const QString &title, const QString &message, int duration );
    bool depenserevenue();
    //void sortTransactionsByType();  // Déclaration de la fonction de tri
     QList<QPair<QString, qreal>>  fetchAndSumTransactions(int n) ;
    void on_excel2_clicked();

private:
    QSqlDatabase db;
};

#endif // FINANCEOP_H
