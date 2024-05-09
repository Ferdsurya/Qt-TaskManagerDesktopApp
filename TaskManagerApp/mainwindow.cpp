#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // Mengisi QTableWidget dengan data awal
    ui->tableWidget->setColumnCount(6);

    // Array
    int columnWidths[] = {250, 300, 200, 200, 130, 150};

    // Mengatur lebar untuk setiap kolom menggunakan loop for
    for (int c = 0; c < 6; ++c) {
        ui->tableWidget->setColumnWidth(c, columnWidths[c]);
        ui->tableWidget_2->setColumnWidth(c, columnWidths[c]);
    }

    // Mengatur tinggi untuk setiap baris
    int rowHeight = 50;  // Ganti dengan tinggi yang diinginkan
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        ui->tableWidget->setRowHeight(row, rowHeight);
        ui->tableWidget_2->setRowHeight(row, rowHeight);
    }

    QHeaderView* header = ui->tableWidget->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Interactive); // ini kalau mau netapin lebar kolom fix ada ukurannya/ ngga ikut strech
    header->setSectionResizeMode(1, QHeaderView::Stretch); //Kalau mau menyesuaikan lebar kolom strech sesuai layar pakai Strech
    header->setSectionResizeMode(2, QHeaderView::Interactive);
    header->setSectionResizeMode(3, QHeaderView::Stretch);
    header->setSectionResizeMode(4, QHeaderView::Interactive);
    header->setSectionResizeMode(5, QHeaderView::Interactive);

    QHeaderView* header2 = ui->tableWidget_2->horizontalHeader();
    header2->setSectionResizeMode(0, QHeaderView::Interactive); // ini kalau mau netapin lebar kolom fix ada ukurannya/ ngga ikut strech
    header2->setSectionResizeMode(1, QHeaderView::Stretch); //Kalau mau menyesuaikan lebar kolom strech sesuai layar pakai Strech
    header2->setSectionResizeMode(2, QHeaderView::Interactive);
    header2->setSectionResizeMode(3, QHeaderView::Stretch);
    header2->setSectionResizeMode(4, QHeaderView::Interactive);
    header2->setSectionResizeMode(5, QHeaderView::Interactive);

    applyTableWidgetStyle(ui->tableWidget);
    applyTableWidgetStyle(ui->tableWidget_2);


    // Menambahkan QComboBox pada kolom ketiga dan kelima untuk setiap baris
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        addComboBoxToRow(row, 2, QStringList() << "Belum Dimulai" << "Dalam Progress" << "Selesai");
        addComboBoxToRow(row, 4, QStringList() << "Low" << "Medium" << "High");
        addCheckBoxToRow(row, 5);
        addTimeDatePickersToRow(row, 3, ui->tableWidget);
    }

    //BARIS KODE SORTING MULAI------------------------------------------------------------------------------------------------------------------------
    // Aktifkan fungsi sorting pada QTableWidget
      ui->tableWidget->setSortingEnabled(true);
      ui->tableWidget_2->setSortingEnabled(true);

      connect(ui->tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::onTableWidgetDeadlineHeaderClicked);
      connect(ui->tableWidget_2->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::onTableWidgetDeadlineHeaderClicked);

      connect(ui->tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::onTableWidgetPrioritasHeaderClicked);
      connect(ui->tableWidget_2->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::onTableWidgetPrioritasHeaderClicked);

      connect(ui->tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::onTableWidgetStatusHeaderClicked);
      connect(ui->tableWidget_2->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::onTableWidgetStatusHeaderClicked);

      //SEARCH BAR--------------------------------------------------------------------------------------------------
      connect(ui->searchBar, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);




// Inisialisasi database
           QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
           db.setDatabaseName("D:/MyBoyFriend/task manager 1-6/db/QtTableTaskManager.db");

           if (!db.open()) {
               qDebug() << "Koneksi database gagal";
               // Handle kesalahan koneksi database
           } else {
               qDebug() << "Database connected successfully!";
           }


//           // Create the table if it does not exist
           QSqlQuery query;
           query.exec("CREATE TABLE IF NOT EXISTS active_task ("
                      "nama TEXT, "
                      "deskripsi TEXT, "
                      "status TEXT, "
                      "deadline TEXT, "  // Mengubah tipe data menjadi INTEGER
                      "prioritas TEXT"
                      ")");

           query.exec("CREATE TABLE IF NOT EXISTS completed_task ("
                      "nama TEXT, "
                      "deskripsi TEXT, "
                      "status TEXT, "
                      "deadline TEXT, "  // Mengubah tipe data menjadi INTEGER
                      "prioritas TEXT"
                      ")");

           loadDataActiveTask();
           loadDataCompletedTask();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_dashboardButton_clicked()
{
    hide();
}

//SEARCH BAR-------------------------------------------------------------------------------
void MainWindow::onSearchTextChanged(const QString &text)
{
    QWidget *currentWidget = ui->tabWd->currentWidget();

    if (currentWidget) {
        QTableWidget *currentTableWidget = currentWidget->findChild<QTableWidget*>();

        if (currentTableWidget) {
            QString searchText = text.toLower();

            int rowCount = currentTableWidget->rowCount();
            int columnCount = currentTableWidget->columnCount();

            if (searchText.isEmpty()) {
                // Jika teks pencarian kosong, tampilkan semua baris
                for (int row = 0; row < rowCount; ++row) {
                    currentTableWidget->setRowHidden(row, false);
                }
                currentTableWidget->setVisible(true);
            } else {
                // Jika ada teks pencarian, terapkan logika pencarian
                bool anyRowVisible = false;

                for (int row = 0; row < rowCount; ++row) {
                    bool rowVisible = false;
                    for (int col = 0; col < columnCount; ++col) {
                        QTableWidgetItem *item = currentTableWidget->item(row, col);
                        if (item && item->text().toLower().contains(searchText)) {
                            rowVisible = true;
                            anyRowVisible = true;
                            break;
                        }
                    }
                    currentTableWidget->setRowHidden(row, !rowVisible);
                }

                // Set the visibility of the entire table based on whether any row is visible
                currentTableWidget->setVisible(anyRowVisible);
            }
        }
    }
}

//FUNGSI UTAMA-------------------------------------------------------------------------------

