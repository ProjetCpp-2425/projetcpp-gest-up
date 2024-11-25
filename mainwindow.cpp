#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChartView>

#include <QString>
#include <QColor>
#include <limits>

#include <QTcpSocket>
#include <QTextStream>

#include <QPainter>
#include <QImage>
#include <QMessageBox>
#include "qrcodegen.hpp"




//NHEBBEEEkkkkkkkk


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) ,isShowingMessage(false){
    ui->setupUi(this);

    // Lier les widgets de l'interface
    idEdit = ui->idLineEdit;
    prenomEdit = ui->prenomLineEdit;
    nomEdit = ui->nomLineEdit;
    posteEdit = ui->posteLineEdit;
    // Configurer un validateur numérique pour le champ salaire
    salaireEdit = ui->salaireLineEdit;
    salaireEdit->setValidator(new QIntValidator(0, 999999, this));
    sexeComboBox = ui->sexeComboBox;
    tableWidget = ui->employeeTableWidget;
    validateButton = ui->validateButton;
    cancelButton = ui->cancelButton;
    deleteButton = ui->deleteButton;
    sortComboBox = ui->sortComboBox; // Combo box pour le tri
    isAddingEmployee = false;


    // Connecter les boutons
    connect(validateButton, &QPushButton::clicked, this, &MainWindow::onValidateButtonClicked);
    connect(cancelButton, &QPushButton::clicked, this, &MainWindow::onCancelButtonClicked);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteButtonClicked);
    connect(sortComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::onSortComboBoxChanged);

    // Configuration initiale du tableau
    tableWidget->setColumnCount(6); // Colonnes : ID, Nom, Prénom, Sexe, Poste, Salaire
    QStringList headers = {"ID", "Nom", "Prénom", "Sexe", "Poste", "Salaire"};
    tableWidget->setHorizontalHeaderLabels(headers);

    // Permettre la sélection d'une ligne complète
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    // Configurer le combo box de tri
    sortComboBox->addItems({"Nom", "Salaire", "Sexe"});

    // Lier les widgets
    searchComboBox = ui->searchComboBox;        // ComboBox pour choisir la colonne
    searchTextLineEdit = ui->searchTextLineEdit; // QLineEdit pour entrer le texte de recherche

    // Ajouter les options de recherche dans la combo box
    searchComboBox->addItems({"ID", "Nom", "Poste"});

    // Connecter le changement dans la combo box à la fonction de recherche
    connect(searchComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::onSearchComboBoxChanged);
    connect(searchTextLineEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchComboBoxChanged);  // Mettre à jour la recherche lors de la saisie

    connect(ui->pushButtonPdf, &QPushButton::clicked, this, &MainWindow::onPdfButtonClicked);

    connect(ui->statsButton, &QPushButton::clicked, this, &MainWindow::showGenderChart);
    connect(ui->statsButton, &QPushButton::clicked, this, &MainWindow::showSalaryExtremes);

    connect(ui->generateQrButton, &QPushButton::clicked, this, &MainWindow::onGenerateQrButtonClicked);




   // connect(tableWidget, &QTableWidget::cellChanged, this, &MainWindow::onCellModified);





}



MainWindow::~MainWindow() {
    delete ui;
}





