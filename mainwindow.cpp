#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addlivreur.h"
#include "editlivreur.h"
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QPdfWriter>
#include <QPainter>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtCharts/QPieSlice>

// Constructor
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug() << "Available database drivers: " << QSqlDatabase::drivers();

    // Initialize the serial port
    setupSerialPort();

    // Load livreurs into the table view on startup
    loadLivreurData();

    // Connect Add, Remove, and Edit buttons to their respective slots
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::on_addButton_clicked);
    connect(ui->removeButton, &QPushButton::clicked, this, &MainWindow::on_removeButton_clicked);
    connect(ui->editButton, &QPushButton::clicked, this, &MainWindow::on_editButton_clicked);
    connect(ui->sortButton, &QPushButton::clicked, this, &MainWindow::onSortButtonClicked);
    connect(ui->generatePdfButton, &QPushButton::clicked, this, &MainWindow::generatePdf);
    connect(ui->restorebutton, &QPushButton::clicked, this, &MainWindow::onRestoreClicked);
    connect(ui->backupbutton, &QPushButton::clicked, this, &MainWindow::onBackupClicked);
    connect(ui->imageButton, &QPushButton::clicked, this, &MainWindow::on_imageButton_clicked);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::onTableSelectionChanged);
    connect(ui->statisbutton, &QPushButton::clicked, this, &MainWindow::on_statistique_clicked);
    connect(ui->rechercheButton, &QPushButton::clicked, this, &MainWindow::onrechercheclicked);
    connect(ui->loadbutton, &QPushButton::clicked, this, &MainWindow::loadLivreurData);
}

// Destructor
MainWindow::~MainWindow()
{
    delete ui;
}

// Serial port setup
void MainWindow::setupSerialPort() {
    serialPort = new QSerialPort(this);

    // Set serial port parameters
    serialPort->setPortName("COM13");  // Adjust port name as needed
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    // Open the serial port
    if (!serialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "Failed to open serial port!";
        return;
    }

    // Connect the readyRead signal to a slot to handle incoming data
    connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readKeypadData);
}

// Reading data from the keypad (serial port)
void MainWindow::readKeypadData() {
    QByteArray data = serialPort->readAll();
    enteredCode.append(data);

    // Clean up the entered code by trimming whitespace and newlines
    enteredCode = enteredCode.trimmed();

    // Update the display while entering the code
    ui->codeDisplayLabel->setText(enteredCode);
}


// Verifying the code entered through serial port
void MainWindow::on_verifyButton_clicked() {
    QString correctCode = "123456"; // Set the correct code to compare with

    // Clean the entered code by trimming spaces and newlines
    enteredCode = enteredCode.trimmed();

    if (enteredCode == correctCode) {
        qDebug() << "Code verified successfully!";
        ui->statusLabel->setText("Code verified successfully!");
    } else {
        qDebug() << "Incorrect code!";
        ui->statusLabel->setText("Incorrect code. Try again.");
    }

    enteredCode.clear();  // Clear entered code after verification
}

// Slot for Add Button click
void MainWindow::on_addButton_clicked() {
    AddLivreur addLivreurDialog(this);
    if (addLivreurDialog.exec() == QDialog::Accepted) {
        addLivreurDialog.addLivreurToDatabase();
        loadLivreurData();
    }
}

// Slot for Remove Button click
void MainWindow::on_removeButton_clicked() {
    QModelIndexList selectedRows = ui->tableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QMessageBox::warning(this, "Error", "No livreur selected for removal.");
        return;
    }

    int row = selectedRows[0].row();
    QString idLivreur = ui->tableView->model()->index(row, 0).data().toString();
    QSqlQuery query;
    query.prepare("DELETE FROM livreur WHERE idLivreur = :idLivreur");
    query.bindValue(":idLivreur", idLivreur);

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Livreur removed successfully.");
        loadLivreurData();
    } else {
        QMessageBox::warning(this, "Error", "Failed to remove livreur: " + query.lastError().text());
    }
}