void MainWindow::on_addrowButton_clicked()
{

    // Mendapatkan referensi ke QTableWidget Anda
       QTableWidget *tableWidget = ui->tableWidget;

       // Menambahkan satu baris baru
       int rowCount = tableWidget->rowCount();
       tableWidget->insertRow(rowCount);

       // Mengisi baris baru dengan item-item default atau kosong
       for (int col = 0; col < tableWidget->columnCount(); ++col) {
           QTableWidgetItem *newItem = new QTableWidgetItem("");
           tableWidget->setItem(rowCount, col, newItem);
       }

    addComboBoxToRow(rowCount, 2, QStringList() << "Belum Dimulai" << "Dalam Progress" << "Selesai");
    addComboBoxToRow(rowCount, 4, QStringList() << "Low" << "Medium" << "High");
    addTimeDatePickersToRow(rowCount, 3, ui->tableWidget);
    addCheckBoxToRow(rowCount,5);


    // Set the height of the last row to 50px
    ui->tableWidget->setRowHeight(rowCount, 50);

}


void MainWindow::addComboBoxToRow(int row, int column, const QStringList &options)
{
    QComboBox *comboBox = new QComboBox(this);
    comboBox->addItems(options);

    ui->tableWidget->setCellWidget(row, column, comboBox);

    // Connecting the activated signal for QComboBox in tableWidget
    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onComboBoxIndexChanged);
}

void MainWindow::onComboBoxIndexChanged()
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
    if (comboBox) {
        // Get the current row and column of the combobox
        int row = ui->tableWidget->indexAt(comboBox->pos()).row();
        int column = ui->tableWidget->indexAt(comboBox->pos()).column();

        // Update the tableWidget with the selected value
        QTableWidgetItem *item = new QTableWidgetItem(comboBox->currentText());
        ui->tableWidget->setItem(row, column, item);
    }
}


void MainWindow::addCheckBoxToRow(int row, int column)
{
    QWidget *widget = new QWidget(this);

    QCheckBox *checkBox = new QCheckBox(widget);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addWidget(checkBox);
    layout->setAlignment(Qt::AlignCenter);

    ui->tableWidget->setCellWidget(row, column, widget);
    ui->tableWidget_2->setCellWidget(row, column, widget);

    // Connect to handle checkbox state changes
    connect(checkBox, &QCheckBox::stateChanged, this, &MainWindow::onCheckBoxStateChanged);
}


void MainWindow::addTimeDatePickersToRow(int row, int column, QTableWidget *tableWidget)
{
    // Hanya menambahkan QPushButton pada kolom 4
    if (column == 3) {
        QPushButton *dateButton = new QPushButton("Pilih Tanggal", this);
        connect(dateButton, &QPushButton::clicked, this, [=]() {
            onDateButtonClicked(row, dateButton, tableWidget);
        });

        // Mengatur tata letak untuk QPushButton
        QHBoxLayout *layout = new QHBoxLayout();
        layout->addWidget(dateButton);

        // Menetapkan tata letak ke sel di tabel
        QWidget *widget = new QWidget(this);
        widget->setLayout(layout);
        tableWidget->setCellWidget(row, column, widget);
    }
}

void MainWindow::onDateButtonClicked(int row, QPushButton *dateButton, QTableWidget *tableWidget)
{
    // Dapatkan posisi global QPushButton (tombol tanggal)
    QPoint dateButtonGlobalPos = dateButton->mapToGlobal(QPoint(0, 0));

    // Buat QCalendarWidget dan atur posisinya
    QCalendarWidget *calendarWidget = new QCalendarWidget(this);
    calendarWidget->setWindowFlags(Qt::Popup);

    // Hitung posisi agar muncul di tengah-tengah kolom ke-4
    int calendarXPos = dateButtonGlobalPos.x() + dateButton->height()*2;
    int calendarYPos = dateButtonGlobalPos.y();

    calendarWidget->move(calendarXPos, calendarYPos);
    calendarWidget->setVisible(true);

    // Sambungkan untuk menanggapi pemilihan tanggal pada kalender dan atur nilai QDateTimeEdit
    connect(calendarWidget, &QCalendarWidget::clicked, this, [=]() {
        QDateTime selectedDateTime = calendarWidget->selectedDate().startOfDay();

        // Ganti QPushButton dengan QDateTimeEdit
        replaceButtonWithDateTimeEdit(row, 3, tableWidget, selectedDateTime);

        calendarWidget->hide();
    });
}


void MainWindow::replaceButtonWithDateTimeEdit(int row, int column, QTableWidget *tableWidget, const QDateTime &dateTime)
{
    // Buat QDateTimeEdit dan atur nilainya
    QDateTimeEdit *dateTimeEdit = new QDateTimeEdit(dateTime, this);
    dateTimeEdit->setDisplayFormat("dd-MM-yyyy hh:mm"); // Format tanggal dan waktu yang diinginkan
    dateTimeEdit->setFocusPolicy(Qt::ClickFocus);

    // Mengatur gaya untuk QDateTimeEdit
    dateTimeEdit->setStyleSheet("QDateTimeEdit::down-button, QDateTimeEdit::up-button {"
                                "    width: 0px;"
                                "    height: 0px;"
                                "    border: none;"
                                "    padding-left: 10px;"
                                "}");

    // Mengatur alignment QDateTimeEdit ke center
    dateTimeEdit->setAlignment(Qt::AlignCenter);

    // Menetapkan tata letak ke sel di tabel
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(dateTimeEdit);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(layout);

    tableWidget->setCellWidget(row, column, widget);
    // Jika tableWidget adalah ui->tableWidget_2, tambahkan juga alignment ke center
      if (tableWidget == ui->tableWidget_2) {
          dateTimeEdit->setAlignment(Qt::AlignCenter);
      }
}


void MainWindow::applyTableWidgetStyle(QTableWidget *tableWidget) {

    QString tableWidgetStyle = "QComboBox { border: none; padding-right: 15px; padding-left: 35px; text-align: center; }"
                               "QComboBox::drop-down {"
                               "    border: none;"
                               "    subcontrol-origin: padding;"
                               "    subcontrol-position: center right;"
                               "    width: 0px; /* hide the arrow */"
                               "}"
                               "QComboBox::item:0 { background-color: green; color: white; }"
                               "QComboBox::item:1 { background-color: yellow; color: black; }"
                               "QComboBox::item:2 { background-color: orange; color: white; }";


    tableWidget->setStyleSheet(tableWidgetStyle);
    tableWidget->setStyleSheet(tableWidget->styleSheet());
}


