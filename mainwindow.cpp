#include "mainwindow.h"
#include "commande.h"
#include "ui_mainwindow.h"
#include<QMessageBox>
#include <QApplication>
#include<QIntValidator>
#include <QFileDialog>
#include <QPdfWriter>
#include <QtDebug>
#include <QPainter>
#include <QDesktopServices>
#include <QTextDocument>
#include <QDate>
#include "qrcode.h"
#include <QtSql>
#include <QtCharts>
#include <QtGui>
#include<QtCharts>
#include<QChartView>
#include<QPieSeries>
#include<QPieSlice>
#include <QChart>
#include <QDateTime>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include "EMAILSENDER.h"
#include <QRegularExpression>
#include <QSerialPortInfo>
#include <QSerialPort>
#include "EMAILSENDER.h"

using namespace QrCodegen;
QString days="";
using namespace std;




Smtp* smtp = nullptr;



#include <QRegularExpression>  // N'oubliez pas d'inclure l'en-tête approprié pour QRegularExpression

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), serial(new seriallink(this)) ,emailSender(new EMAILSENDER(this))
{
    ui->setupUi(this);

    // Champ date et heure (DateTimeEdit)
    ui->dateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");  // Format souhaité pour l'affichage

    ui->Total->setValidator(new QIntValidator(0, 9999999, this));
    ui->leid->setValidator(new QIntValidator(0, 9999999, this));

    // Validator pour la description et le nom
    QRegularExpression regex("[A-Za-z0-9\\s.,!?'-]+");
    QValidator *validator = new QRegularExpressionValidator(regex, this);
    ui->Description->setValidator(validator);
    ui->Nom->setValidator(validator);

    // Création de l'expression régulière pour l'état avec QRegularExpression
    QRegularExpression etatRegExp("^(accepted|cancelled|pending|delivered)$", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionValidator *etatValidator = new QRegularExpressionValidator(etatRegExp, this);  // Utilisation du bon validator ici
    ui->Etat->setValidator(etatValidator);

    ui->tableView_2->setModel(c.afficher());

    m_currentPage = 0;
    connect(serial, &seriallink::dataReceived, this, &MainWindow::processData);
    QString portName = "COM"; // Remplacez par le bon port COM
        int baudRate = 9600;

        // Tenter de se connecter
        if (serial->connectToArduino(portName, baudRate)) {
            qDebug() << "Connexion réussie à l'Arduino sur le port" << portName;
        } else {
            qDebug() << "Échec de la connexion à l'Arduino sur le port" << portName;
        }
        connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::on_connectButton_clicked);
        connect(ui->disconnectButton, &QPushButton::clicked, this, &MainWindow::on_disconnectButton_clicked);
        connect(ui->sendCommandButton, &QPushButton::clicked, this, &MainWindow::on_sendCommandButton_clicked);
}


MainWindow::~MainWindow()
{
    delete ui;
}
QString MainWindow::filterBadWords(const QString &text)
{
    QString filteredText = text;
    QStringList badWords = {"fuck", "khraa", "chleka"};

    for (const QString &badWord : badWords)
    {
        QString stars(badWord.length(), '*');
        filteredText.replace(badWord, stars, Qt::CaseInsensitive);
    }

    return filteredText;
}
void MainWindow::onEmailStatusReceived(const QString &status)
{
    // Afficher un message d'erreur si l'envoi échoue
    if (status.contains("Erreur")) {
        QMessageBox::critical(this, "Erreur", status);
    } else {
        QMessageBox::information(this, "Succès", status);
    }
}


