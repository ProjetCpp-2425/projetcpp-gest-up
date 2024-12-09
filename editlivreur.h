#ifndef EDITLIVREUR_H
#define EDITLIVREUR_H

#include <QDialog>
#include <QDate>

namespace Ui {
class EditLivreur;
}

class EditLivreur : public QDialog
{
    Q_OBJECT

public:
    explicit EditLivreur(QWidget *parent = nullptr);
    ~EditLivreur();

    // Method to set livreur data in the dialog's fields
    void setLivreurData(const QString &idLivreur, const QString &nom, const QString &prenom,
                        const QString &telephone, const QString &adresse);

    // Method to update the livreur in the database
    bool updateLivreurInDatabase();
    void setLivreurData(const QString &idLivreur, const QString &nom, const QString &prenom,
                        const QString &telephone, const QString &adresse, const QString &totalLivraisons);
private slots:
    void onOkButtonClicked();
    void onCancelButtonClicked();
private:
    Ui::EditLivreur *ui;
    QString currentIdLivreur;  // Store the current ID to identify the livreur during the update
};

#endif // EDITLIVREUR_H
