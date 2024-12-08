#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QMessageBox>
#include <QApplication>
#include <QStandardItemModel>
#include "commande.h"
#include <QSerialPortInfo>
#include <QSerialPort>
#include "seriallink.h"
#include "EMAILSENDER.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int m_currentPage = 0;
        const int m_pageSize = 3;
        //QSortFilterProxyModel* proxyModel;
        QStandardItemModel *model;;
        QString filterBadWords(const QString &text);
private slots:
    void on_pb_ajouter_clicked();
    void on_pb_supprimer_clicked();

    void on_pb_modifier_clicked();

    void on_pushButton_rechercher_clicked();
    void on_pushButton_chercher_nom_clicked();
    void on_pushButton_PDF_clicked();
    void on_triID_clicked();
    void on_nom_az_clicked();
    void on_nom_desc_clicked();
    void on_ID_desc_clicked();
    void on_qrcodegen_2_clicked();
    void on_pushButton_2_clicked();
    void addPagination();
    void on_moisuivant_clicked();
    void on_moisprec_clicked();
    void onEmailStatusReceived(const QString &status);
    void on_connectButton_clicked();
    void on_disconnectButton_clicked();
    void on_sendCommandButton_clicked();
    void listAvailablePorts();
    void processData(const QString &data);
    void on_sendButton_clicked();





private:
    Ui::MainWindow *ui;
    Commande c;;
     Commande *commande;;
     seriallink *serial;
    QByteArray data;

};

#endif // MAINWINDOW_H