void MainWindow::on_pb_ajouter_clicked()
{
    // Récupération des valeurs des champs
    int id = ui->leid->text().toInt();
    QString dateDePeremption = ui->dateTimeEdit->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    double total = ui->Total->text().toDouble();
    QString typeDePaiement = ui->Typdepaiment->text();
    QString nom = ui->Nom->text();
    QString description = ui->Description->text();
    QString etat = ui->Etat->text();  // Récupération de l'état de la commande

    // Vérification de l'état
    QStringList validStates = {"accepted", "cancelled", "pending", "delivered"};
    if (!validStates.contains(etat.toLower())) {
        QMessageBox::warning(this, "Erreur", "L'état doit être l'un des suivants : accepted, cancelled, pending, delivered.");
        return;
    }

    description = filterBadWords(description);

    // Vérification des champs obligatoires
    if (id <= 0 || total <= 0 || nom.isEmpty() || typeDePaiement.isEmpty() || description.isEmpty() || etat.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs obligatoires avec des valeurs valides.");
        return;
    }

    if (typeDePaiement != "cash" && typeDePaiement != "card") {
        QMessageBox::warning(this, "Erreur", "Le type de paiement doit être 'cash' ou 'card'.");
        return;
    }

    // Affichage des données pour débogage
    qDebug() << "ID: " << id
             << ", Date de péremption: " << dateDePeremption
             << ", Total: " << total
             << ", Type de paiement: " << typeDePaiement
             << ", Nom: " << nom
             << ", Description: " << description
             << ", Etat: " << etat;

    // Création de l'objet Commande
    Commande c(id, dateDePeremption, total, typeDePaiement, nom, description, etat);

    // Tentative d'ajout
    if (c.ajouter()) {
        // Message de succès
        QMessageBox::information(this, "Succès", "Commande ajoutée avec succès !");

        // Actualisation du tableau
        ui->tableView_2->setModel(c.afficher());

        // Envoi de l'email après l'ajout réussi
        if (smtp == nullptr) {
            smtp = new Smtp("asmaabdellatif11@gmail.com", "bts2004army", "smtp.gmail.com", 465);
        }

        // Envoi de l'email
        QString subject = "Nouvelle Commande Ajoutée";
        QString body = "Une nouvelle commande a été ajoutée avec les détails suivants:\n\n"
                       "ID: " + QString::number(id) + "\n" +
                       "Nom: " + nom + "\n" +
                       "Total: " + QString::number(total) + "\n" +
                       "Etat: " + etat + "\n" +
                       "Description: " + description;

        // Remplacez "destinataire@example.com" par l'adresse email à laquelle vous souhaitez envoyer l'email
        smtp->sendMail("asmaabdellatif11@gmail.com", "jiminmylife20@gmail.com", subject, body);
        connect(smtp, &Smtp::status, this, &MainWindow::onEmailStatusReceived);

    } else {
        // Message d'erreur en cas d'échec
        QMessageBox::critical(this, "Erreur", "L'ajout a échoué. Vérifiez les données ou contactez l'administrateur.");
    }

}


void MainWindow::on_pb_supprimer_clicked()
{

        Commande c1;
        c1.setIdCommande(ui->id_supp->text().toInt());

        bool existe = c1.rechercher(c1.getIdCommande()); // Rechercher la cmd

        if (existe) {
            // Demander une confirmation avant la suppression
            QMessageBox::StandardButton confirmation = QMessageBox::question(this, "Confirmation", "Voulez-vous vraiment supprimer cette commande ?", QMessageBox::Yes | QMessageBox::No);
            if (confirmation == QMessageBox::Yes) {
                bool test = c1.supprimer(c1.getIdCommande()); // Supprimer la cmd
                if (test) {
                    QMessageBox::information(this, "Succès", "Suppression avec succès.");
                    ui->tableView_2->setModel(c.afficher());
                } else {
                    QMessageBox::warning(this, "Echec", "Echec de suppression.");
                }
            }
        } else {
            QMessageBox::warning(this, "Erreur", "Le véhicule n'existe pas."); // Alerter l'utilisateur que le véhicule n'a pas été trouvé
        }


}
void MainWindow::on_pb_modifier_clicked()
{
    int id = ui->leid->text().toInt();  // Récupérer l'ID de l'utilisateur
    QString dateDePeremption = ui->dateTimeEdit->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    double total = ui->Total->text().toDouble();
    QString typeDePaiement = ui->Typdepaiment->text();
    QString nom = ui->Nom->text();
    QString description = ui->Description->text();
    QString etat = ui->Etat->text();  // Récupérer la valeur de l'état

    // Vérifier que l'identifiant est valide
    if (id <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer une valeur numérique valide pour l'identifiant.");
        return;
    }

    // Vérifier que les champs obligatoires sont remplis
    if (total <= 0 || nom.isEmpty() || typeDePaiement.isEmpty() || description.isEmpty() || etat.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs obligatoires.");
        return;
    }

    // Vérifier que l'état est valide
    if (etat != "accepted" && etat != "cancelled" && etat != "pending" && etat != "delivered") {
        QMessageBox::warning(this, "Erreur", "L'état doit être l'un des suivants : accepted, cancelled, pending, delivered.");
        return;
    }

    // Créer un objet Commande avec les données saisies, y compris l'état
    Commande c(id, dateDePeremption, total, typeDePaiement, nom, description, etat);

    // Confirmer la modification avec l'utilisateur
    QMessageBox::StandardButton confirmUpdate = QMessageBox::question(this, "Confirmation",
        "Êtes-vous sûr de vouloir modifier cet enregistrement ?",
        QMessageBox::Yes | QMessageBox::No);

    if (confirmUpdate == QMessageBox::No) {
        return; // L'utilisateur a annulé la modification
    }

    // Appeler la méthode modifier de l'objet Commande
    bool success = c.modifier(id);

    // Si la modification est réussie, rafraîchir le tableau
    if (success) {
        ui->tableView_2->setModel(c.afficher());
        QMessageBox::information(this, "Succès", "Modification effectuée avec succès.");
    } else {
        QMessageBox::critical(this, "Erreur", "La modification n'a pas pu être effectuée.");
    }
}