void MainWindow::onCheckBoxStateChanged(int state)
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());

    if (checkBox) {
        int row = -1;
        QTableWidget *currentTableWidget = nullptr;
        QTableWidget *targetTableWidget = nullptr;

        // Iterate through rows to find the checkbox in column 5 of tableWidget or tableWidget_2
        for (int r = 0; r < ui->tableWidget->rowCount(); ++r) {
            QWidget *cellWidget = ui->tableWidget->cellWidget(r, 5);
            if (cellWidget && cellWidget->findChild<QCheckBox *>() == checkBox) {
                row = r;
                currentTableWidget = ui->tableWidget;
                targetTableWidget = ui->tableWidget_2;
                break;
            }
        }

        if (row == -1) {
            for (int r = 0; r < ui->tableWidget_2->rowCount(); ++r) {
                QWidget *cellWidget = ui->tableWidget_2->cellWidget(r, 5);
                if (cellWidget && cellWidget->findChild<QCheckBox *>() == checkBox) {
                    row = r;
                    currentTableWidget = ui->tableWidget_2;
                    targetTableWidget = ui->tableWidget;
                    break;
                }
            }
        }

        if (row != -1) {
            if (state == Qt::Checked) {
                qDebug() << "Checkbox Checked. Moving row from" << currentTableWidget << "to" << targetTableWidget;

                // Extract data from the selected row
                        QString nama = currentTableWidget->item(row, 0)->text();
                        QString deskripsi = currentTableWidget->item(row, 1)->text();

                        // Check if the data in the current row is valid
                        if (nama.isEmpty() || deskripsi.isEmpty()) {
                            qDebug() << "Warning: Data in the selected row is incomplete. Cannot proceed.";
                            return;
                        }

                        // Check if the data exists in the database
                        QSqlQuery checkQuery;
                        checkQuery.prepare("SELECT COUNT(*) FROM active_task "
                                           "WHERE nama = :nama AND deskripsi = :deskripsi");
                        checkQuery.bindValue(":nama", nama);
                        checkQuery.bindValue(":deskripsi", deskripsi);

                        if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() > 0) {
                            // Data exists in the database, now delete it
                            QSqlQuery deleteQuery;
                            deleteQuery.prepare("DELETE FROM active_task "
                                                "WHERE nama = :nama AND deskripsi = :deskripsi");
                            deleteQuery.bindValue(":nama", nama);
                            deleteQuery.bindValue(":deskripsi", deskripsi);

                            if (deleteQuery.exec()) {
                                qDebug() << "Data deleted from the active_task table";

                                // Now, insert the data into the completed_task table
                                QSqlQuery insertQuery;
                                insertQuery.prepare("INSERT INTO completed_task (nama, deskripsi, status, deadline, prioritas) "
                                                    "VALUES (:nama, :deskripsi, :status, :deadline, :prioritas)");
                                insertQuery.bindValue(":nama", nama);
                                insertQuery.bindValue(":deskripsi", deskripsi);

                                // Provide default values for status, deadline, and prioritas
                                insertQuery.bindValue(":status", "Selesai");
                                insertQuery.bindValue(":deadline", QDateTime::currentDateTime());
                                insertQuery.bindValue(":prioritas", "Default Prioritas");

                                if (insertQuery.exec()) {
                                    qDebug() << "Data inserted into the completed_task table";
                                } else {
                                    qDebug() << "Error inserting data into the completed_task table:" << insertQuery.lastError().text();
                                }
                            } else {
                                qDebug() << "Error deleting data from the active_task table:" << deleteQuery.lastError().text();
                            }
                        }

                        // Move the row to the target table widget
                        int targetRow = targetTableWidget->rowCount();
                        targetTableWidget->insertRow(targetRow);

                        for (int col = 0; col < currentTableWidget->columnCount(); ++col) {
                            // Move QTableWidgetItem
                            QTableWidgetItem *item = currentTableWidget->takeItem(row, col);
                            targetTableWidget->setItem(targetRow, col, item);

                            // Move QComboBox value
                            if (col == 2 || col == 4) {
                                QComboBox *comboBox = qobject_cast<QComboBox*>(currentTableWidget->cellWidget(row, col));
                                if (comboBox) {
                                    QComboBox *targetComboBox = new QComboBox(this);
                                    for (int i = 0; i < comboBox->count(); ++i) {
                                        targetComboBox->addItem(comboBox->itemText(i));
                                    }
                                    targetComboBox->setCurrentText(comboBox->currentText());
                                    targetTableWidget->setCellWidget(targetRow, col, targetComboBox);
                                }
                            }
                            // Move QDateTimeEdit value
                            else if (col == 3) {
                                QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit*>(currentTableWidget->cellWidget(row, col));
                                if (dateTimeEdit) {
                                    QDateTime dateTime = dateTimeEdit->dateTime();
                                    QString formattedDateTime = dateTime.toString("dd-MM-yyyy hh:mm");

                                    QDateTimeEdit *targetDateTimeEdit = new QDateTimeEdit(dateTime, this);
                                    targetDateTimeEdit->setDisplayFormat("dd-MM-yyyy hh:mm");
                                    targetDateTimeEdit->setStyleSheet("QDateTimeEdit::down-button, QDateTimeEdit::up-button { width: 0px; }");
                                    targetDateTimeEdit->setDateTime(QDateTime::fromString(formattedDateTime, "dd-MM-yyyy hh:mm"));

                                    QWidget *centeredWidget = new QWidget(this);
                                    QHBoxLayout *layout = new QHBoxLayout(centeredWidget);
                                    layout->addWidget(targetDateTimeEdit);
                                    layout->setAlignment(Qt::AlignCenter);

                                    targetTableWidget->setCellWidget(targetTableWidget->rowCount() - 1, col, centeredWidget);
                                }
                            }
                        }

                        targetTableWidget->setRowHeight(targetRow, 50);
                        // Duplicate the checkbox to the target table widget
                        QWidget *checkBoxWidget = new QWidget(this);
                        QCheckBox *targetCheckBox = new QCheckBox(checkBoxWidget);
                        targetCheckBox->setChecked(true);
                        connect(targetCheckBox, &QCheckBox::stateChanged, this, &MainWindow::onCheckBoxStateChanged);
                        QHBoxLayout *layout = new QHBoxLayout(checkBoxWidget);
                        layout->addWidget(targetCheckBox);
                        layout->setAlignment(Qt::AlignCenter);

                        targetTableWidget->setCellWidget(targetRow, 5, checkBoxWidget);

                        // Remove the original row
                        currentTableWidget->removeRow(row);
                        saveDataToDatabaseCompletedTask();


            } else if (state == Qt::Unchecked) {
                qDebug() << "Checkbox Unchecked. Moving row back from" << targetTableWidget << "to" << currentTableWidget;
                moveRowFromTableWidget2(row);
            }

        } else {
            qDebug() << "Checkbox not found in known tables.";
        }
    }
}

