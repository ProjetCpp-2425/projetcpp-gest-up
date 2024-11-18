#include "finance.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlDriver>
#include "PieChartWidget.h"
#include "ui_finance.h"
#include <QProcess>

finance::finance(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::finance)
{
    ui->setupUi(this);

     operate= financeop();
     qDebug()<<QSqlDatabase::drivers();
     if (operate.open()){
     operate.displayTransactions(ui->finance_table);
    operate.close();}
     ui->finance_table->verticalHeader()->hide();
     ui->finance_table->setSelectionBehavior(QAbstractItemView::SelectRows);
     ui->update_finance->hide();
     ui->annuler->hide();

}




finance::~finance()
{
    delete ui;
}


void finance::on_ajouter_line_clicked()
{

    QString montant=ui->montant_line->text();
    QString mode= ui->mode_line->currentText();
    QString type= ui->type_line->currentText();
    QDate date= ui->date_line->date();
    QString cat=ui->cate_line->currentText();
    if(cat.isEmpty()||type.isEmpty()||montant.isEmpty()||mode.isEmpty())
        return;
    if (operate.open()) {
            // Insert a transaction
            if (operate.insertTransaction(mode, type, cat, date, montant.toFloat())) {
                qDebug() << "Transaction inserted successfully.";
            } else {
                qDebug() << "Failed to insert transaction.";
            }
            operate.displayTransactions(ui->finance_table);
            operate.close();
        }
    QList<QComboBox*> lineEdits = ui->groupBox_finance->findChildren<QComboBox*>();
       for (QComboBox* lineEdit : lineEdits) {
           lineEdit->setCurrentIndex(0); // Effacer le texte
       }
}



void finance::on_supprimer_clicked()
{
    QModelIndexList selectedRows = ui->finance_table->selectionModel()->selectedRows();

        if (selectedRows.isEmpty()) {
            qDebug() << "No row selected.";
            return; // No row selected, exit the function
        }

        // Get the first selected row (you could also loop through if multiple selections are allowed)
        int row = selectedRows.first().row();

        // Get the ID from the selected row
        QAbstractItemModel *model = ui->finance_table->model();
        int idTransaction = model->data(model->index(row, 0)).toInt();
        operate.open();
        operate.deleteTransaction(idTransaction);
        operate.displayTransactions(ui->finance_table);
        operate.close();

}


void finance::on_pushButton_6_clicked()
{

}


void finance::on_sortdate_clicked()
{
    operate.sort=2;
     operate.open();
    operate.displayTransactions(ui->finance_table);
     operate.close();
}


void finance::on_sortmontant_clicked()
{
    operate.sort=1;
     operate.open();
    operate.displayTransactions(ui->finance_table);
     operate.close();
}


void finance::on_doubleSpinBox_2_textChanged(const QString &arg1)
{

}


void finance::on_finance_montant_textChanged(const QString &arg1)
{


}


void finance::on_finance_montant_valueChanged(double arg1)
{
    operate.montant=ui->finance_montant->value();
    operate.open();
   operate.displayTransactions(ui->finance_table);
    operate.close();

}


int idTransaction;
void finance::on_modifier_finance_clicked()
{
    QModelIndexList selectedRows = ui->finance_table->selectionModel()->selectedRows();

        if (selectedRows.isEmpty()) {
            qDebug() << "No row selected.";
            return; // No row selected, exit the function
        }

        // Get the first selected row (you could also loop through if multiple selections are allowed)
        int row = selectedRows.first().row();
       int selectedRow = selectedRows[0].row();
        // Get the ID from the selected row
        QAbstractItemModel *model = ui->finance_table->model();
        idTransaction = model->data(model->index(row, 0)).toInt();
        QString montant = model->index(selectedRow, 5).data().toString();
            QString mode = model->index(selectedRow, 1).data().toString();
            QString type = model->index(selectedRow, 2).data().toString();
            QDate date = QDate::fromString(model->index(selectedRow, 4).data().toString(), "dd/MM/yyyy");
            QString cat = model->index(selectedRow, 3).data().toString();

            // Set the values to the respective input fields
            ui->montant_line->setValue(montant.toDouble());
            ui->mode_line->setCurrentText(mode);
            ui->type_line->setCurrentText(type);
            ui->date_line->setDate(date);
            ui->cate_line->setCurrentText(cat);
    ui->ajouter_line->hide();
    ui->update_finance->show();
    ui->annuler->show();

}


void finance::on_annuler_clicked()
{
    ui->ajouter_line->show();
    ui->annuler->hide();
    ui->update_finance->hide();
    QList<QComboBox*> lineEdits = ui->groupBox_finance->findChildren<QComboBox*>();
       for (QComboBox* lineEdit : lineEdits) {
           lineEdit->setCurrentIndex(0); // Effacer le texte
       }
}