void MainWindow::on_pushButton_rechercher_clicked()
{
    // Récupérer la valeur de recherche et supprimer les espaces inutiles
    QString valeur = ui->lerechercher->text().trimmed();

    // Vérifier si la valeur de recherche est vide
    if (valeur.isEmpty())
    {
        // Afficher un message d'erreur
        QMessageBox::warning(this, "Recherche", "Veuillez saisir une valeur de recherche.");
        // Quitter la fonction sans exécuter la recherche
        return;
    }

    // Appeler la fonction de recherche dans la classe Piste et récupérer le modèle de résultats
    QSqlQueryModel *model = c.rechercher(valeur);

    // Vérifier si aucun résultat n'a été trouvé
    if (model->rowCount() == 0)
    {
        // Afficher un message d'information
        QMessageBox::information(this, "Recherche", "AQucun résultat trouvé.");
    }
    else
    {
        // Créer un message avec le nombre de résultats trouvés
        QString message = QString("%1 résultat(s) trouvé(s).").arg(model->rowCount());
        // Afficher un message d'information avec le nombre de résultats trouvés
        QMessageBox::information(this, "Recherche", message);
        // Définir le modèle de résultats dans la table
        ui->tableView_2->setModel(model);
        // Trier les résultats par ordre croissant de l'id
        ui->tableView_2->sortByColumn(0, Qt::AscendingOrder);
    }
}

void MainWindow::on_pushButton_chercher_nom_clicked()
{
    // Récupérer la valeur de recherche pour le nom et supprimer les espaces inutiles
    QString valeur = ui->lerechercher->text().trimmed();

    // Vérifier si la valeur de recherche est vide
    if (valeur.isEmpty())
    {
        // Afficher un message d'erreur
        QMessageBox::warning(this, "Recherche", "Veuillez saisir un nom à rechercher.");
        // Quitter la fonction sans exécuter la recherche
        return;
    }

    // Appeler la fonction de recherche dans la classe Piste (ou équivalent) et récupérer le modèle de résultats
    QSqlQueryModel *model = c.rechercherParNom(valeur);

    // Vérifier si aucun résultat n'a été trouvé
    if (model->rowCount() == 0)
    {
        // Afficher un message d'information
        QMessageBox::information(this, "Recherche", "Aucun résultat trouvé.");
    }
    else
    {
        // Créer un message avec le nombre de résultats trouvés
        QString message = QString("%1 résultat(s) trouvé(s).").arg(model->rowCount());
        // Afficher un message d'information avec le nombre de résultats trouvés
        QMessageBox::information(this, "Recherche", message);
        // Définir le modèle de résultats dans la table
        ui->tableView_2->setModel(model);
        // Trier les résultats par ordre croissant de l'id (ou tout autre critère)
        ui->tableView_2->sortByColumn(0, Qt::AscendingOrder);
    }
}