void MainWindow::moveRowFromTableWidget2(int row)
{
    qDebug() << "Moving row from tableWidget_2 to tableWidget. Row:" << row;

    QTableWidget *sourceTableWidget = ui->tableWidget_2;
    QTableWidget *targetTableWidget = ui->tableWidget;

    // Move the row to the target table widget
    int targetRow = targetTableWidget->rowCount();
    targetTableWidget->insertRow(targetRow);

    for (int col = 0; col < sourceTableWidget->columnCount(); ++col) {
        // Move QTableWidgetItem
        QTableWidgetItem *item = sourceTableWidget->takeItem(row, col);
        targetTableWidget->setItem(targetRow, col, item);

        // Move QComboBox value
        if (col == 2 || col == 4) {
            QComboBox *comboBox = qobject_cast<QComboBox*>(sourceTableWidget->cellWidget(row, col));
            if (comboBox) {
                QComboBox *targetComboBox = new QComboBox(this);
                for (int i = 0; i < comboBox->count(); ++i) {
                    targetComboBox->addItem(comboBox->itemText(i));
                }
                targetComboBox->setCurrentText(comboBox->currentText());
                targetTableWidget->setCellWidget(targetRow, col, targetComboBox);
            }
        }
        // Move QDateTimeEdit value
        else if (col == 3) {
            QWidget *sourceWidget = sourceTableWidget->cellWidget(row, col);
            if (sourceWidget) {
                QDateTimeEdit *sourceDateTimeEdit = sourceWidget->findChild<QDateTimeEdit *>();
                if (sourceDateTimeEdit) {
                    QDateTime dateTime = sourceDateTimeEdit->dateTime();
                    QString formattedDateTime = dateTime.toString("dd-MM-yyyy hh:mm");

                    QDateTimeEdit *dateTimeEdit = new QDateTimeEdit(dateTime, this);
                    dateTimeEdit->setDisplayFormat("dd-MM-yyyy hh:mm");
                    dateTimeEdit->setStyleSheet("QDateTimeEdit::down-button, QDateTimeEdit::up-button { width: 0px; }");

                    QWidget *centeredWidget = new QWidget(this);
                    QHBoxLayout *layout = new QHBoxLayout(centeredWidget);
                    layout->addWidget(dateTimeEdit);
                    layout->setAlignment(Qt::AlignCenter);

                    targetTableWidget->setCellWidget(targetRow, col, centeredWidget);
                }
            }
        }
    }

    // Set checkbox to Unchecked
    QWidget *checkBoxWidget = new QWidget(this);
    QCheckBox *targetCheckBox = new QCheckBox(checkBoxWidget);
    targetCheckBox->setChecked(false);
    connect(targetCheckBox, &QCheckBox::stateChanged, this, &MainWindow::onCheckBoxStateChanged);
    QHBoxLayout *layout = new QHBoxLayout(checkBoxWidget);
    layout->addWidget(targetCheckBox);
    layout->setAlignment(Qt::AlignCenter);

    targetTableWidget->setCellWidget(targetRow, 5, checkBoxWidget);

    // Set sort role for specific columns
    targetTableWidget->horizontalHeaderItem(3)->setData(Qt::UserRole, QVariant(QDateTime::fromString("dd-MM-yyyy hh:mm")));

    targetTableWidget->setRowHeight(targetRow, 50);
    // Remove the original row
    sourceTableWidget->removeRow(row);

    qDebug() << "Row moved successfully!";
}



void MainWindow::on_saveButton_clicked()
{
    saveDataToDatabase();
    saveNewDataToDatabase();

    saveDataToDatabaseCompletedTask();
}

void MainWindow::saveDataToDatabase()
{
    QTableWidget *tableWidget = ui->tableWidget;

    int rowCount = tableWidget->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        QString nama = tableWidget->item(row, 0)->text();
        QString deskripsi = tableWidget->item(row, 1)->text();
        QWidget *statusWidget = tableWidget->cellWidget(row, 2);
        QString status = "";

        if (statusWidget && statusWidget->metaObject()->className() == QString("QComboBox")) {
            QComboBox *comboBox = qobject_cast<QComboBox *>(statusWidget);
            if (comboBox) {
                status = comboBox->currentText();
            }
        }

        QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit*>(tableWidget->cellWidget(row, 3));
        QString deadline = dateTimeEdit ? dateTimeEdit->dateTime().toString(Qt::ISODate) : "";

        QWidget *prioritasWidget = tableWidget->cellWidget(row, 4);
        QString prioritas = "";

        if (prioritasWidget && prioritasWidget->metaObject()->className() == QString("QComboBox")) {
            QComboBox *comboBox = qobject_cast<QComboBox *>(prioritasWidget);
            if (comboBox) {
                prioritas = comboBox->currentText();
            }
        }

        // Dapatkan data lama dari database
        QSqlQuery oldDataQuery;
        oldDataQuery.prepare("SELECT * FROM active_task WHERE nama = :nama");
        oldDataQuery.bindValue(":nama", nama);

        if (oldDataQuery.exec() && oldDataQuery.next()) {
            QString oldDeskripsi = oldDataQuery.value("deskripsi").toString();
            QString oldStatus = oldDataQuery.value("status").toString();
            QDateTime oldDeadline = oldDataQuery.value("deadline").toDateTime();
            QString oldPrioritas = oldDataQuery.value("prioritas").toString();

            // Bandingkan dengan nilai baru sebelum memperbarui
            if (deskripsi == oldDeskripsi && status == oldStatus && deadline == oldDeadline.toString(Qt::ISODate) && prioritas == oldPrioritas) {
                qDebug() << "No changes detected. Skipping update.";
                continue;  // Tidak ada perubahan, lewati baris saat ini
            }
        }

        // Perbarui data jika ada perubahan
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE active_task "
                            "SET deskripsi = :deskripsi, status = :status, deadline = :deadline, prioritas = :prioritas "
                            "WHERE nama = :nama");
        updateQuery.bindValue(":nama", nama);
        updateQuery.bindValue(":deskripsi", deskripsi);
        updateQuery.bindValue(":status", status);
        updateQuery.bindValue(":deadline", deadline);
        updateQuery.bindValue(":prioritas", prioritas);

        if (!updateQuery.exec()) {
            qDebug() << "Failed to update data in the database";
            // Handle update error
        } else {
            qDebug() << "Data updated successfully!";
        }
    }
}