void MainWindow::onValidateButtonClicked() {
    // Récupérer les données des champs
    QString id = idEdit->text();
    QString prenom = prenomEdit->text();
    QString nom = nomEdit->text();
    QString poste = posteEdit->text();
    int salaire = salaireEdit->text().toInt(); // Conversion en entier
    QString sexe = sexeComboBox->currentText();

    // Vérifier que les champs ne sont pas vides
    if (id.isEmpty() || prenom.isEmpty() || nom.isEmpty() || poste.isEmpty() ) {
        QMessageBox::warning(this, "Champs vides", "Veuillez remplir tous les champs !");
        return;
    }
    if (salaire <= 0) {
        QMessageBox::warning(this, "Salaire invalide", "Le salaire doit être supérieur à 0 !");
        isAddingEmployee = false; // Réinitialiser le drapeau

        return;
    }


    // Ajouter une nouvelle ligne au tableau
    int rowCount = tableWidget->rowCount();
    tableWidget->insertRow(rowCount);

    tableWidget->setItem(rowCount, 0, new QTableWidgetItem(id));
    tableWidget->setItem(rowCount, 1, new QTableWidgetItem(nom));
    tableWidget->setItem(rowCount, 2, new QTableWidgetItem(prenom));
    tableWidget->setItem(rowCount, 3, new QTableWidgetItem(sexe));
    tableWidget->setItem(rowCount, 4, new QTableWidgetItem(poste));
    tableWidget->setItem(rowCount, 5, new QTableWidgetItem(QString::number(salaire))); // Convertir l'entier en chaîne pour l'affichage

    // Ajouter le bouton "Supprimer" dans la dernière colonne
    QPushButton *deleteButton = new QPushButton("Supprimer");
    tableWidget->setCellWidget(rowCount, 6, deleteButton);

    // Connecter le bouton "Supprimer" à la fonction `onDeleteButtonClicked`
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteButtonClicked);

    // Obtenir la date et l'heure actuelles
    QString currentDateTime = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");

    // Afficher un QMessageBox avec la date et l'heure d'ajout
    QMessageBox::information(this, "Ajout d'employé",
        QString("L'employé a été ajouté avec succès le %1.").arg(currentDateTime));


        // Réinitialiser le drapeau après l'ajout
        isAddingEmployee = false;


        QSqlQuery query;
        query.prepare("INSERT INTO EMPLOYES (ID, PRENOM, NOM, SEXE, POSTE, SALAIRE) "
                      "VALUES (:id, :prenom, :nom, :sexe, :poste, :salaire)");
        query.bindValue(":id", id);
        query.bindValue(":prenom", prenom);
        query.bindValue(":nom", nom);
        query.bindValue(":sexe", sexe);
        query.bindValue(":poste", poste);
        query.bindValue(":salaire", salaire);

        if (!query.exec()) {
            QMessageBox::critical(this, "Erreur",
                "L'employé n'a pas pu être ajouté à la base de données :\n" + query.lastError().text());
            return;
        }





    // Réinitialiser les champs
    idEdit->clear();
    prenomEdit->clear();
    nomEdit->clear();
    posteEdit->clear();
    salaireEdit->clear();
    sexeComboBox->setCurrentIndex(0);




}

/*void MainWindow::onCellModified(int row, int column) {
    Q_UNUSED(column);

    // Obtenir la date et l'heure actuelles
    QString currentDateTime = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");

    // Obtenir le nom de l'employé modifié pour afficher dans le message
    QTableWidgetItem *nameItem = tableWidget->item(row, 1); // Colonne 1 pour "Nom"
    QString employeeName = nameItem ? nameItem->text() : "Inconnu";

    // Afficher un QMessageBox avec la date et l'heure de modification
    QMessageBox::information(this, "Modification d'employé",
        QString("Les données de l'employé '%1' ont été modifiées avec succès le %2.")
        .arg(employeeName)
        .arg(currentDateTime));
}
*/

void MainWindow::onCancelButtonClicked() {
    // Réinitialiser tous les champs
    idEdit->clear();
    prenomEdit->clear();
    nomEdit->clear();
    posteEdit->clear();
    salaireEdit->clear();
    sexeComboBox->setCurrentIndex(0);
}


void MainWindow::onDeleteButtonClicked() {
    // Obtenir l'index de la ligne sélectionnée
    QModelIndexList selectedRows = tableWidget->selectionModel()->selectedRows();

    if (selectedRows.isEmpty()) {
        QMessageBox::warning(this, "Suppression impossible", "Veuillez sélectionner une ligne à supprimer.");
        return;
    }

    // Obtenir l'ID de l'employé à partir de la première colonne de la ligne sélectionnée
    int row = selectedRows.first().row();
    QVariant id = tableWidget->item(row, 0)->text(); // Supposons que l'ID est dans la première colonne

    // Créer la requête SQL pour supprimer l'employé avec l'ID correspondant
    QSqlQuery query;
    query.prepare("DELETE FROM EMPLOYES WHERE ID = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur",
            "La suppression a échoué :\n" + query.lastError().text());
        return;
    }

    // Si la suppression est réussie, supprimer la ligne du tableau
    tableWidget->removeRow(row);

    QMessageBox::information(this, "Succès", "L'employé a été supprimé avec succès.");
}


