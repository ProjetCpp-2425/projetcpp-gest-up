#include "finance.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlDriver>
#include "ui_finance.h"

finance::finance(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::finance)
{
    ui->setupUi(this);

     operate= financeop("C:\\Users\\Mahdi\\OneDrive\\Documents\\finance\\finance.db");
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
    QString mode= ui->mode_line->text();
    QString type= ui->type_line->text();
    QDate date= ui->date_line->date();
    QString cat=ui->cate_line->text();
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
    QList<QLineEdit*> lineEdits = ui->groupBox_finance->findChildren<QLineEdit*>();
       for (QLineEdit* lineEdit : lineEdits) {
           lineEdit->clear(); // Effacer le texte
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
            ui->mode_line->setText(mode);
            ui->type_line->setText(type);
            ui->date_line->setDate(date);
            qDebug()<<date;
            ui->cate_line->setText(cat);
    ui->ajouter_line->hide();
    ui->update_finance->show();
    ui->annuler->show();

}


void finance::on_annuler_clicked()
{
    ui->ajouter_line->show();
    ui->annuler->hide();
    ui->update_finance->hide();
    QList<QLineEdit*> lineEdits = ui->groupBox_finance->findChildren<QLineEdit*>();
       for (QLineEdit* lineEdit : lineEdits) {
           lineEdit->clear(); // Effacer le texte
       }
}


void finance::on_update_finance_clicked()
{
    ui->ajouter_line->show();
    ui->annuler->hide();
    ui->update_finance->hide();
    QString montant=ui->montant_line->text();
    QString mode= ui->mode_line->text();
    QString type= ui->type_line->text();
    QDate date= ui->date_line->date();
    QString cat=ui->cate_line->text();
    QList<QLineEdit*> lineEdits = ui->groupBox_finance->findChildren<QLineEdit*>();
       for (QLineEdit* lineEdit : lineEdits) {
           lineEdit->clear(); // Effacer le texte
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
    ui->total_nb_finance->setText(QString::number(counting));
    double montant=operate.totalMontant();
    ui->total_finance->setText(QString::number(montant));
    operate.close();
}