void MainWindow::on_pushButton_PDF_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     "/home",
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
    qDebug() << dir;
    QPdfWriter pdf(dir + "/PdfList.pdf");
    QPainter painter(&pdf);
    int i = 4000;

    painter.drawPixmap(QRect(100, 100, 2000, 2000), QPixmap("C:/Users/Admin/Desktop/msi.png"));
    painter.setPen(Qt::red);
    painter.setFont(QFont("Time New Roman", 20));  // Taille de police réduite
    painter.drawText(3000, 1450, "Liste Des Commandes");
    painter.setPen(Qt::black);
    painter.setFont(QFont("Time New Roman", 8));  // Police réduite pour les données

    // Dessin du tableau avec taille réduite
    painter.drawRect(100, 100, 8000, 2500);  // Réduire la largeur de la bordure du tableau
    painter.drawRect(100, 3000, 8000, 500);  // Réduire la hauteur de la première ligne
    painter.drawText(300, 3300, "ID");
   // painter.drawText(1300, 3300, "Date de Peremption");
    //painter.drawText(2700, 3300, "Description");
    //painter.drawText(4000, 3300, "Total");
    //painter.drawText(5000, 3300, "Type de Paiment");
    //painter.drawText(6300, 3300, "Nom");
    painter.drawText(1300, 3300, "Etat");

    painter.drawRect(100, 3000, 8000, 10700);  // Réduire la largeur du tableau

    QTextDocument previewDoc;
    QString pdflist = QDate::currentDate().toString("'data_'MM_dd_yyyy'.txt'");

    QTextCursor cursor(&previewDoc);

    QSqlQuery query;
    query.prepare("SELECT * FROM commande");
    query.exec();
    while (query.next())
    {
        // Récupérer les valeurs
        QString id = query.value(0).toString();
        QString datePeremption = query.value(1).toString();
        QString description = query.value(2).toString();
        QString total = query.value(3).toString();
        QString typePaiement = query.value(4).toString();
        QString nom = query.value(5).toString();
        QString etat = query.value(6).toString();

        // Vérification si l'état est accepté ou correspond à des valeurs spécifiques
        if (etat == "accepted")  // Vous pouvez ajuster cette condition pour d'autres états
        {
            // Affichage dans le PDF avec des ajustements pour réduire la taille
            painter.drawText(300, i, id);
          //  painter.drawText(1300, i, datePeremption);
            //painter.drawText(2700, i, description);
            //painter.drawText(4000, i, total);
            //painter.drawText(5000, i, typePaiement);
            //painter.drawText(6300, i, nom);
            painter.drawText(1300, i, etat);

            i = i + 400;  // Décalage vertical réduit pour compacter le tableau
        }
    }

    int reponse = QMessageBox::question(this, "Générer PDF", " PDF Enregistré ! Voulez Vous Afficher Le PDF ?",
                                        QMessageBox::Yes | QMessageBox::No);
    if (reponse == QMessageBox::Yes)
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(dir + "/PdfList.pdf"));
        painter.end();
    }
    else
    {
        painter.end();
    }
}




void MainWindow::on_triID_clicked()
{
    ui->tableView_2->setModel(c.triID());

}

void MainWindow::on_nom_az_clicked()
{
     ui->tableView_2->setModel(c.triNom());
}

void MainWindow::on_nom_desc_clicked()
{
    ui->tableView_2->setModel(c.triNomDesc());
}