void MainWindow::saveNewDataToDatabase()
{
    QTableWidget *tableWidget = ui->tableWidget;

    int rowCount = tableWidget->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        QString nama = tableWidget->item(row, 0)->text();
        QString deskripsi = tableWidget->item(row, 1)->text();

        // Check if the cell contains a QComboBox for status
        QWidget *statusWidget = tableWidget->cellWidget(row, 2);
        QString status = "";
        if (statusWidget && statusWidget->metaObject()->className() == QString("QComboBox")) {
            QComboBox *comboBox = qobject_cast<QComboBox *>(statusWidget);
            if (comboBox) {
                status = comboBox->currentText();
            }
        }

        // Check if the cell contains a QDateTimeEdit for deadline
        QWidget *deadlineWidget = tableWidget->cellWidget(row, 3);
        QString deadline = "";
        if (deadlineWidget && deadlineWidget->metaObject()->className() == QString("QWidget")) {
            QWidget *widget = qobject_cast<QWidget *>(deadlineWidget);
            if (widget) {
                QDateTimeEdit *dateTimeEdit = widget->findChild<QDateTimeEdit *>();
                if (dateTimeEdit) {
                    deadline = dateTimeEdit->dateTime().toString(Qt::ISODate);
                }
            }
        }

        // Check if the cell contains a QComboBox for prioritas
        QWidget *prioritasWidget = tableWidget->cellWidget(row, 4);
        QString prioritas = "";
        if (prioritasWidget && prioritasWidget->metaObject()->className() == QString("QComboBox")) {
            QComboBox *comboBox = qobject_cast<QComboBox *>(prioritasWidget);
            if (comboBox) {
                prioritas = comboBox->currentText();
            }
        }

        // Check if the data in the current row is valid
        if (nama.isEmpty() || deskripsi.isEmpty() || status.isEmpty() || deadline.isEmpty() || prioritas.isEmpty()) {
            qDebug() << "Warning: Data in row" << row + 1 << " is incomplete. Continuing to save.";
            continue;  // Skip the current row if data is incomplete
        }

        // Cek apakah data sudah ada di dalam database
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT COUNT(*) FROM active_task "
                           "WHERE nama = :nama AND deskripsi = :deskripsi AND status = :status "
                           "AND deadline = :deadline AND prioritas = :prioritas");
        checkQuery.bindValue(":nama", nama);
        checkQuery.bindValue(":deskripsi", deskripsi);
        checkQuery.bindValue(":status", status);
        checkQuery.bindValue(":deadline", deadline);
        checkQuery.bindValue(":prioritas", prioritas);

        if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() > 0) {
            qDebug() << "Warning: Data in row" << row + 1 << " is same. Not saved to database.";
        } else {
            // Perform the INSERT since the data is not in the database
            QSqlQuery insertQuery;
            insertQuery.prepare("INSERT INTO active_task (nama, deskripsi, status, deadline, prioritas) "
                                "VALUES (:nama, :deskripsi, :status, :deadline, :prioritas)");
            insertQuery.bindValue(":nama", nama);
            insertQuery.bindValue(":deskripsi", deskripsi);
            insertQuery.bindValue(":status", status);
            insertQuery.bindValue(":deadline", deadline);
            insertQuery.bindValue(":prioritas", prioritas);

            if (!insertQuery.exec()) {
                qDebug() << "Gagal menyimpan data ke database";
                // Handle kesalahan penyimpanan data ke database
            } else {
                qDebug() << "Data saved successfully!";
            }
        }
    }
}


void MainWindow::loadDataActiveTask() {

    ui->tableWidget->setRowCount(0);

    // Query untuk mengambil data dari database
    QSqlQuery query("SELECT * FROM active_task");

    // Mengisi QTableWidget dengan data dari database
    while (query.next()) {
        // Mengambil nilai dari setiap kolom
        QString nama = query.value(0).toString();
        QString deskripsi = query.value(1).toString();
        QString status = query.value(2).toString();
        QDateTime deadline = QDateTime::fromString(query.value(3).toString(), Qt::ISODate); // Menggunakan QDateTime
        QString prioritas = query.value(4).toString();

        // Menambahkan baris baru ke QTableWidget dan mengisinya dengan data
        int rowPosition = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(rowPosition);
        ui->tableWidget->setItem(rowPosition, 0, new QTableWidgetItem(nama));
        ui->tableWidget->setItem(rowPosition, 1, new QTableWidgetItem(deskripsi));

        // Menggunakan QComboBox untuk kolom status
        QComboBox *statusComboBox = new QComboBox;
        statusComboBox->addItems(QStringList() << "Belum Dimulai" << "Dalam Progress" << "Selesai");
        statusComboBox->setCurrentText(status);
        ui->tableWidget->setCellWidget(rowPosition, 2, statusComboBox);

        // Menggunakan QDateTimeEdit untuk kolom deadline
        QDateTimeEdit *deadlineEdit = new QDateTimeEdit(deadline);
        deadlineEdit->setDisplayFormat("dd-MM-yyyy HH:mm");
        ui->tableWidget->setCellWidget(rowPosition, 3, deadlineEdit);
        // Mengatur gaya untuk QDateTimeEdit
        deadlineEdit->setStyleSheet("QDateTimeEdit::down-button, QDateTimeEdit::up-button {"
                                    "    width: 0px;"
                                    "    height: 0px;"
                                    "    border: none;"
                                    "    padding-left: 10px;"
                                    "}");

        // Mengatur alignment QDateTimeEdit ke center
        deadlineEdit->setAlignment(Qt::AlignCenter);

        // Menggunakan QComboBox untuk kolom prioritas
        QComboBox *prioritasComboBox = new QComboBox;
        prioritasComboBox->addItems(QStringList() << "Low" << "Medium" << "High");
        prioritasComboBox->setCurrentText(prioritas);
        ui->tableWidget->setCellWidget(rowPosition, 4, prioritasComboBox);

        addCheckBoxToRow(rowPosition, 5);

        ui->tableWidget->setRowHeight(rowPosition, 50);
    }

    connect(ui->tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::onTableWidgetStatusHeaderClicked);
    connect(ui->tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::onTableWidgetDeadlineHeaderClicked);
    connect(ui->tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::onTableWidgetPrioritasHeaderClicked);
}

