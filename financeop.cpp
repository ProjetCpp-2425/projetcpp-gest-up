#include "financeop.h"
#include <QSqlRecord>

#include <QTableView>
#include <QStandardItemModel>
financeop::financeop() {
    // Empty constructor initializes the database connection object but doesn't set it up
    // You can add any default initialization logic here if needed
}
financeop::financeop(const QString& dbPath) {
     db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("test_bd");//inserer le nom de la source de données
    db.setUserName("projetcpp2a30");//inserer nom de l'utilisateur
    db.setPassword("esprit24");//inserer mot de passe de cet utilisateur

}

bool financeop::open() {
    if (!db.open()) {
        qDebug() << "Error: Unable to open the database:" << db.lastError().text();
        return false;
    }
    return true;
}

void financeop::close() {
    db.close();
}

bool financeop::insertTransaction( const QString& modePaiement, const QString& type,
                                   const QString& categorieTransaction, const QDate& date, float montant) {
    QSqlQuery query;
    query.prepare( "INSERT INTO \"transaction\" (\"mode_payement\", \"type\", \"categorie-transaction\", \"date\", \"montant\") "
                   "VALUES (:modePaiement, :type, :categorieTransaction, TO_DATE(:date, 'DD/MM/YYYY'), :montant)");
   // query.bindValue(":idTransaction", idTransaction);
    query.bindValue(":modePaiement", modePaiement);
    query.bindValue(":type", type);
    query.bindValue(":categorieTransaction", categorieTransaction);
    query.bindValue(":date", date.toString("dd/MM/yyyy"));
    query.bindValue(":montant", montant);
    qDebug()<<query.lastQuery();
    if (!query.exec()) {
        qDebug() << "Error: Unable to insert transaction:" << query.lastError();
        return false;
    }
    return true;
}
void financeop::displayTransactions(QTableView *tableView) {
    // Create a model to hold the data


    // Prepare the SQL query to select all records from the transaction table
    QString sql("SELECT \"id-transaction\" AS \"ID\", \"mode_payement\" AS \"Mode de payement\", \"type\" AS \"Type\", "
                "\"categorie-transaction\" AS \"Categorie de Transaction\", TO_CHAR(\"date\", 'DD/MM/YYYY') AS \"Date de transaction\", "
                "\"montant\" AS \"Montant\" FROM \"transaction\"");
    if(montant>=0.1)
        sql+=" WHERE \"montant\"="+QString::number(montant)+" ";
    if (sort == 1) {
            sql += " ORDER BY \"montant\" ASC"; // Sort by montant in ascending order
        } else if (sort == 2) {
            sql += " ORDER BY \"date\" ASC"; // Sort by date in ascending order
        }

    QSqlQuery query(db);
    // Execute the query
    if (!query.exec(sql)) {
        qDebug() << "Error: Unable to execute query:" << query.lastError().text();
        return; // Exit if the query fails
    }
    QList<QList<QVariant>> data; // List of rows, each row is a list of columns

    while (query.next()) {
        QList<QVariant> row;
        for (int i = 0; i < query.record().count(); ++i) {
            row.append(query.value(i));
        }
        data.append(row);
    }


    // Set the model's query
    QStandardItemModel *model = new QStandardItemModel(data.size(), query.record().count());

    for (int row = 0; row < data.size(); ++row) {
        for (int col = 0; col < data[row].size(); ++col) {
            QStandardItem *item = new QStandardItem(data[row][col].toString());
            model->setItem(row, col, item);
        }
    }

    // Set headers if necessary
    QStringList headers;
    for (int i = 0; i < query.record().count(); ++i) {
        headers << query.record().fieldName(i);
    }
    model->setHorizontalHeaderLabels(headers);

    // Create your QTableView and set the model
    tableView->setModel(model);
    tableView->resizeColumnsToContents(); // Optional: resize columns to fit content
    tableView->show();
}
bool financeop::deleteTransaction(int idTransaction) {
    QSqlQuery query(db);

    // Prepare the delete statement
    QString txt=("DELETE FROM \"transaction\" WHERE \"id-transaction\" = "+QString::number(idTransaction));


    // Execute the query
    if (!query.exec(txt)) {
        qDebug() << "Error deleting transaction:" << query.lastError().text();
        return false; // Return false if there was an error
    }

    return true; // Return true if deletion was successful
}