void MainWindow::on_ID_desc_clicked()
{
    ui->tableView_2->setModel(c.triIDdesc());
}
void MainWindow::on_qrcodegen_2_clicked()
{
    // Récupérer l'index de la ligne sélectionnée dans le tableau
    int rowIndex = ui->tableView_2->currentIndex().row();

    // Récupérer la valeur de la colonne IDCOMMANDE (première colonne)
    QVariant idValue = ui->tableView_2->model()->data(ui->tableView_2->model()->index(rowIndex, 0));
    int id = idValue.toInt();

    // Créer une requête SQL pour récupérer les données de la commande en fonction de l'ID
    QSqlQuery qry;
    qry.prepare("SELECT * FROM commande WHERE IDCOMMANDE=:id");
    qry.bindValue(":id", id);

    // Exécuter la requête
    if (!qry.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête : " << qry.lastError().text();
        return;
    }

    QString qrText = "";

    // Vérifier si la requête retourne des résultats
    if (qry.next()) {
        // Récupération des données de la commande
        QString dateDePeremption = qry.value(1).toString(); // Supposons que la date de péremption est à la deuxième colonne
        double total = qry.value(2).toDouble(); // Supposons que le total est à la troisième colonne
        QString typeDePaiement = qry.value(3).toString(); // Supposons que le type de paiement est à la quatrième colonne
        QString nom = qry.value(4).toString(); // Nom du client
        QString description = qry.value(5).toString(); // Description de la commande

        // Afficher les données récupérées pour débogage
        qDebug() << "ID: " << id
                 << ", Date de péremption: " << dateDePeremption
                 << ", Total: " << total
                 << ", Type de paiement: " << typeDePaiement
                 << ", Nom: " << nom
                 << ", Description: " << description;

        // Créer le texte du QR code avec les nouveaux attributs
        qrText = "ID: " + QString::number(id) +
                 ", Date de péremption: " + dateDePeremption +
                 ", Total: " + QString::number(total, 'f', 2) + // Affiche le total avec 2 décimales
                 ", Type de paiement: " + typeDePaiement +
                 ", Nom: " + nom +
                 ", Description: " + description;
    }

    // Vérifier si qrText a bien été généré
    if (qrText.isEmpty()) {
        qDebug() << "Aucune donnée trouvée pour le QR code";
        return;
    }

    // Générer le QR code à partir du texte
    QrCode qr = QrCode::encodeText(qrText.toUtf8().constData(), QrCode::Ecc::HIGH);

    // Créer une image pour afficher le QR code
    QImage image(qr.getSize(), qr.getSize(), QImage::Format_RGB888);

    // Remplir l'image avec les pixels du QR code
    for (int y = 0; y < qr.getSize(); y++) {
        for (int x = 0; x < qr.getSize(); x++) {
            int color = qr.getModule(x, y);  // 0 pour blanc, 1 pour noir

            // Si le pixel est noir, on l'affiche en noir, sinon en blanc
            if (color == 0)
                image.setPixel(x, y, qRgb(254, 254, 254)); // Blanc
            else
                image.setPixel(x, y, qRgb(0, 0, 0)); // Noir
        }
    }

    // Ajuster la taille de l'image pour qu'elle soit bien lisible (ajustée à 600x600)
    image = image.scaled(300, 300);

    // Afficher l'image du QR code dans le widget
    ui->qrcode->setPixmap(QPixmap::fromImage(image));
}


