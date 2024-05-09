#include "dashboard.h"
#include "ui_dashboard.h"
#include <QGuiApplication>
#include <QScreen>
#include <QPropertyAnimation>
#include <QTimer>

Dashboard::Dashboard(QWidget *parent) :
    QWidget(parent), ui(new Ui::Dashboard)
{
    ui->setupUi(this);

    gifMovie = new QMovie(":/prefix1/icon/techny-kanban-board-on-tablet.gif");
    ui->Jumlahtugas_label->setMovie(gifMovie);
    gifMovie->start();
    ui->Jumlahtugas_label->setScaledContents(true);

    gifMovie2 = new QMovie(":/prefix1/icon/design-science.gif");
    ui->gifLabel->setMovie(gifMovie2);
    gifMovie2->start();
    ui->gifLabel->setScaledContents(true);

    tasklist = new MainWindow();

    // Set up database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("D:/MyBoyFriend/task manager 1-6/db/QtTableTaskManager.db");

    if (!db.open()) {
        QMessageBox::critical(this, "Error", "Database connection failed:\n" + db.lastError().text());
    } else {
        setupModel();
        setupModel_2();
        loadTableData();  // Call the loadTableData function after setupModel
        updateTaskCountLabelAndProgressBar();  // Call the updateTaskCountLabel function to display the task count
    }
    updateTimeLabel();
    updateGreetingsLabel();

    connect(ui->searchBar, &QLineEdit::textChanged, this, &Dashboard::onSearchTextChanged);

   QTimer *t = new QTimer(this);
   t->setInterval(1000);
   connect(t, &QTimer::timeout, [&](){
       // Mendapatkan waktu saat ini dalam zona waktu GMT+7
       QDateTime currentTime = QDateTime::currentDateTime();

       ui->label_jam->setText(currentTime.toString("hh:mm:ss"));
       ui->label_tanggal->setText(currentTime.toString("dddd, dd MMM yyyy"));

       ui->label_jam->setStyleSheet("font-family: 'Inter'; font-size: 20px;");
       ui->label_tanggal->setStyleSheet("font-family: 'Inter'; font-size: 20px;");

       setupModel();
       setupModel_2();
       loadTableData();  // Call the loadTableData function after setupModel
       updateTaskCountLabelAndProgressBar();
       updateTimeLabel();
       updateGreetingsLabel();
   });
   t->start();


}

Dashboard::~Dashboard()
{
    delete ui;
}

void Dashboard::onSearchTextChanged(const QString &text)
{
    QTableView *currentTableView = ui->tableView;  // Ganti dengan nama objek sesuai kebutuhan Anda

    if (currentTableView) {
        QAbstractItemModel *model = currentTableView->model();

        if (model) {
            QString searchText = text.toLower();

            int rowCount = model->rowCount();
            int columnCount = model->columnCount();

            if (searchText.isEmpty()) {
                // Jika teks pencarian kosong, tampilkan semua baris
                for (int row = 0; row < rowCount; ++row) {
                    currentTableView->setRowHidden(row, false);
                }
                currentTableView->setVisible(true);
            } else {
                // Jika ada teks pencarian, terapkan logika pencarian
                bool anyRowVisible = false;

                for (int row = 0; row < rowCount; ++row) {
                    bool rowVisible = false;
                    for (int col = 0; col < columnCount; ++col) {
                        QModelIndex index = model->index(row, col);
                        QVariant data = model->data(index);

                        if (data.toString().toLower().contains(searchText)) {
                            rowVisible = true;
                            anyRowVisible = true;
                            break;
                        }
                    }
                    currentTableView->setRowHidden(row, !rowVisible);
                }

                // Set the visibility of the entire table based on whether any row is visible
                currentTableView->setVisible(anyRowVisible);
            }
        }
    }
}


void Dashboard::on_tasklistButton_clicked()
{

    tasklist->show();
    QPoint mainWindowPos = this->mapToGlobal(QPoint(0, 0));
    tasklist->setGeometry(mainWindowPos.x(), mainWindowPos.y(), width(), height());
}

