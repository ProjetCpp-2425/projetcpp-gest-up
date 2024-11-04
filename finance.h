#ifndef FINANCE_H
#define FINANCE_H

#include <QMainWindow>
#include<QSqlDatabase>
#include<QSqlError>
#include "financeop.h"

QT_BEGIN_NAMESPACE
namespace Ui { class finance; }
QT_END_NAMESPACE

class finance : public QMainWindow
{
    Q_OBJECT

public:
    finance(QWidget *parent = nullptr);
    ~finance();

private slots:
    void on_ajouter_line_clicked();


    void on_supprimer_clicked();

    void on_pushButton_6_clicked();

    void on_sortdate_clicked();

    void on_sortmontant_clicked();

    void on_doubleSpinBox_2_textChanged(const QString &arg1);

    void on_finance_montant_textChanged(const QString &arg1);

    void on_finance_montant_valueChanged(double arg1);

    void on_modifier_finance_clicked();

    void on_annuler_clicked();

    void on_update_finance_clicked();

    void on_csv_clicked();

    void on_tabWidget_currentChanged(int index);

private:
    Ui::finance *ui;
    financeop operate;
};
#endif // FINANCE_H
