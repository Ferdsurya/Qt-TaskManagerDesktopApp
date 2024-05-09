#include "notifikasi.h"
#include "ui_notifikasi.h"
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QMessageBox>
#include <QLabel>
#include <QDateTime>
#include <QDebug>
#include <QTextBrowser>

Notifikasi::Notifikasi(QWidget *parent) : QWidget(parent), ui(new Ui::Notifikasi)
{
    ui->setupUi(this);

    setupDatabase();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Notifikasi::checkAndShowNotification);

    // Jalankan checkAndShowNotification setelah 0,1 detik
    QTimer::singleShot(100, this, &Notifikasi::checkAndShowNotification);

    // Tutup checkAndShowNotification setelah 7 detik
    QTimer::singleShot(7*1000, this, &Notifikasi::on_pushButton_clicked);
}

void Notifikasi::setupDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/Users/ferdi surya prayoga/Desktop/db/QtTableTaskManager.db");

    if (!db.open())
    {
        qDebug() << "Error opening database: " << db.lastError().text();
        QMessageBox::critical(this, "Error", "Tidak dapat membuka database: " + db.lastError().text());
    }
    else
    {
        qDebug() << "Database opened successfully.";
    }
}


void Notifikasi::checkAndShowNotification()
{
    QSqlQuery query("SELECT nama, deskripsi, deadline FROM active_task WHERE deadline >= datetime('now') ORDER BY deadline ASC LIMIT 1");

    if (query.next())
    {
        QString namaTugas = query.value(0).toString();
        QString deskripsi = query.value(1).toString();
        QString deadline = query.value(2).toDateTime().toString("dd-MM-yyyy hh:mm:ss");

        qDebug() << "Notifikasi Tugas: " << namaTugas;
        qDebug() << "Deskripsi: " << deskripsi;
        qDebug() << "Deadline: " << deadline;

        ui->NotifikasiTugas_label->setText("Notifikasi Tugas " + namaTugas);
        ui->textBrowser->setText("Tenggat waktu " + deskripsi + " akan berakhir pada " + deadline);

        // Tampilkan notifikasi animasi atau sesuaikan dengan kebutuhan Anda

    }
    else
    {
        qDebug() << "No upcoming tasks found.";
    }
}


void Notifikasi::on_pushButton_clicked()
{
    hide();
}
