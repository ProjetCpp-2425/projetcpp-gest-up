#include "addlivreur.h"
#include "ui_addlivreur.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QRegularExpression>

AddLivreur::AddLivreur(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddLivreur)
{
    ui->setupUi(this);

    // Validator for 8-digit ID and phone numbers
    QRegularExpression idRegEx("\\d{8}");
    QValidator *idValidator = new QRegularExpressionValidator(idRegEx, this);
    ui->idLivreurLineEdit->setValidator(idValidator);
    ui->phoneLineEdit->setValidator(idValidator);

    // Validator for Total Livraisons (must be a non-negative number)
    QRegularExpression totalLivraisonRegEx("\\d+"); // Only positive integers allowed
    QValidator *totalLivraisonValidator = new QRegularExpressionValidator(totalLivraisonRegEx, this);
    ui->TotalLineEdit->setValidator(totalLivraisonValidator);
}

AddLivreur::~AddLivreur()
{
    delete ui;
}

// Methods to get data from the dialog inputs
QString AddLivreur::getIdLivreur() const { return ui->idLivreurLineEdit->text(); }
QString AddLivreur::getNom() const { return ui->nomLineEdit->text(); }
QString AddLivreur::getPrenom() const { return ui->prenomLineEdit->text(); }
QString AddLivreur::getTelephone() const { return ui->phoneLineEdit->text(); }
QString AddLivreur::getAdresse() const { return ui->adresseLineEdit->text(); }
int AddLivreur::getTotalLivraisons() const { return ui->TotalLineEdit->text().toInt(); } // Convert string to integer

// Function to check uniqueness of fields like ID and Phone
bool AddLivreur::isUnique(const QString &field, const QString &value) {
    QSqlQuery query;
    query.prepare(QString("SELECT COUNT(*) FROM LIVREUR WHERE %1 = :value").arg(field));
    query.bindValue(":value", value);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() == 0;  // Return true if count is 0 (unique)
    }
    return false;
}

// Function to add livreur to database with validation
bool AddLivreur::addLivreurToDatabase() {
    // Validate ID Livreur
    if (getIdLivreur().length() != 8 || !getIdLivreur().toUInt()) {
        QMessageBox::warning(this, "Validation Error", "ID Livreur must be an 8-digit number.");
        return false;
    }

    // Check if ID Livreur is unique
    if (!isUnique("IDLIVREUR", getIdLivreur())) {
        QMessageBox::warning(this, "Validation Error", "ID Livreur must be unique.");
        return false;
    }

    // Validate phone number length (must be 8 digits)
    if (getTelephone().length() != 8 || !getTelephone().toUInt()) {
        QMessageBox::warning(this, "Validation Error", "Phone number must be an 8-digit number.");
        return false;
    }

    // Check if Phone is unique
    if (!isUnique("TELEPHONE", getTelephone())) {
        QMessageBox::warning(this, "Validation Error", "Phone number must be unique.");
        return false;
    }

    // Validate Total Livraisons (must be non-negative)
    if (getTotalLivraisons() < 0) {
        QMessageBox::warning(this, "Validation Error", "Total Livraisons must be a non-negative number.");
        return false;
    }

    // If all validation checks pass, attempt to insert into the database
    QSqlQuery query;
    query.prepare("INSERT INTO LIVREUR (IDLIVREUR, NOM, PRENOM, TELEPHONE, ADRESSE, TOTAL_LIVRAISONS) "
                  "VALUES (:idlivreur, :nom, :prenom, :telephone, :adresse, :total_livraisons)");
    query.bindValue(":idlivreur", getIdLivreur());
    query.bindValue(":nom", getNom());
    query.bindValue(":prenom", getPrenom());
    query.bindValue(":telephone", getTelephone());
    query.bindValue(":adresse", getAdresse());
    query.bindValue(":total_livraisons", getTotalLivraisons());

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Livreur added successfully.");
        return true;
    } else {
        QMessageBox::warning(this, "Error", "Failed to add livreur: " + query.lastError().text());
        return false;
    }
}