void MainWindow::on_pushButton_2_clicked()
{
    // Création d'un modèle pour exécuter la requête SQL
    QSqlQueryModel *model = new QSqlQueryModel();

    // Requête SQL pour compter les commandes dans les différentes plages de total
    model->setQuery("SELECT "
                    "COUNT(CASE WHEN TOTAL BETWEEN 0 AND 50 THEN 1 END) AS \"0_50\", "
                    "COUNT(CASE WHEN TOTAL BETWEEN 51 AND 200 THEN 1 END) AS \"51_200\", "
                    "COUNT(CASE WHEN TOTAL > 200 THEN 1 END) AS \"200_plus\" "
                    "FROM commande");

    // Vérification si la requête a échoué
    if (model->lastError().isValid()) {
        qDebug() << "Erreur dans la requête SQL : " << model->lastError().text();
        delete model;
        return;
    }

    // Récupération des résultats de la requête
    int count_0_50 = model->data(model->index(0, 0)).toInt();
    int count_51_200 = model->data(model->index(0, 1)).toInt();
    int count_200_plus = model->data(model->index(0, 2)).toInt();

    // Calcul du total
    int total = count_0_50 + count_51_200 + count_200_plus;

    // Création d'une série pour le graphique
    QPieSeries *series = new QPieSeries();

    // Ajout des tranches avec des pourcentages
    QPieSlice *slice_0_50 = series->append("0-50", count_0_50);
    QPieSlice *slice_51_200 = series->append("51-200", count_51_200);
    QPieSlice *slice_200_plus = series->append("200+", count_200_plus);

    if (total > 0) {
        slice_0_50->setLabel(QString("0-50: %1%").arg((count_0_50 * 100.0) / total, 0, 'f', 2));
        slice_51_200->setLabel(QString("51-200: %1%").arg((count_51_200 * 100.0) / total, 0, 'f', 2));
        slice_200_plus->setLabel(QString("200+: %1%").arg((count_200_plus * 100.0) / total, 0, 'f', 2));
    }

    // Configuration des tranches
    slice_0_50->setBrush(Qt::blue);
    slice_51_200->setBrush(QColor("orange"));
    slice_200_plus->setBrush(Qt::green);

    slice_0_50->setLabelPosition(QPieSlice::LabelInsideHorizontal);
    slice_51_200->setLabelPosition(QPieSlice::LabelInsideHorizontal);
    slice_200_plus->setLabelPosition(QPieSlice::LabelInsideHorizontal);

    // Création du graphique
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des commandes par total");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Ajustement des marges et de la taille des labels
    chart->setMargins(QMargins(10, 10, 10, 10));
    chart->legend()->setAlignment(Qt::AlignBottom);

    // Création d'une vue pour le graphique
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // --- Insérer le graphique dans le frame ---
    if (!ui->frame->layout()) {
        ui->frame->setLayout(new QVBoxLayout());
    }

    // Supprimer tout ancien widget dans le frame
    QLayoutItem *child;
    while ((child = ui->frame->layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    // Ajouter le graphique au layout du frame
    ui->frame->layout()->addWidget(chartView);

    // Redimensionner pour agrandir le graphique
    chartView->resize(1000, 1000);
    ui->frame->layout()->setContentsMargins(1, 1, 1, 1);
}



void MainWindow::addPagination()
{


int currentPage = ui->tableView_2->currentIndex().row() / m_pageSize;
int rowCount = ui->tableView_2->model()->rowCount();

int startRow = currentPage * m_pageSize;
int endRow = qMin(startRow + m_pageSize, rowCount) - 1;

// Show only the rows for the current page
// QAbstractItemModel *model = ui->tabmedi_3->model();
for (int row = 0; row < rowCount; row++) {
if (row >= startRow && row <= endRow) {
ui->tableView_2->setRowHidden(row, false);
} else {
ui->tableView_2->setRowHidden(row, true);
}
}


/*
// Disable "Previous Month" and "Next Month" buttons if necessary
ui->moisprec->setEnabled(currentPage > 0);
ui->moisuivant->setEnabled(currentPage < totalPages - 1);
*/



}

void MainWindow::on_moisuivant_clicked()
{ QModelIndex currentIndex = ui->tableView_2->currentIndex();
int nextIndexRow = currentIndex.row() + m_pageSize;
int rowCount = ui->tableView_2->model()->rowCount();
if (nextIndexRow >= rowCount) {
nextIndexRow = rowCount - 1;
}

m_currentPage = nextIndexRow / m_pageSize;
addPagination();

QModelIndex nextIndex = ui->tableView_2->model()->index(nextIndexRow, currentIndex.column());
ui->tableView_2->setCurrentIndex(nextIndex);
}



void MainWindow::on_moisprec_clicked()
{
m_currentPage--;
if (m_currentPage < 0) {
m_currentPage = 0;
}
addPagination();

// Aller à la page précédente
int prevPageStartRow = m_currentPage * m_pageSize;
int rowCount = ui->tableView_2->model()->rowCount();
if (prevPageStartRow < rowCount) {
QModelIndex prevIndex = ui->tableView_2->model()->index(prevPageStartRow, 0);
ui->tableView_2->setCurrentIndex(prevIndex);
}

}

void MainWindow::on_connectButton_clicked()
{
    // Tentative de connexion à l'Arduino avec le port et le taux de transmission
    QString portName = "COM3"; // Remplacez par le port correct pour votre système
    int baudRate = 9600;       // Taux de transmission

    if (serial->connectToArduino(portName, baudRate)) {
        qDebug() << "Connexion réussie à l'Arduino.";
    } else {
        qDebug() << "Échec de la connexion à l'Arduino.";
    }
}

void MainWindow::on_disconnectButton_clicked()
{
    // Déconnexion de l'Arduino
    serial->disconnectFromArduino();
}

void MainWindow::on_sendCommandButton_clicked()
{
    // Envoi d'une commande à l'Arduino
    QString command = "COMMAND";  // Remplacez par la commande souhaitée
    serial->sendCommand(command);
}

void MainWindow::processData(const QString &data)
{
    // Traitement des données reçues de l'Arduino
    qDebug() << "Données reçues de l'Arduino : " << data;

    // Vous pouvez traiter les données ici, par exemple les afficher dans un QLabel
    ui->receivedDataLabel->setText(data);  // Si vous avez un QLabel nommé receivedDataLabel
}
void MainWindow::listAvailablePorts() {
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        qDebug() << "Port:" << info.portName()
        << ", Description:" << info.description()
        << ", Manufacturer:" << info.manufacturer();
    }
}
void MainWindow::on_sendButton_clicked()
{
    // Récupérer les informations saisies dans les champs
    QString recipient = ui->lineEditRecipient->text();   // Email du destinataire
    QString subject = ui->lineEditSubject->text();       // Sujet de l'email
    QString message = ui->textEditMessage->toPlainText(); // Contenu de l'email

    // Appeler la méthode pour envoyer l'email
    emailSender->sendEmail(recipient, subject, message);
}