void MainWindow::onSortComboBoxChanged() {
    int sortIndex = sortComboBox->currentIndex();
    QString orderByClause;

    // Déterminer la colonne et l'ordre de tri en fonction de la sélection dans le ComboBox
    switch (sortIndex) {
        case 0: // Tri par nom
            orderByClause = "ORDER BY NOM ASC";  // Trier par nom (ordre croissant)
            break;
        case 1: // Tri par salaire
            orderByClause = "ORDER BY SALAIRE ASC";  // Trier par salaire (ordre croissant)
            break;
        case 2: // Tri par sexe
            orderByClause = "ORDER BY SEXE ASC";  // Trier par sexe (ordre croissant)
            break;
        default:
            return;  // Si aucune option valide, ne rien faire
    }

    // Créer la requête SQL pour récupérer les données triées
    QSqlQuery query;
    query.prepare("SELECT ID, PRENOM, NOM, SEXE, POSTE, SALAIRE FROM EMPLOYES " + orderByClause);

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur",
            "Erreur lors de l'exécution de la requête de tri :\n" + query.lastError().text());
        return;
    }

    // Vider le tableau avant d'insérer les données triées
    tableWidget->clearContents();
    tableWidget->setRowCount(0);

    // Insérer les résultats triés dans le tableau
    while (query.next()) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);

        // Remplir chaque colonne avec les données de la base
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));  // ID
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));  // Prénom
        tableWidget->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));  // Nom
        tableWidget->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));  // Sexe
        tableWidget->setItem(row, 4, new QTableWidgetItem(query.value(4).toString()));  // Poste
        tableWidget->setItem(row, 5, new QTableWidgetItem(query.value(5).toString()));  // Salaire
    }
}

void MainWindow::onSearchComboBoxChanged() {
    QString searchText = ui->searchTextLineEdit->text();  // Texte entré par l'utilisateur
    int searchColumn = 0; // Par défaut, rechercher par ID

    // Déterminer la colonne en fonction de la sélection dans la combo box
    switch (ui->searchComboBox->currentIndex()) {
        case 0: // ID
            searchColumn = 0;
            break;
        case 1: // Nom
            searchColumn = 1;
            break;
        case 2: // Poste
            searchColumn = 4;
            break;
    }

    // Parcourir les lignes du tableau et masquer celles qui ne correspondent pas
    for (int i = 0; i < tableWidget->rowCount(); ++i) {
        QTableWidgetItem *item = tableWidget->item(i, searchColumn);
        if (item) {
            bool match = item->text().contains(searchText, Qt::CaseInsensitive);
            tableWidget->setRowHidden(i, !match); // Masquer les lignes qui ne correspondent pas
        }
    }
}