// Slot for Edit Button click
void MainWindow::on_editButton_clicked() {
    QModelIndexList selectedRows = ui->tableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QMessageBox::warning(this, "Error", "No livreur selected for editing.");
        return;
    }

    int row = selectedRows[0].row();
    QString idLivreur = ui->tableView->model()->index(row, 0).data().toString();
    QSqlQuery query;
    query.prepare("SELECT * FROM livreur WHERE idLivreur = :idLivreur");
    query.bindValue(":idLivreur", idLivreur);

    if (query.exec() && query.next()) {
        EditLivreur editLivreurDialog(this);
        editLivreurDialog.setLivreurData(query.value("idLivreur").toString(),
                                         query.value("nom").toString(),
                                         query.value("prenom").toString(),
                                         query.value("telephone").toString(),
                                         query.value("adresse").toString(),
                                         query.value("total_livraisons").toString());

        if (editLivreurDialog.exec() == QDialog::Accepted) {
            editLivreurDialog.updateLivreurInDatabase();
            loadLivreurData();
        }
    } else {
        QMessageBox::warning(this, "Error", "Failed to retrieve livreur data: " + query.lastError().text());
    }
}

// Function to load livreur data into the table view
void MainWindow::loadLivreurData() {
    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery("SELECT idLivreur, nom, prenom, telephone, adresse, total_livraisons FROM livreur");
    if (model->lastError().isValid()) {
        qDebug() << "Error loading data from database: " << model->lastError().text();
        return;
    }
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
}



// Function to load livreur image based on selection
void MainWindow::loadLivreurImage(const QString &idLivreur) {
    QSqlQuery query;
    query.prepare("SELECT image_path FROM livreur WHERE idLivreur = :idLivreur");
    query.bindValue(":idLivreur", idLivreur);

    if (query.exec() && query.next()) {
        QString imagePath = query.value("image_path").toString();
        if (!imagePath.isEmpty() && QFile::exists(imagePath)) {
            QPixmap pixmap(imagePath);
            if (!pixmap.isNull()) {
                ui->imageLabel->setPixmap(pixmap.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio));
            } else {
                ui->imageLabel->clear();
                QMessageBox::warning(this, "Error", "Failed to load image.");
            }
        } else {
            ui->imageLabel->clear();
        }
    } else {
        ui->imageLabel->clear();
    }
}

// Slot to handle image button click
void MainWindow::on_imageButton_clicked()
{
    // Ensure a livreur is selected
    QModelIndexList selectedRows = ui->tableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QMessageBox::warning(this, "Error", "No livreur selected. Please select a livreur first.");
        return;
    }

    // Get the selected livreur's ID
    QString idLivreur = ui->tableView->model()->index(selectedRows[0].row(), 0).data().toString();

    // Open a file dialog to select an image
    QString imagePath = QFileDialog::getOpenFileName(this, "Select Livreur Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (imagePath.isEmpty()) {
        QMessageBox::warning(this, "Error", "No image selected.");
        return;
    }

    // Update the database with the image path
    QSqlQuery query;
    query.prepare("UPDATE livreur SET image_path = :imagePath WHERE idLivreur = :idLivreur");
    query.bindValue(":imagePath", imagePath);
    query.bindValue(":idLivreur", idLivreur);

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Image uploaded successfully!");
        qDebug() << "Image path updated in database: " << imagePath;

        // Refresh the image display
        loadLivreurImage(idLivreur);
    } else {
        QMessageBox::warning(this, "Error", "Failed to update the database: " + query.lastError().text());
    }
}