void MainWindow::loadDataCompletedTask() {

    // Query untuk mengambil data dari database
    QSqlQuery query("SELECT * FROM completed_task");

    // Mengisi QTableWidget dengan data dari database
    while (query.next()) {
        // Mengambil nilai dari setiap kolom
        QString nama = query.value(0).toString();
        QString deskripsi = query.value(1).toString();
        QString status = query.value(2).toString();
        QDateTime deadline = QDateTime::fromString(query.value(3).toString(), Qt::ISODate); // Menggunakan QDateTime
        QString prioritas = query.value(4).toString();

        // Menambahkan baris baru ke QTableWidget dan mengisinya dengan data
        int rowPosition = ui->tableWidget_2->rowCount();
        ui->tableWidget_2->insertRow(rowPosition);
        ui->tableWidget_2->setItem(rowPosition, 0, new QTableWidgetItem(nama));
        ui->tableWidget_2->setItem(rowPosition, 1, new QTableWidgetItem(deskripsi));

        // Menggunakan QComboBox untuk kolom status
        QComboBox *statusComboBox = new QComboBox;
        statusComboBox->addItems(QStringList() << "Belum Dimulai" << "Dalam Progress" << "Selesai");
        statusComboBox->setCurrentText(status);
        ui->tableWidget_2->setCellWidget(rowPosition, 2, statusComboBox);
        statusComboBox->setStyleSheet("QComboBox { border: none; padding-right: 15px; padding-left: 35px; text-align: center; }"
                                      "QComboBox::drop-down {"
                                      "    border: none;"
                                      "    subcontrol-origin: padding;"
                                      "    subcontrol-position: center right;"
                                      "    width: 0px; /* hide the arrow */"
                                      "}");

        // Menggunakan QDateTimeEdit untuk kolom deadline
        QDateTimeEdit *deadlineEdit = new QDateTimeEdit(deadline);
        deadlineEdit->setDisplayFormat("dd-MM-yyyy HH:mm");
        ui->tableWidget_2->setCellWidget(rowPosition, 3, deadlineEdit);
        // Mengatur gaya untuk QDateTimeEdit
        deadlineEdit->setStyleSheet("QDateTimeEdit::down-button, QDateTimeEdit::up-button {"
                                    "    width: 0px;"
                                    "    height: 0px;"
                                    "    border: none;"
                                    "    padding-left: 10px;"
                                    "}");

        // Mengatur alignment QDateTimeEdit ke center
        deadlineEdit->setAlignment(Qt::AlignCenter);

        // Menggunakan QComboBox untuk kolom prioritas
        QComboBox *prioritasComboBox = new QComboBox;
        prioritasComboBox->addItems(QStringList() << "Low" << "Medium" << "High");
        prioritasComboBox->setCurrentText(prioritas);
        ui->tableWidget_2->setCellWidget(rowPosition, 4, prioritasComboBox);
        prioritasComboBox->setStyleSheet("QComboBox { border: none; padding-right: 15px; padding-left: 35px; text-align: center; }"
                                      "QComboBox::drop-down {"
                                      "    border: none;"
                                      "    subcontrol-origin: padding;"
                                      "    subcontrol-position: center right;"
                                      "    width: 0px; /* hide the arrow */"
                                      "}");

        ui->tableWidget_2->setRowHeight(rowPosition, 50);
    }
    connect(ui->tableWidget_2->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::onTableWidgetStatusHeaderClicked);
    connect(ui->tableWidget_2->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::onTableWidgetDeadlineHeaderClicked);
    connect(ui->tableWidget_2->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::onTableWidgetPrioritasHeaderClicked);
}

void MainWindow::saveDataToDatabaseCompletedTask()
{
    QTableWidget *tableWidget = ui->tableWidget_2;

    int rowCount = tableWidget->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        QString nama = tableWidget->item(row, 0)->text();
        QString deskripsi = tableWidget->item(row, 1)->text();
        QWidget *statusWidget = tableWidget->cellWidget(row, 2);
        QString status = "";

        if (statusWidget && statusWidget->metaObject()->className() == QString("QComboBox")) {
            QComboBox *comboBox = qobject_cast<QComboBox *>(statusWidget);
            if (comboBox) {
                status = comboBox->currentText();
            }
        }

        QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit*>(tableWidget->cellWidget(row, 3));
        QString deadline = dateTimeEdit ? dateTimeEdit->dateTime().toString(Qt::ISODate) : "";

        QWidget *prioritasWidget = tableWidget->cellWidget(row, 4);
        QString prioritas = "";

        if (prioritasWidget && prioritasWidget->metaObject()->className() == QString("QComboBox")) {
            QComboBox *comboBox = qobject_cast<QComboBox *>(prioritasWidget);
            if (comboBox) {
                prioritas = comboBox->currentText();
            }
        }

        // Dapatkan data lama dari database completed_task
        QSqlQuery oldDataQuery;
        oldDataQuery.prepare("SELECT * FROM completed_task WHERE nama = :nama");
        oldDataQuery.bindValue(":nama", nama);

        if (oldDataQuery.exec() && oldDataQuery.next()) {
            QString oldDeskripsi = oldDataQuery.value("deskripsi").toString();
            QString oldStatus = oldDataQuery.value("status").toString();
            QDateTime oldDeadline = oldDataQuery.value("deadline").toDateTime();
            QString oldPrioritas = oldDataQuery.value("prioritas").toString();

            // Bandingkan dengan nilai baru sebelum memperbarui
            if (deskripsi == oldDeskripsi && status == oldStatus && deadline == oldDeadline.toString(Qt::ISODate) && prioritas == oldPrioritas) {
                qDebug() << "No changes detected. Skipping update.";
                continue;  // Tidak ada perubahan, lewati baris saat ini
            }
        }

        // Perbarui data jika ada perubahan
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE completed_task "
                            "SET deskripsi = :deskripsi, status = :status, deadline = :deadline, prioritas = :prioritas "
                            "WHERE nama = :nama");
        updateQuery.bindValue(":nama", nama);
        updateQuery.bindValue(":deskripsi", deskripsi);
        updateQuery.bindValue(":status", status);
        updateQuery.bindValue(":deadline", deadline);
        updateQuery.bindValue(":prioritas", prioritas);

        if (!updateQuery.exec()) {
            qDebug() << "Failed to update data in the database";
            // Handle update error
        } else {
            qDebug() << "Data updated successfully!";
        }
    }
}