void Dashboard::loadTableData()
{
    ui->tableView->resizeColumnsToContents();
    ui->tableView_2->resizeColumnsToContents();

    int rowHeight = 50;  // Change to the desired height
    for (int row = 0; row < ui->tableView->model()->rowCount(); ++row) {
        ui->tableView->setRowHeight(row, rowHeight);
    }
    for (int row = 0; row < ui->tableView_2->model()->rowCount(); ++row) {
        ui->tableView_2->setRowHeight(row, rowHeight);
    }

    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView->model());

    if (!model) {
        return;
    }

    // Set alignment of all items to center
    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 0; col < model->columnCount(); ++col) {
            QStandardItem* item = model->item(row, col);
            if (item) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }

    QStandardItemModel* model_2 = qobject_cast<QStandardItemModel*>(ui->tableView_2->model());

    if (!model_2) {
        return;
    }

    // Set alignment of all items to center
    for (int row = 0; row < model_2->rowCount(); ++row) {
        for (int col = 0; col < model_2->columnCount(); ++col) {
            QStandardItem* item = model_2->item(row, col);
            if (item) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }


    QHeaderView* header = ui->tableView->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    QHeaderView* header2 = ui->tableView_2->horizontalHeader();
    header2->setSectionResizeMode(QHeaderView::Stretch);

    setItemColors();
    setItemColors_2();

    ui->tableView->setStyleSheet("font-family: 'Inter'; font-size: 16px;");
    ui->tableView_2->setStyleSheet("font-family: 'Inter'; font-size: 16px;");
}

void Dashboard::updateTaskCountLabelAndProgressBar() {
    // Use the previously set up database object
    QSqlDatabase db = QSqlDatabase::database();

    if (!db.isOpen()) {
        qDebug() << "Database is not open. Unable to update task count and progress bar.";
        return;
    }

    QSqlQuery activeTaskQuery(db);
    QSqlQuery completedTaskQuery(db);

    // Count the number of rows (tasks) in the "active_task" table
    int activeTaskCount = 0;
    if (activeTaskQuery.exec("SELECT COUNT(*) FROM active_task")) {
        if (activeTaskQuery.next()) {
            activeTaskCount = activeTaskQuery.value(0).toInt();
        } else {
            qDebug() << "Query result for active tasks is empty.";
        }
    } else {
        qDebug() << "Error in query for active tasks: " << activeTaskQuery.lastError().text();
        return;
    }

    // Count the number of rows (completed tasks) in the "completed_task" table
    int completedTaskCount = 0;
    if (completedTaskQuery.exec("SELECT COUNT(*) FROM completed_task")) {
        if (completedTaskQuery.next()) {
            completedTaskCount = completedTaskQuery.value(0).toInt();
        } else {
            qDebug() << "Query result for completed tasks is empty.";
        }
    } else {
        qDebug() << "Error in query for completed tasks: " << completedTaskQuery.lastError().text();
        return;
    }

    // Display the task count on QLabel
    ui->label_jumlahtugas->setText(QString("Ada %1 Tugas yang belum diselesaikan").arg(activeTaskCount));

    // Update the progress bar based on the ratio of completed tasks to total tasks
    int totalTaskCount = activeTaskCount + completedTaskCount;
    int progressValue = 0;
    if (totalTaskCount > 0) {
        progressValue = static_cast<int>((static_cast<double>(completedTaskCount) / totalTaskCount) * 100);
    }

    // Set the value of the progress bar
    ui->progressBar->setValue(progressValue);
}


void Dashboard::setupModel()
{
    // Use QStandardItemModel instead of QSqlQueryModel
    QStandardItemModel *itemModel = new QStandardItemModel(this);

    // Set up the model with your query
    QSqlQuery query("SELECT * FROM active_task");
    int row = 0;
    while (query.next()) {
        for (int col = 0; col < query.record().count(); ++col) {
            QStandardItem *item;
            if (col == 3) { // Assuming "Deadline" is at index 3
                QDateTime deadline = query.value(col).toDateTime();
                item = new QStandardItem(deadline.toString("dd MMMM yyyy hh:mm"));
                // Set the actual QDateTime value as UserData for later use if needed
                item->setData(deadline, Qt::UserRole);
            } else {
                item = new QStandardItem(query.value(col).toString());
            }
            itemModel->setItem(row, col, item);
        }
        ++row;
    }

    // Set the header data
    itemModel->setHorizontalHeaderLabels({"Nama", "Deskripsi", "Status", "Deadline", "Prioritas"});

    // Set model for QTableView
    ui->tableView->setModel(itemModel);

    connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &Dashboard::onTableViewDeadlineHeaderClicked);
    connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &Dashboard::onTableViewStatusHeaderClicked);
    connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &Dashboard::onTableViewPrioritasHeaderClicked);
}
void Dashboard::setupModel_2()
{
    // Use QStandardItemModel instead of QSqlQueryModel
    QStandardItemModel *itemModel = new QStandardItemModel(this);

    // Set up the model with your query
    QSqlQuery query("SELECT * FROM completed_task");
    int row = 0;
    while (query.next()) {
        for (int col = 0; col < query.record().count(); ++col) {
            QStandardItem *item;
            if (col == 3) { // Assuming "Deadline" is at index 3
                QDateTime deadline = query.value(col).toDateTime();
                item = new QStandardItem(deadline.toString("dd MMMM yyyy hh:mm"));
                // Set the actual QDateTime value as UserData for later use if needed
                item->setData(deadline, Qt::UserRole);
            } else {
                item = new QStandardItem(query.value(col).toString());
            }
            itemModel->setItem(row, col, item);
        }
        ++row;
    }

    // Set the header data
    itemModel->setHorizontalHeaderLabels({"Nama", "Deskripsi", "Status", "Deadline", "Prioritas"});

    // Set model for QTableView_2
    ui->tableView_2->setModel(itemModel);
}