void MainWindow::onPdfButtonClicked() {
    // Demander le chemin de sauvegarde pour le fichier PDF
    QString filePath = QFileDialog::getSaveFileName(this, "Enregistrer le fichier PDF", "", "*.pdf");
    if (filePath.isEmpty()) {
        return; // Si l'utilisateur annule
    }

    // Créer un QPdfWriter
    QPdfWriter pdfWriter(filePath);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setResolution(300);

    // Préparer un QPainter pour dessiner dans le fichier PDF
    QPainter painter(&pdfWriter);

    // Définir les marges et la position initiale
    int margin = 50;
    int x = margin;
    int y = margin;
    int rowHeight = 40;  // Hauteur des lignes
    int columnWidth = 250;  // Largeur des colonnes
    int additionalSpacing = 200;  // Espacement supplémentaire entre les colonnes
    int signatureFieldWidth = 200;  // Largeur des champs pour l'heure de présence et de départ
    int signatureFieldHeight = 60;  // Hauteur plus grande pour les champs de signature

    // Dessiner le titre du tableau
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.drawText(x, y, "              Tableau des employés");
    y += 80;  // Espacement entre le titre et les données

    // Dessiner les en-têtes des colonnes (ID, Nom, Prénom, Heure de présence, Heure de départ)
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    QStringList headers = {"ID", "Nom", "Prénom", "Présence", "Départ"};
    for (int col = 0; col < headers.size(); ++col) {
        painter.drawText(x + col * (columnWidth + additionalSpacing), y, headers[col]);
    }
    y += rowHeight + 20;  // Espacement après l'en-tête

    // Dessiner les données du tableau (les champs sont vides pour les heures de présence et de départ)
    painter.setFont(QFont("Arial", 10));
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        for (int col = 0; col < 3; ++col) { // Seulement ID, Nom et Prénom
            QTableWidgetItem *item = tableWidget->item(row, col);
            if (item) {
                // Dessiner les cellules pour l'ID, Nom et Prénom
                QRect cellRect(x + col * (columnWidth + additionalSpacing), y, columnWidth, rowHeight);
                painter.drawText(cellRect, Qt::AlignLeft | Qt::AlignVCenter, item->text());
            }
        }

        // Dessiner les champs vides pour l'heure de présence
        QRect presenceRect(x + 3 * (columnWidth + additionalSpacing), y, signatureFieldWidth, signatureFieldHeight);
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(presenceRect);  // Dessiner un rectangle vide pour le champ
        painter.setPen(Qt::black);  // Retourner à la couleur noire pour le texte
        painter.drawText(presenceRect, Qt::AlignCenter, "__________");  // L'utilisateur peut signer ici

        // Augmenter considérablement l'espacement pour éloigner les deux cases
        int departFieldX = x + 3 * (columnWidth + additionalSpacing) + 300;  // Décalage plus important
        // Dessiner les champs vides pour l'heure de départ
        QRect departureRect(departFieldX, y, signatureFieldWidth, signatureFieldHeight);
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(departureRect);  // Dessiner un rectangle vide pour le champ
        painter.setPen(Qt::black);  // Retourner à la couleur noire pour le texte
        painter.drawText(departureRect, Qt::AlignCenter, "__________");  // L'utilisateur peut signer ici

        y += rowHeight + signatureFieldHeight + 60;  // Espacement plus grand entre les lignes pour les champs de signature
    }

    // Finir et sauvegarder le PDF
    painter.end();

    // Informer l'utilisateur
    QMessageBox::information(this, "Succès", "Le fichier PDF a été généré avec succès !");
}

void MainWindow::onStatsButtonClicked() {
    QString selectedStat = ui->statsComboBox->currentText();

    if (selectedStat == "Répartition des employés par sexe") {
        showGenderChart();
    } else if (selectedStat == "Employés avec les salaires les plus élevés / bas") {
        showSalaryExtremes();
    }
}

/*void MainWindow::showGenderDistribution() {
    if (isShowingMessage) return; // Empêcher l'appel multiple
    isShowingMessage = true;

    int maleCount = 0;
    int femaleCount = 0;

    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        QTableWidgetItem *genderItem = tableWidget->item(row, 3); // Colonne 3 pour "Sexe"
        if (genderItem) {
            QString gender = genderItem->text();
            if (gender == "H") {
                maleCount++;
            } else if (gender == "F") {
                femaleCount++;
            }
        }
    }

    QString message = QString("Répartition des employés par sexe :\n\n"
                              "Hommes : %1\n"
                              "Femmes : %2")
                              .arg(maleCount)
                              .arg(femaleCount);

    QMessageBox::information(this, "Statistique : Répartition par sexe", message);

    isShowingMessage = false; // Réinitialiser après l'affichage
}*/
void MainWindow::showGenderChart() {
    // Compter les employés par sexe
    int maleCount = 0;
    int femaleCount = 0;

    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        QTableWidgetItem *genderItem = tableWidget->item(row, 3); // Colonne 3 : Sexe
        if (genderItem) {
            QString gender = genderItem->text();
            if (gender == "H") {
                maleCount++;
            } else if (gender == "F") {
                femaleCount++;
            }
        }
    }

    // Créer une série pour le camembert
    QPieSeries *series = new QPieSeries();
    series->append("Hommes", maleCount);
    series->append("Femmes", femaleCount);

    // Ajouter des labels avec pourcentages
    for (auto slice : series->slices()) {
        slice->setLabel(QString("%1: %2 (%3%)")
            .arg(slice->label())
            .arg(static_cast<int>(slice->value()))
            .arg((slice->percentage() * 100), 0, 'f', 1));
    }

    // Créer un graphique
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Distribution des employés par sexe");
    chart->legend()->setAlignment(Qt::AlignBottom);

    // Afficher le graphique dans une fenêtre
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Créer une nouvelle fenêtre pour afficher le graphique
    QMainWindow *chartWindow = new QMainWindow(this);
    chartWindow->setCentralWidget(chartView);
    chartWindow->resize(600, 400);
    chartWindow->setWindowTitle("Statistique - Répartition par sexe");
    chartWindow->show();
}