// Slot for on_statistique_clicked
void MainWindow::on_statistique_clicked()
{
    // Set the title for the statistics window
    this->setWindowTitle("Livraisons Statistics");

    // Pie series for livraison distribution
    QPieSeries *livraisonSeries = new QPieSeries();

    // Ensure database connection
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Database not open!";
        QMessageBox::critical(this, "Database Error", "Failed to open database connection.");
        return;
    }

    // Query to count total livraisons grouped by livreur
    QSqlQuery query(db);
    query.prepare("SELECT nom, total_livraisons FROM livreur WHERE total_livraisons > 0");

    if (!query.exec()) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        QMessageBox::critical(this, "Query Error", "Could not execute query.");
        return;
    }

    // Add data to the pie series
    while (query.next()) {
        QString livreurName = query.value(0).toString();
        int livraisonCount = query.value(1).toInt();
        livraisonSeries->append(livreurName + " (" + QString::number(livraisonCount) + ")", livraisonCount);
    }

    // Customize the pie chart
    livraisonSeries->setLabelsVisible();
    livraisonSeries->setLabelsPosition(QPieSlice::LabelOutside);
    for (auto slice : livraisonSeries->slices()) {
        slice->setLabelColor(Qt::black);
        slice->setBorderColor(Qt::blue);
        slice->setBorderWidth(2);
    }

    // Create the chart
    QChart *chart = new QChart();
    chart->addSeries(livraisonSeries);
    chart->setTitle("Livraisons Distribution");
    chart->setAnimationOptions(QChart::AllAnimations);

    // Create the chart view
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Display the chart in a new window
    QWidget *statsWindow = new QWidget();
    statsWindow->setWindowTitle("Livraisons Statistics");
    QVBoxLayout *layout = new QVBoxLayout(statsWindow);
    layout->addWidget(chartView);

    statsWindow->resize(800, 600);
    statsWindow->show();
}

// Slot for onRestoreClicked (restore functionality)
void MainWindow::onRestoreClicked()
{
    // Ask the user to select the CSV file for restoring
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open CSV File"), "", tr("CSV Files (*.csv);;All Files (*)"));

    if (fileName.isEmpty()) {
        return;  // If no file is selected, do nothing
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Unable to open the file for reading.");
        return;
    }

    // Remove all existing data in the 'livreur' table
    QSqlQuery clearQuery;
    if (!clearQuery.exec("DELETE FROM livreur")) {
        QMessageBox::warning(this, "Error", "Failed to clear existing data: " + clearQuery.lastError().text());
        file.close();
        return;
    }

    // Read CSV file and insert data into the database
    QTextStream in(&file);
    QSqlQuery insertQuery;
    int lineNumber = 0;

    while (!in.atEnd()) {
        QString line = in.readLine();
        lineNumber++;

        QStringList fields = line.split(',');

        if (fields.size() != 6) {  // Ensure the correct number of columns
            QMessageBox::warning(this, "Error", "Invalid data format on line " + QString::number(lineNumber));
            continue;
        }

        insertQuery.prepare("INSERT INTO livreur (idLivreur, nom, prenom, telephone, adresse, total_livraisons) "
                            "VALUES (:idLivreur, :nom, :prenom, :telephone, :adresse, :totalLivraisons)");

        insertQuery.bindValue(":idLivreur", fields[0].trimmed());
        insertQuery.bindValue(":nom", fields[1].trimmed());
        insertQuery.bindValue(":prenom", fields[2].trimmed());
        insertQuery.bindValue(":telephone", fields[3].trimmed());
        insertQuery.bindValue(":adresse", fields[4].trimmed());
        insertQuery.bindValue(":totalLivraisons", fields[5].trimmed().toInt());

        if (!insertQuery.exec()) {
            QMessageBox::warning(this, "Error", "Failed to insert data on line " + QString::number(lineNumber) + ": " + insertQuery.lastError().text());
        }
    }

    file.close();

    // Reload data to reflect the restored information
    loadLivreurData();

    QMessageBox::information(this, "Restore Complete", "The data has been successfully restored from the CSV file.");
}

// Slot for onBackupClicked (backup functionality)
void MainWindow::onBackupClicked()
{
    // Ask the user where to save the CSV file
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save CSV File"), "", tr("CSV Files (*.csv);;All Files (*)"));

    if (fileName.isEmpty()) {
        return;  // If no file is selected, do nothing
    }

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Unable to open the file for writing.");
        return;
    }

    QTextStream out(&file);

    // Query all data from the 'livreur' table
    QSqlQuery query("SELECT idLivreur, nom, prenom, telephone, adresse, total_livraisons FROM livreur");

    while (query.next()) {
        QString idLivreur = query.value("idLivreur").toString();
        QString nom = query.value("nom").toString();
        QString prenom = query.value("prenom").toString();
        QString telephone = query.value("telephone").toString();
        QString adresse = query.value("adresse").toString();
        QString totalLivraisons = query.value("total_livraisons").toString();

        // Write the data to the CSV file
        out << idLivreur << "," << nom << "," << prenom << "," << telephone << "," << adresse << "," << totalLivraisons << "\n";
    }

    file.close();

    QMessageBox::information(this, "Backup Complete", "The data has been successfully backed up to the CSV file.");
}