//BARIS KODE SORTING MULAI------------------------------------------------------------------------------------------------------------------------

void MainWindow::onTableWidgetNameHeaderClicked(int logicalIndex)
{
    // Fungsi ini akan dipanggil ketika header kolom diklik
    // Lakukan sorting berdasarkan logicalIndex (indeks kolom)
    ui->tableWidget->sortByColumn(logicalIndex, Qt::AscendingOrder);
    ui->tableWidget_2->sortByColumn(logicalIndex, Qt::AscendingOrder);
}


// Fungsi untuk sorting kolom deadline
void MainWindow::sortDeadlineColumn() {
    QTableWidget *tableWidget = ui->tableWidget;

    // Mendapatkan jumlah baris dalam tabel
    int rowCount = tableWidget->rowCount();

    // Mendapatkan arah sort saat ini dari header kolom deadline
    Qt::SortOrder currentOrder = tableWidget->horizontalHeader()->sortIndicatorOrder();

    // Menggunakan algoritma sorting sederhana (bubble sort) untuk QDateTimeEdit
    for (int i = 0; i < rowCount - 1; ++i) {
        for (int j = 0; j < rowCount - i - 1; ++j) {
            QDateTimeEdit *dateTimeEdit1 = qobject_cast<QDateTimeEdit *>(tableWidget->cellWidget(j, 3));
            QDateTimeEdit *dateTimeEdit2 = qobject_cast<QDateTimeEdit *>(tableWidget->cellWidget(j + 1, 3));
            QComboBox *comboBoxStatus1 = qobject_cast<QComboBox *>(tableWidget->cellWidget(j, 2));
            QComboBox *comboBoxStatus2 = qobject_cast<QComboBox *>(tableWidget->cellWidget(j + 1, 2));
            QComboBox *comboBoxPrioritas1 = qobject_cast<QComboBox *>(tableWidget->cellWidget(j, 4));
            QComboBox *comboBoxPrioritas2 = qobject_cast<QComboBox *>(tableWidget->cellWidget(j + 1, 4));

            if (dateTimeEdit1 && dateTimeEdit2 && comboBoxStatus1 && comboBoxStatus2 && comboBoxPrioritas1 && comboBoxPrioritas2) {
                QDateTime value1 = dateTimeEdit1->dateTime();
                QDateTime value2 = dateTimeEdit2->dateTime();
                QString statusValue1 = comboBoxStatus1->currentText();
                QString statusValue2 = comboBoxStatus2->currentText();
                QString prioritasValue1 = comboBoxPrioritas1->currentText();
                QString prioritasValue2 = comboBoxPrioritas2->currentText();

                // Melakukan pertukaran jika perlu berdasarkan arah sort
                if ((currentOrder == Qt::AscendingOrder && value1 > value2) ||
                    (currentOrder == Qt::DescendingOrder && value1 < value2)) {
                    // Melakukan pertukaran QDateTimeEdit
                    dateTimeEdit1->setDateTime(value2);
                    dateTimeEdit2->setDateTime(value1);

                    // Melakukan pertukaran QComboBox status
                    comboBoxStatus1->setCurrentText(statusValue2);
                    comboBoxStatus2->setCurrentText(statusValue1);

                    // Melakukan pertukaran QComboBox prioritas
                    comboBoxPrioritas1->setCurrentText(prioritasValue2);
                    comboBoxPrioritas2->setCurrentText(prioritasValue1);

                    // Melakukan pertukaran data seluruh baris
                    for (int column = 0; column < tableWidget->columnCount(); ++column) {
                        QTableWidgetItem *item1 = tableWidget->takeItem(j, column);
                        QTableWidgetItem *item2 = tableWidget->takeItem(j + 1, column);

                        tableWidget->setItem(j, column, item2);
                        tableWidget->setItem(j + 1, column, item1);
                    }
                }
            }
        }
    }
}

// Fungsi ini akan dipanggil ketika header kolom deadline diklik
void MainWindow::onTableWidgetDeadlineHeaderClicked(int logicalIndex) {
    if (logicalIndex == 3) { // Kolom keempat (indeks logis 3)
        sortDeadlineColumn();
    }
}

// Fungsi untuk sorting kolom prioritas beserta kolom terkait (deadline, status, nama, dan deskripsi)
void MainWindow::sortPrioritasColumn() {
    QTableWidget *tableWidget = ui->tableWidget;

    // Mendapatkan jumlah baris dalam tabel
    int rowCount = tableWidget->rowCount();

    // Mendapatkan arah sort saat ini dari header kolom prioritas
    Qt::SortOrder currentOrder = tableWidget->horizontalHeader()->sortIndicatorOrder();

    // Menggunakan algoritma sorting sederhana (bubble sort) untuk QComboBox prioritas
    for (int i = 0; i < rowCount - 1; ++i) {
        for (int j = 0; j < rowCount - i - 1; ++j) {
            QComboBox *comboBoxPrioritas1 = qobject_cast<QComboBox *>(tableWidget->cellWidget(j, 4));
            QComboBox *comboBoxPrioritas2 = qobject_cast<QComboBox *>(tableWidget->cellWidget(j + 1, 4));
            QDateTimeEdit *dateTimeEdit1 = qobject_cast<QDateTimeEdit *>(tableWidget->cellWidget(j, 3));
            QDateTimeEdit *dateTimeEdit2 = qobject_cast<QDateTimeEdit *>(tableWidget->cellWidget(j + 1, 3));
            QComboBox *comboBoxStatus1 = qobject_cast<QComboBox *>(tableWidget->cellWidget(j, 2));
            QComboBox *comboBoxStatus2 = qobject_cast<QComboBox *>(tableWidget->cellWidget(j + 1, 2));
            QTableWidgetItem *itemNama1 = tableWidget->item(j, 0);
            QTableWidgetItem *itemNama2 = tableWidget->item(j + 1, 0);
            QTableWidgetItem *itemDeskripsi1 = tableWidget->item(j, 1);
            QTableWidgetItem *itemDeskripsi2 = tableWidget->item(j + 1, 1);

            if (comboBoxPrioritas1 && comboBoxPrioritas2 && dateTimeEdit1 && dateTimeEdit2 && comboBoxStatus1 && comboBoxStatus2 &&
                itemNama1 && itemNama2 && itemDeskripsi1 && itemDeskripsi2) {
                QString valuePrioritas1 = comboBoxPrioritas1->currentText();
                QString valuePrioritas2 = comboBoxPrioritas2->currentText();
                QDateTime valueDeadline1 = dateTimeEdit1->dateTime();
                QDateTime valueDeadline2 = dateTimeEdit2->dateTime();
                QString valueStatus1 = comboBoxStatus1->currentText();
                QString valueStatus2 = comboBoxStatus2->currentText();
                QString valueNama1 = itemNama1->text();
                QString valueNama2 = itemNama2->text();
                QString valueDeskripsi1 = itemDeskripsi1->text();
                QString valueDeskripsi2 = itemDeskripsi2->text();

                // Melakukan pertukaran jika perlu berdasarkan arah sort
                if ((currentOrder == Qt::AscendingOrder && comparePrioritas(valuePrioritas1, valuePrioritas2) > 0) ||
                    (currentOrder == Qt::DescendingOrder && comparePrioritas(valuePrioritas1, valuePrioritas2) < 0)) {
                    // Melakukan pertukaran QComboBox prioritas
                    comboBoxPrioritas1->setCurrentText(valuePrioritas2);
                    comboBoxPrioritas2->setCurrentText(valuePrioritas1);

                    // Melakukan pertukaran QDateTimeEdit deadline
                    dateTimeEdit1->setDateTime(valueDeadline2);
                    dateTimeEdit2->setDateTime(valueDeadline1);

                    // Melakukan pertukaran QComboBox status
                    comboBoxStatus1->setCurrentText(valueStatus2);
                    comboBoxStatus2->setCurrentText(valueStatus1);

                    // Melakukan pertukaran QTableWidgetItem nama
                    itemNama1->setText(valueNama2);
                    itemNama2->setText(valueNama1);

                    // Melakukan pertukaran QTableWidgetItem deskripsi
                    itemDeskripsi1->setText(valueDeskripsi2);
                    itemDeskripsi2->setText(valueDeskripsi1);
                }
            }
        }
    }
}