// Function to update a transaction by ID
bool financeop::updateTransaction(int idTransaction, const QString &modePaiement, const QString &type,
                                  const QString &categorieTransaction, const QDate &date, float montant) {
    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
        return false;
    }

    QSqlQuery query;
    QString updateQuery = QString("UPDATE \"transaction\" "
                                  "SET \"mode_payement\" = '%1', "
                                  "\"type\" = '%2', "
                                  "\"categorie-transaction\" = '%3', "
                                  "\"date\" = TO_DATE('%4', 'DD/MM/YYYY'), "
                                  "\"montant\" = %5 "
                                  "WHERE \"id-transaction\" = '%6'")
                           .arg(modePaiement)
                           .arg(type)
                           .arg(categorieTransaction)
                           .arg(date.toString("dd/MM/yyyy"))
                           .arg(montant)
                           .arg(idTransaction);

    // Now execute the query

    if (!query.exec(updateQuery)) {
        qDebug() << "Update failed:" << query.lastError();
        return false;
    }

    // Execute the query


    qDebug() << "Transaction updated successfully!";
    return true;
}
void financeop::exportToCSV(QTableView *tableView) {
    // Check if the table view or model is valid
    if (!tableView || !tableView->model()) {
        QMessageBox::warning(nullptr, "Export Error", "The table view or its model is not set up correctly.");
        return;
    }

    // Prompt for file save location
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save CSV File", "", "*.csv");
    if (fileName.isEmpty()) return;

    // Ensure the file has a .csv extension
    if (!fileName.endsWith(".csv")) {
        fileName += ".csv";
    }

    // Open file for writing
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Export Error", "Cannot write to file!");
        return;
    }

    QTextStream stream(&file);
    QAbstractItemModel *model = tableView->model();

    // Write column headers
    for (int col = 0; col < model->columnCount(); ++col) {
        stream << model->headerData(col, Qt::Horizontal).toString();
        if (col < model->columnCount() - 1) {
            stream << ",";
        }
    }
    stream << "\n";

    // Write each row of data
    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 0; col < model->columnCount(); ++col) {
            QVariant data = model->data(model->index(row, col));

            // Check if the data is a date and format it
            if (data.type() == QVariant::Date || data.type() == QVariant::DateTime) {
                stream << data.toDateTime().toString("yyyy-MM-dd");  // Format as needed
            } else {
                stream << data.toString();
            }

            if (col < model->columnCount() - 1) {
                stream << ",";
            }
        }
        stream << "\n";
    }

    file.close();
    QMessageBox::information(nullptr, "Export", "Data exported to CSV successfully!");
}
int financeop::countTransactions() {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM \"transaction\"");

    if (!query.exec()) {
        qDebug() << "Error counting rows in transaction table:" << query.lastError().text();
        return -1;  // Return -1 if there was an error
    }

    if (query.next()) {
        return query.value(0).toInt();  // Return the count from the first column
    } else {
        return 0;  // Return 0 if the query has no results
    }
}
double financeop::totalMontant() {
    QSqlQuery query;
    query.prepare("SELECT SUM(\"montant\") FROM \"transaction\"");

    if (!query.exec()) {
        qDebug() << "Error calculating total montant in transaction table:" << query.lastError().text();
        return -1;  // Return -1 if there was an error
    }

    if (query.next()) {
        return query.value(0).toDouble();  // Return the total sum from the first column
    } else {
        return 0;  // Return 0 if there are no rows
    }
}
