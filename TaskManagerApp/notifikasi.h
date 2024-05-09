#ifndef NOTIFIKASI_H
#define NOTIFIKASI_H

#include <QWidget>
#include <QTimer>
#include <QSqlDatabase>

namespace Ui {
class Notifikasi;
}

class Notifikasi : public QWidget
{
    Q_OBJECT

public:
    explicit Notifikasi(QWidget *parent = nullptr);
    void setupDatabase();

private slots:
    void checkAndShowNotification();

    void on_pushButton_clicked();

private:
    Ui::Notifikasi *ui; // Menggunakan widget yang sudah ada di notifikasi.ui
    QTimer *timer;
    QSqlDatabase db;
};

#endif // NOTIFIKASI_H
