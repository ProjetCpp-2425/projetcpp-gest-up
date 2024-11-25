#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QPushButton>
#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidget>
#include <QFont>
#include <QFontMetrics>
#include <QPageSize>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QBarSet>
#include <QBarSeries>
#include <QChart>
#include <QChartView>
#include <QValueAxis>
#include <QBarCategoryAxis>

#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>






QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onValidateButtonClicked();
    void onCancelButtonClicked();
    void onDeleteButtonClicked();
    void onSortComboBoxChanged();
    void onSearchComboBoxChanged();   // Nouvelle fonction de recherche
    void onPdfButtonClicked();
    void onStatsButtonClicked(); // Fonction appelée lorsque le bouton est cliqué
    void showSalaryExtremes(); // Statistique : Salaires les plus élevés et les plus bas
    //void onCellModified(int row, int column); // **Nouveau slot pour gérer la modification des cellules**
    void showGenderChart(); // Nouvelle fonction pour afficher la statistique
    void onGenerateQrButtonClicked();







private:
    Ui::MainWindow *ui;

    bool isShowingMessage;
    QLineEdit *idEdit;
    QLineEdit *prenomEdit;
    QLineEdit *nomEdit;
    QLineEdit *posteEdit;
    QLineEdit *salaireEdit;
    QComboBox *sexeComboBox;
    QTableWidget *tableWidget;
    QPushButton *validateButton;
    QPushButton *cancelButton;
    QPushButton *deleteButton;
    QComboBox *sortComboBox;
    QComboBox *searchComboBox;        // ComboBox de recherche
    QLineEdit *searchTextLineEdit;    // Ligne de texte pour la valeur de recherche
    QComboBox *statsComboBox; // ComboBox pour choisir la statistique
    QPushButton *statsButton; // Bouton pour afficher la statistique
    bool isAddingEmployee; // Nouveau drapeau pour distinguer l'ajout des modifications
    QSqlDatabase db;
    QMainWindow *chartWindow = nullptr;




};


#endif // MAINWINDOW_H