void Dashboard::setItemColors()
{
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView->model());

    if (!model) {
        return;
    }

    // Mengatur warna latar belakang berdasarkan nilai kolom "Status" dan "Prioritas"
    for (int row = 0; row < model->rowCount(); ++row) {
        QStandardItem* statusItem = model->item(row, 2);  // Kolom "Status"
        QStandardItem* prioritasItem = model->item(row, 4);  // Kolom "Prioritas"

        // Periksa tipe data sebelum mengonversi ke string
        if (statusItem && prioritasItem) {
            QString status = statusItem->text();
            QString prioritas = prioritasItem->text();

            // Set background color based on "Status"
            if (status == "Belum Dimulai") {
                statusItem->setBackground(QBrush(QColor(217, 217, 217, 1 * 255))); // #D9D9D9 with 49% opacity
            } else if (status == "Dalam Progress") {
                statusItem->setBackground(QBrush(QColor(214, 228, 238, 0.49 * 255))); // #D6E4EE with 49% opacity
            } else if (status == "Selesai") {
                statusItem->setBackground(QBrush(QColor(191, 244, 77, 0.49 * 255))); // #BFF44D with 49% opacity
            }

            // Set background color based on "Prioritas"
            if (prioritas.toLower() == "low") {
                prioritasItem->setBackground(QBrush(QColor(191, 244, 77, 0.49 * 255))); // #BFF44D with 49% opacity
            } else if (prioritas.toLower() == "medium") {
                prioritasItem->setBackground(QBrush(QColor(244, 157, 77, 0.49 * 255))); // #F49D4D with 49% opacity
            } else if (prioritas.toLower() == "high") {
                prioritasItem->setBackground(QBrush(QColor(255, 202, 202, 0.70 * 255))); // #FFCACA with 49% opacity
            }
        }
    }
}

void Dashboard::setItemColors_2()
{
    QStandardItemModel* model_2 = qobject_cast<QStandardItemModel*>(ui->tableView_2->model());

    if (!model_2) {
        return;
    }

    // Mengatur warna latar belakang berdasarkan nilai kolom "Status" dan "Prioritas"
    for (int row = 0; row < model_2->rowCount(); ++row) {
        QStandardItem* statusItem = model_2->item(row, 2);  // Kolom "Status"
        QStandardItem* prioritasItem = model_2->item(row, 4);  // Kolom "Prioritas"

        // Periksa tipe data sebelum mengonversi ke string
        if (statusItem && prioritasItem) {
            QString status = statusItem->text();
            QString prioritas = prioritasItem->text();

            // Set background color based on "Status"
            if (status == "Belum Dimulai") {
                statusItem->setBackground(QBrush(QColor(217, 217, 217, 1 * 255))); // #D9D9D9 with 49% opacity
            } else if (status == "Dalam Progress") {
                statusItem->setBackground(QBrush(QColor(214, 228, 238, 0.49 * 255))); // #D6E4EE with 49% opacity
            } else if (status == "Selesai") {
                statusItem->setBackground(QBrush(QColor(191, 244, 77, 0.49 * 255))); // #BFF44D with 49% opacity
            }

            // Set background color based on "Prioritas"
            if (prioritas.toLower() == "low") {
                prioritasItem->setBackground(QBrush(QColor(191, 244, 77, 0.49 * 255))); // #BFF44D with 49% opacity
            } else if (prioritas.toLower() == "medium") {
                prioritasItem->setBackground(QBrush(QColor(244, 157, 77, 0.49 * 255))); // #F49D4D with 49% opacity
            } else if (prioritas.toLower() == "high") {
                prioritasItem->setBackground(QBrush(QColor(255, 202, 202, 0.70 * 255))); // #FFCACA with 49% opacity
            }
        }
    }
}

