#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSerialPort>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QPixmap>
#include <QPushButton>
#include <QItemSelectionModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QFont>
#include <QPdfWriter>
#include <QPainter>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_addButton_clicked();
    void on_removeButton_clicked();
    void on_editButton_clicked();
    void loadLivreurData();
    void on_imageButton_clicked();
    void onTableSelectionChanged();
    void onRestoreClicked();
    void onBackupClicked();
    void on_statistique_clicked();
    void onSortButtonClicked();
    void onrechercheclicked();  // Declaration for the recherche function
    void generatePdf();         // Declaration for generatePdf function
    void on_verifyButton_clicked(); // Declaration for the verify button function

private:
    Ui::MainWindow *ui;
    QSerialPort *serialPort;
    QString enteredCode;

    void setupSerialPort();
    void readKeypadData();
    void loadLivreurImage(const QString &idLivreur);
};

#endif // MAINWINDOW_H