// Fungsi pembanding untuk mengurutkan prioritas (low > medium > high)
int MainWindow::comparePrioritas(const QString &prioritas1, const QString &prioritas2) {
    QStringList prioritasOrder = {"low", "medium", "high"};

    int index1 = prioritasOrder.indexOf(prioritas1.toLower());
    int index2 = prioritasOrder.indexOf(prioritas2.toLower());

    return index1 - index2;
}

// Fungsi ini akan dipanggil ketika header kolom prioritas diklik
void MainWindow::onTableWidgetPrioritasHeaderClicked(int logicalIndex) {
    if (logicalIndex == 4) { // Kolom kelima (indeks logis 4)
        sortPrioritasColumn();
    }
}

// Fungsi untuk sorting kolom status beserta kolom terkait (deadline dan prioritas)
void MainWindow::sortStatusColumn() {
    QTableWidget *tableWidget = ui->tableWidget;

    // Mendapatkan jumlah baris dalam tabel
    int rowCount = tableWidget->rowCount();

    // Mendapatkan arah sort saat ini dari header kolom status
    Qt::SortOrder currentOrder = tableWidget->horizontalHeader()->sortIndicatorOrder();

    // Menggunakan algoritma sorting sederhana (bubble sort) untuk QComboBox status
    for (int i = 0; i < rowCount - 1; ++i) {
        for (int j = 0; j < rowCount - i - 1; ++j) {
            QComboBox *comboBoxStatus1 = qobject_cast<QComboBox *>(tableWidget->cellWidget(j, 2));
            QComboBox *comboBoxStatus2 = qobject_cast<QComboBox *>(tableWidget->cellWidget(j + 1, 2));
            QDateTimeEdit *dateTimeEdit1 = qobject_cast<QDateTimeEdit *>(tableWidget->cellWidget(j, 3));
            QDateTimeEdit *dateTimeEdit2 = qobject_cast<QDateTimeEdit *>(tableWidget->cellWidget(j + 1, 3));
            QComboBox *comboBoxPrioritas1 = qobject_cast<QComboBox *>(tableWidget->cellWidget(j, 4));
            QComboBox *comboBoxPrioritas2 = qobject_cast<QComboBox *>(tableWidget->cellWidget(j + 1, 4));

            if (comboBoxStatus1 && comboBoxStatus2 && dateTimeEdit1 && dateTimeEdit2 && comboBoxPrioritas1 && comboBoxPrioritas2) {
                QString status1 = comboBoxStatus1->currentText();
                QString status2 = comboBoxStatus2->currentText();
                QDateTime value1 = dateTimeEdit1->dateTime();
                QDateTime value2 = dateTimeEdit2->dateTime();
                QString prioritasValue1 = comboBoxPrioritas1->currentText();
                QString prioritasValue2 = comboBoxPrioritas2->currentText();

                // Melakukan pertukaran jika perlu berdasarkan arah sort
                if ((currentOrder == Qt::AscendingOrder && status1 > status2) ||
                    (currentOrder == Qt::DescendingOrder && status1 < status2)) {
                    // Melakukan pertukaran QComboBox status
                    comboBoxStatus1->setCurrentText(status2);
                    comboBoxStatus2->setCurrentText(status1);

                    // Melakukan pertukaran QDateTimeEdit deadline
                    dateTimeEdit1->setDateTime(value2);
                    dateTimeEdit2->setDateTime(value1);

                    // Melakukan pertukaran QComboBox prioritas
                    comboBoxPrioritas1->setCurrentText(prioritasValue2);
                    comboBoxPrioritas2->setCurrentText(prioritasValue1);

                    // Melakukan pertukaran data seluruh baris
                    for (int column = 0; column < tableWidget->columnCount(); ++column) {
                        QTableWidgetItem *item1 = tableWidget->takeItem(j, column);
                        QTableWidgetItem *item2 = tableWidget->takeItem(j + 1, column);

                        tableWidget->setItem(j, column, item2);
                        tableWidget->setItem(j + 1, column, item1);
                    }
                }
            }
        }
    }
}

// Fungsi ini akan dipanggil ketika header kolom status diklik
void MainWindow::onTableWidgetStatusHeaderClicked(int logicalIndex) {
    if (logicalIndex == 2) { // Kolom ketiga (indeks logis 2)
        sortStatusColumn();
    }
}

void MainWindow::on_tombolHapus_clicked()
{
    QSqlQuery query;

    // Menghapus data yang terdapat dalam database
    query.exec("DELETE FROM completed_task");

    // Menghapus seluruh data yang tertampil pada tab "complete"
    ui->tableWidget_2->setRowCount(0);
}