void MainWindow::showSalaryExtremes() {
    if (isShowingMessage) return; // Empêcher l'appel multiple
    isShowingMessage = true;

    int maxSalary = INT_MIN;
    int minSalary = INT_MAX;
    QString maxEmployee;
    QString minEmployee;

    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        QTableWidgetItem *salaryItem = tableWidget->item(row, 5); // Colonne 5 pour "Salaire"
        QTableWidgetItem *nameItem = tableWidget->item(row, 1);   // Colonne 1 pour "Nom"
        if (salaryItem && nameItem) {
            int salary = salaryItem->text().toInt();
            QString name = nameItem->text();

            if (salary > maxSalary) {
                maxSalary = salary;
                maxEmployee = name;
            }
            if (salary < minSalary) {
                minSalary = salary;
                minEmployee = name;
            }
        }
    }

    QString message = QString("Employés avec les salaires les plus élevés / bas :\n\n"
                              "Salaire le plus élevé : %1 (€) - %2\n"
                              "Salaire le plus bas : %3 (€) - %4")
                              .arg(maxSalary)
                              .arg(maxEmployee)
                              .arg(minSalary)
                              .arg(minEmployee);

    QMessageBox::information(this, "Statistique : Salaires extrêmes", message);

    isShowingMessage = false; // Réinitialiser après l'affichage
}

void MainWindow::onGenerateQrButtonClicked() {
    // Construire le texte du QR Code avec les employés et leurs dates
    QString qrText;
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        QString id = tableWidget->item(row, 0)->text(); // ID
        QString nom = tableWidget->item(row, 1)->text(); // Nom
        QString prenom = tableWidget->item(row, 2)->text(); // Prénom
        QString dateAjout = QDateTime::currentDateTime().toString("dd/MM/yyyy");

        qrText += QString("ID: %1, Nom: %2, Prénom: %3, Date d'ajout: %4\n")
                      .arg(id)
                      .arg(nom)
                      .arg(prenom)
                      .arg(dateAjout);
    }

    // Vérifier si la liste est vide
    if (qrText.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "La liste des employés est vide !");
        return;
    }

    // Générer le code QR en utilisant `qrcodegen`
    qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(qrText.toUtf8().constData(), qrcodegen::QrCode::Ecc::MEDIUM);

    // Convertir le QR Code en image Qt
    int size = 300; // Taille du QR Code
    QImage qrImage(size, size, QImage::Format_RGB32);
    qrImage.fill(Qt::white);
    QPainter painter(&qrImage);

    int scale = size / qr.getSize();
    for (int y = 0; y < qr.getSize(); ++y) {
        for (int x = 0; x < qr.getSize(); ++x) {
            if (qr.getModule(x, y)) { // Si c'est un module noir
                painter.fillRect(x * scale, y * scale, scale, scale, Qt::black);
            }
        }
    }
    painter.end();

    // Afficher l'image QR dans une fenêtre séparée
    QLabel *qrLabel = new QLabel;
    qrLabel->setPixmap(QPixmap::fromImage(qrImage));
    qrLabel->setWindowTitle("Code QR - Liste des employés");
    qrLabel->setAlignment(Qt::AlignCenter);
    qrLabel->resize(size, size);
    qrLabel->show();

    qrImage.save("employes_qrcode.png");

}