void Dashboard::updateTimeLabel() {
    // Mendapatkan waktu saat ini dalam zona waktu GMT+7
    QDateTime currentTime = QDateTime::currentDateTime();

    ui->label_jam->setText(currentTime.toString("hh:mm:ss"));
    ui->label_tanggal->setText(currentTime.toString("dddd, dd MMM yyyy"));

    ui->label_jam->setStyleSheet("font-family: 'Inter'; font-size: 20px;");
    ui->label_tanggal->setStyleSheet("font-family: 'Inter'; font-size: 20px;");
}

void Dashboard::updateGreetingsLabel() {
    // Mendapatkan waktu saat ini
    QTime currentTime = QTime::currentTime();

    // Memeriksa waktu dan menyesuaikan teks pada label_halo
    if (currentTime >= QTime(4, 0) && currentTime < QTime(12, 0)) {
        ui->label_halo->setText("Selamat Pagi");
    } else if (currentTime >= QTime(12, 0) && currentTime < QTime(15, 0)) {
        ui->label_halo->setText("Selamat Siang");
    } else if (currentTime >= QTime(15, 0) && currentTime < QTime(18, 0)) {
        ui->label_halo->setText("Selamat Sore");
    } else {
        // Waktu di luar rentang dianggap sebagai malam
        ui->label_halo->setText("Selamat Malam");
    }
}

//BARIS KODE SORTING MULAI------------------------------------------------------------------------------------------------------------------------

void Dashboard::sortDeadlineColumnTableView() {
    QTableView *tableView = ui->tableView;

    // Mendapatkan model dari QTableView
    QAbstractItemModel *model = tableView->model();

    // Mendapatkan jumlah baris dalam tabel
    int rowCount = model->rowCount();

    // Mendapatkan arah sort saat ini dari header kolom deadline
    Qt::SortOrder currentOrder = tableView->horizontalHeader()->sortIndicatorOrder();

    // Menggunakan algoritma sorting sederhana (bubble sort) untuk QDateTimeEdit
    for (int i = 0; i < rowCount - 1; ++i) {
        for (int j = 0; j < rowCount - i - 1; ++j) {
            // Mendapatkan nilai datetime dalam format string
            QString strValue1 = model->data(model->index(j, 3), Qt::EditRole).toString();
            QString strValue2 = model->data(model->index(j + 1, 3), Qt::EditRole).toString();

            // Mengonversi string ke QDateTime sesuai dengan format
            QDateTime value1 = QDateTime::fromString(strValue1, "dd MMMM yyyy hh:mm");
            QDateTime value2 = QDateTime::fromString(strValue2, "dd MMMM yyyy hh:mm");

            // Melakukan pertukaran jika perlu berdasarkan arah sort
            if ((currentOrder == Qt::AscendingOrder && value1 > value2) ||
                (currentOrder == Qt::DescendingOrder && value1 < value2)) {
                // Melakukan pertukaran data seluruh kolom
                for (int column = 0; column < model->columnCount(); ++column) {
                    // Menyesuaikan dengan tipe data asli item di kolom deadline
                    QVariant item1Data = model->data(model->index(j, column), Qt::EditRole);
                    QVariant item2Data = model->data(model->index(j + 1, column), Qt::EditRole);

                    model->setData(model->index(j, column), item2Data, Qt::EditRole);
                    model->setData(model->index(j + 1, column), item1Data, Qt::EditRole);
                }
            }
        }
    }
}

// Fungsi ini akan dipanggil ketika header kolom deadline di QTableView diklik
void Dashboard::onTableViewDeadlineHeaderClicked(int logicalIndex) {
        if (logicalIndex == 3) {
            sortDeadlineColumnTableView();
        }
    }

