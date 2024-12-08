#include "finance.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlDriver>
#include "PieChartWidget.h"
#include "ui_finance.h"
#include <QProcess>

#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
using namespace QXlsx;

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
     ui->finance_id->hide();
     ui->finance_date->hide();


}


int index2=0;

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
    if(index2==0){
    operate.montant=ui->finance_montant->value();
    operate.open();
   operate.displayTransactions(ui->finance_table);
    operate.close();}

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
    ui->comboBox_2->setCurrentIndex(0);
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
        operate.sort2=-1;
    else if(index==1)
        operate.sort2=3;
    else if(index==2)
        operate.sort2=4;
    operate.open();
    operate.displayTransactions(ui->finance_table);
    operate.close();
}


void finance::on_finance_montant2_currentIndexChanged(int index)
{
    if(index==0){
        ui->finance_montant->show();
        ui->finance_id->hide();
        ui->finance_date->hide();

        ui->finance_date->setDate(QDate::fromString("02/02/2024", "dd/MM/yyyy"));
        ui->finance_id->setValue(0);
    }
    else if(index==1){
        ui->finance_montant->hide();
        ui->finance_id->show();
        ui->finance_date->hide();
        ui->finance_date->setDate(QDate::fromString("02/02/2024", "dd/MM/yyyy"));
        ui->finance_montant->setValue(0);
    }
    else{
        ui->finance_montant->hide();
        ui->finance_id->hide();
        ui->finance_date->show();
        ui->finance_montant->setValue(0);
        ui->finance_id->setValue(0);
    }
    index2=index;
    operate.open();
    operate.displayTransactions(ui->finance_table);
    operate.close();
}


void finance::on_finance_id_valueChanged(int arg1)
{
    if(index2==1){
    operate.ID_sort=arg1;
    operate.open();
   operate.displayTransactions(ui->finance_table);
    operate.close();}
}


void finance::on_finance_date_userDateChanged(const QDate &date)
{
    if(index2==2){
    operate.date_sort=date;
    operate.open();
   operate.displayTransactions(ui->finance_table);
    operate.close();}
}



void finance::on_comboBox_2_currentTextChanged(const QString &arg1)
{
    operate.open();
    int counting=operate.countTransactions(arg1);
    double depense=operate.totaldepense(arg1);
    double revenue=operate.totalrevenue(arg1);
    ui->total_nb_finance->setText(QString::number(counting));
    double montant=revenue-depense;
    ui->total_finance->setText(QString::number(montant));
    ui->depense_text->setText(QString::number(depense));
    ui->revenue_text->setText(QString::number(revenue));
    operate.close();
}


void finance::on_pushButton_bilan_clicked()
{
    operate.open();
    QString annee=ui->comboBox_2->currentText();
    //qDebug()<<annee;
    int counting=operate.countTransactions(annee);
    double depense=operate.totaldepense(annee);
    double revenue=operate.totalrevenue(annee);
    double montant=revenue-depense;
    operate.close();
    //QString annee=ui->comboBox_2->currentText();
    int year;
    if(annee=="Année")
        return;
     year=annee.toInt();
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Excel File", "", "*.xlsx");
        if (fileName.isEmpty()) return;

        // Open the XLSX file for writing
        QXlsx::Document xlsx;

        // Add title above the table
        QXlsx::Format titleFormat;
        titleFormat.setFont(QFont("Arial", 18, QFont::Bold)); // Large, bold font
        titleFormat.setFontColor(QColor(255, 255, 255)); // White font color
        titleFormat.setPatternBackgroundColor(QColor(0, 102, 204)); // Blue background
        titleFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter); // Center align text

        QString title = QString("Bilan Financier of year %1").arg(year);
        xlsx.write("A1", title, titleFormat);

        // Merge cells from A1 to D1
        xlsx.mergeCells("A1:D1");

        // Add some space before starting the table data
        int currentRow = 2;

        // Write column headers with styling
        QStringList headers = {"Number of Transactions", "Total Expenses (Depense)", "Total Revenue", "Net Amount (Montant)"};
        QXlsx::Format headerFormat;
        headerFormat.setFont(QFont("Arial", 12, QFont::Bold)); // Set bold font
        headerFormat.setPatternBackgroundColor(QColor(255, 87, 34)); // Orange background for headers
        headerFormat.setFontColor(QColor(255, 255, 255)); // White text
        headerFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter); // Center align text

        for (int col = 0; col < headers.size(); ++col) {
            xlsx.write(currentRow, col + 1, headers[col], headerFormat);
            xlsx.setColumnWidth(col + 1, headers[col].length() + 10); // Set initial column width with padding
        }

        currentRow++; // Move to the next row for data

        // Write data values with styling
        QXlsx::Format dataFormat;
        dataFormat.setFont(QFont("Arial", 10)); // Regular font with size 10
        dataFormat.setPatternBackgroundColor(QColor(242, 242, 242)); // Light gray background for data cells
        dataFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter); // Center align text

        QList<QVariant> data = {counting, depense, revenue, montant};
        for (int col = 0; col < data.size(); ++col) {
            xlsx.write(currentRow, col + 1, data[col].toString(), dataFormat);
            xlsx.setColumnWidth(col + 1, qMax(headers[col].length(), data[col].toString().length()) + 10); // Adjust column width
        }

        // Save the file
        if (xlsx.saveAs(fileName)) {
            QMessageBox::information(nullptr, "Export", "Bilan Financier exported to Excel (XLSX) successfully!");
        } else {
            QMessageBox::warning(nullptr, "Export Error", "Cannot write to file!");
        }
}