void finance::on_update_finance_clicked()
{
    ui->ajouter_line->show();
    ui->annuler->hide();
    ui->update_finance->hide();
    QString montant=ui->montant_line->text();
    QString mode= ui->mode_line->currentText();
    QString type= ui->type_line->currentText();
    QDate date= ui->date_line->date();
    QString cat=ui->cate_line->currentText();
    QList<QComboBox*> lineEdits = ui->groupBox_finance->findChildren<QComboBox*>();
       for (QComboBox* lineEdit : lineEdits) {
           lineEdit->setCurrentIndex(0); // Effacer le texte
       }

       operate.open();
       if (operate.updateTransaction(idTransaction,mode, type, cat, date, montant.toFloat())) {
           qDebug() << "Transaction inserted successfully.";
       } else {
           qDebug() << "Failed to insert transaction.";
       }
      operate.displayTransactions(ui->finance_table);
       operate.close();
}


void finance::on_csv_clicked()
{
    operate.exportToCSV(ui->finance_table);
}


void finance::on_tabWidget_currentChanged(int index)
{
    operate.open();
    int counting=operate.countTransactions();
    double depense=operate.totaldepense();
    double revenue=operate.totalrevenue();
    ui->total_nb_finance->setText(QString::number(counting));
    double montant=revenue-depense;
    ui->total_finance->setText(QString::number(montant));
    ui->depense_text->setText(QString::number(depense));
    ui->revenue_text->setText(QString::number(revenue));

    operate.showhistorique(ui->historique);
    // Define pie chart data
    QString filePath="C:/Users/KNUser/Desktop/oracle inst/Atelier_Connexion (1)/Atelier_Connexion/data.txt";
    QList<QPair<QString, qreal>> data;
    data=operate.fetchAndSumTransactions(0);
    // operate.close();
    QFile file(filePath);

       // Open the file for writing
       if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
           QTextStream out(&file);

           // Loop through the QList and write each QPair to the file
           for (const QPair<QString, qreal>& pair : data) {
               out << pair.first << ", " << pair.second << "\n";
           }

           // Close the file
           file.close();
           qDebug() << "Data saved to file successfully.";
       } else {
           qDebug() << "Failed to open file for writing.";
       }
       QStringList arguments;
           arguments << "C:/Users/KNUser/Desktop/oracle inst/Atelier_Connexion (1)/Atelier_Connexion/chart.py";

           QProcess::execute("C:/Users/KNUser/AppData/Local/Programs/Python/Python311/python.exe", arguments);
           QPixmap pixmap("C:/Users/KNUser/Desktop/oracle inst/Atelier_Connexion (1)/Atelier_Connexion/pie_chart.png");

           // Scale the image to ensure it fits the QLabel while showing everything
           ui->stat->setPixmap(pixmap.scaled(ui->stat->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

           // Optionally, make sure the QLabel is centered
           ui->stat->setAlignment(Qt::AlignCenter);  // Centers the image in the QLabel


       // Draw the pie chart on the widget
        //PieChartWidget::drawPieChart(data, ui->stat);
           //QString filePath="C:/Users/KNUser/Desktop/oracle inst/Atelier_Connexion (1)/Atelier_Connexion/data.txt";
          // QList<QPair<QString, qreal>> data;
           data=operate.fetchAndSumTransactions(1);
            operate.close();


              // Open the file for writing
              if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                  QTextStream out(&file);

                  // Loop through the QList and write each QPair to the file
                  for (const QPair<QString, qreal>& pair : data) {
                      out << pair.first << ", " << pair.second << "\n";
                  }

                  // Close the file
                  file.close();
                  qDebug() << "Data saved to file successfully.";
              } else {
                  qDebug() << "Failed to open file for writing.";
              }
               arguments.clear();
                  arguments << "C:/Users/KNUser/Desktop/oracle inst/Atelier_Connexion (1)/Atelier_Connexion/chart.py";

                  QProcess::execute("C:/Users/KNUser/AppData/Local/Programs/Python/Python311/python.exe", arguments);
                  // pixmap("C:/Users/KNUser/Desktop/oracle inst/Atelier_Connexion (1)/Atelier_Connexion/pie_chart.png");

                  // Scale the image to ensure it fits the QLabel while showing everything
                  QPixmap pixmap1("C:/Users/KNUser/Desktop/oracle inst/Atelier_Connexion (1)/Atelier_Connexion/pie_chart.png");
                  ui->stat2->setPixmap(pixmap1.scaled(ui->stat->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

                  // Optionally, make sure the QLabel is centered
                  ui->stat2->setAlignment(Qt::AlignCenter);  // Centers the image in the QLabel


              // Draw the pie chart on the widget
               //PieChartWidget::drawPieChart(data, ui->stat);


}


void finance::on_comboBox_currentIndexChanged(int index)
{
    if(index==0)
        operate.sort=-1;
    else if(index==1)
        operate.sort=3;
    else if(index==2)
        operate.sort=4;
    operate.open();
   operate.displayTransactions(ui->finance_table);
    operate.close();
}