// Fungsi untuk sorting kolom prioritas di QTableView beserta kolom terkait (deadline, status, nama, dan deskripsi)
void Dashboard::sortPrioritasColumnTableView() {
    QTableView *tableView = ui->tableView;

    // Mendapatkan model dari QTableView
    QAbstractItemModel *model = tableView->model();

    // Mendapatkan jumlah baris dalam tabel
    int rowCount = model->rowCount();

    // Mendapatkan arah sort saat ini dari header kolom prioritas
    Qt::SortOrder currentOrder = tableView->horizontalHeader()->sortIndicatorOrder();

    // Menggunakan algoritma sorting sederhana (bubble sort) untuk QComboBox prioritas
    for (int i = 0; i < rowCount - 1; ++i) {
        for (int j = 0; j < rowCount - i - 1; ++j) {
            QString valuePrioritas1 = model->data(model->index(j, 4), Qt::EditRole).toString();
            QString valuePrioritas2 = model->data(model->index(j + 1, 4), Qt::EditRole).toString();

            // Melakukan pertukaran jika perlu berdasarkan arah sort
            if ((currentOrder == Qt::AscendingOrder && comparePrioritas(valuePrioritas1, valuePrioritas2) > 0) ||
                (currentOrder == Qt::DescendingOrder && comparePrioritas(valuePrioritas1, valuePrioritas2) < 0)) {
                // Melakukan pertukaran data seluruh kolom
                for (int column = 0; column < model->columnCount(); ++column) {
                    QVariant item1Data = model->data(model->index(j, column), Qt::EditRole);
                    QVariant item2Data = model->data(model->index(j + 1, column), Qt::EditRole);

                    model->setData(model->index(j, column), item2Data, Qt::EditRole);
                    model->setData(model->index(j + 1, column), item1Data, Qt::EditRole);
                }
            }
        }
    }
}

int Dashboard::comparePrioritas(const QString &prioritas1, const QString &prioritas2) {
    QStringList prioritasOrder = {"low", "medium", "high"};

    int index1 = prioritasOrder.indexOf(prioritas1.toLower());
    int index2 = prioritasOrder.indexOf(prioritas2.toLower());

    return index1 - index2;
}

// Fungsi ini akan dipanggil ketika header kolom prioritas di QTableView diklik
void Dashboard::onTableViewPrioritasHeaderClicked(int logicalIndex) {
    if (logicalIndex == 4) { // Kolom kelima (indeks logis 4)
        sortPrioritasColumnTableView();
    }
}

// Fungsi untuk sorting kolom status di QTableView beserta kolom terkait (deadline dan prioritas)
void Dashboard::sortStatusColumnTableView() {
    QTableView *tableView = ui->tableView;

    // Mendapatkan model dari QTableView
    QAbstractItemModel *model = tableView->model();

    // Mendapatkan jumlah baris dalam tabel
    int rowCount = model->rowCount();

    // Mendapatkan arah sort saat ini dari header kolom status
    Qt::SortOrder currentOrder = tableView->horizontalHeader()->sortIndicatorOrder();

    // Menggunakan algoritma sorting sederhana (bubble sort) untuk QComboBox status
    for (int i = 0; i < rowCount - 1; ++i) {
        for (int j = 0; j < rowCount - i - 1; ++j) {
            QString status1 = model->data(model->index(j, 2), Qt::EditRole).toString();
            QString status2 = model->data(model->index(j + 1, 2), Qt::EditRole).toString();

            // Melakukan pertukaran jika perlu berdasarkan arah sort
            if ((currentOrder == Qt::AscendingOrder && status1 > status2) ||
                (currentOrder == Qt::DescendingOrder && status1 < status2)) {
                // Melakukan pertukaran data seluruh kolom
                for (int column = 0; column < model->columnCount(); ++column) {
                    QVariant item1Data = model->data(model->index(j, column), Qt::EditRole);
                    QVariant item2Data = model->data(model->index(j + 1, column), Qt::EditRole);

                    model->setData(model->index(j, column), item2Data, Qt::EditRole);
                    model->setData(model->index(j + 1, column), item1Data, Qt::EditRole);
                }
            }
        }
    }
}

// Fungsi ini akan dipanggil ketika header kolom status di QTableView diklik
void Dashboard::onTableViewStatusHeaderClicked(int logicalIndex) {
    if (logicalIndex == 2) { // Kolom ketiga (indeks logis 2)
        sortStatusColumnTableView();
    }
}

void Dashboard::on_notifButton_clicked()
{
    Notifikasi *notif = new Notifikasi(this);

    // Atur posisi form di tengah-tengah MainWindow
    QPoint mainWindowPos = this->pos();
    int x = mainWindowPos.x() + (this->width() - notif->width());
    int y = mainWindowPos.y() + ((this->height()*1/3) - notif->height());

    notif->move(x, y);
    notif->show();
}