// Sorting function for livreurs
void MainWindow::onSortButtonClicked()
{
    QString selectedOption = ui->comboBox->currentText();
    QString orderByClause;

    // Determine the sorting column and order based on the selected option
    if (selectedOption == "Nom") {
        orderByClause = "nom ASC";
    } else if (selectedOption == "Prenom") {
        orderByClause = "prenom ASC";
    } else if (selectedOption == "Telephone") {
        orderByClause = "telephone ASC";
    }

    // Create a new query with ORDER BY to sort
    QSqlQueryModel *model = new QSqlQueryModel(this);
    QString query = QString("SELECT idLivreur, nom, prenom, telephone, adresse, total_livraisons FROM livreur ORDER BY %1").arg(orderByClause);
    model->setQuery(query);

    // Check for query errors
    if (model->lastError().isValid()) {
        qDebug() << "Error sorting data: " << model->lastError().text();
        return;
    }

    // Set the sorted model to the table view
    ui->tableView->setModel(model);
}


// Slot for onTableSelectionChanged (table selection change functionality)
void MainWindow::onTableSelectionChanged()
{
    QModelIndexList selectedRows = ui->tableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        ui->imageLabel->clear();
        return;
    }

    QString idLivreur = ui->tableView->model()->index(selectedRows[0].row(), 0).data().toString();

    // Load the image for the selected livreur
    loadLivreurImage(idLivreur);
}
// Function to generate PDF with livreur data
void MainWindow::generatePdf()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save PDF"), "", tr("PDF Files (*.pdf);;All Files (*)"));

    if (fileName.isEmpty()) {
        return;
    }

    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setResolution(300);

    QPainter painter(&pdfWriter);
    QFont font("Arial", 12);
    painter.setFont(font);

    painter.drawText(100, 100, "Livreur Data");

    int yOffset = 150;
    int i = 1;
    QSqlQuery query("SELECT idLivreur, nom, prenom, telephone, adresse, total_livraisons FROM livreur");

    while (query.next()) {
        painter.drawText(50, yOffset, "Livreur " + QString::number(i) + ":");
        i++;
        yOffset += 60;
        painter.drawText(100, yOffset, "ID: " + query.value("idLivreur").toString());
        yOffset += 60;
        painter.drawText(100, yOffset, "Nom: " + query.value("nom").toString());
        yOffset += 60;
        painter.drawText(100, yOffset, "Prenom: " + query.value("prenom").toString());
        yOffset += 60;
        painter.drawText(100, yOffset, "Telephone: " + query.value("telephone").toString());
        yOffset += 60;
        painter.drawText(100, yOffset, "Adresse: " + query.value("adresse").toString());
        yOffset += 60;
        painter.drawText(100, yOffset, "Total Livraisons: " + query.value("total_livraisons").toString());
        yOffset += 100;
    }

    painter.end();
    QMessageBox::information(this, tr("PDF Generated"), tr("The livreur data has been saved as a PDF."));
}
void MainWindow::onrechercheclicked()
{
    // Retrieve the search text from the line edit
    QString searchText = ui->lineEdit_recherche->text();

    // Create a new QSqlQueryModel
    QSqlQueryModel *model = new QSqlQueryModel(this);
    QString query = "SELECT idLivreur, nom, prenom, telephone, adresse, total_livraisons "
                    "FROM livreur "
                    "WHERE idLivreur LIKE :searchText "
                    "OR nom LIKE :searchText "
                    "OR prenom LIKE :searchText "
                    "OR telephone LIKE :searchText "
                    "OR adresse LIKE :searchText";

    QSqlQuery sqlQuery;
    sqlQuery.prepare(query);
    sqlQuery.bindValue(":searchText", "%" + searchText + "%");
    if (sqlQuery.exec()) {
        model->setQuery(sqlQuery);
        if (model->rowCount() == 0) {
            QMessageBox::information(this, "No Results", "No livreurs found matching your search.");
        }
    } else {
        QMessageBox::warning(this, "Error", "Failed to execute search query: " + sqlQuery.lastError().text());
    }
    ui->tableView->setModel(model);
}

