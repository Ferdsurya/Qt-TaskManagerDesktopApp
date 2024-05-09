#ifndef DASHBOARD_H
#define DASHBOARD_H
#include "notifikasi.h"
#include "mainwindow.h"

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QDebug>
#include <QCalendarWidget>
#include <QTableWidgetItem>
#include <QLabel>
#include <QMovie>
#include <QDateTimeEdit>

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStyledItemDelegate>

#include <QSqlError>
#include <QMessageBox>
#include <QPainter>
#include <QStandardItemModel>
#include <QPainterPath>


namespace Ui {
class Dashboard;
}

class Dashboard : public QWidget
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);
    ~Dashboard();
    void on_notificationButton_clicked();
    int comparePrioritas(const QString &prioritas1, const QString &prioritas2);


private slots:
    void onSearchTextChanged(const QString &text);
    void setupModel();
    void setupModel_2();

    void loadTableData();
    void setItemColors();
    void setItemColors_2();

    void on_tasklistButton_clicked();

    void updateTaskCountLabelAndProgressBar();
    void updateTimeLabel();
    void updateGreetingsLabel();

    void onTableViewDeadlineHeaderClicked(int logicalIndex);
    void onTableViewPrioritasHeaderClicked(int logicalIndex);
    void onTableViewStatusHeaderClicked(int logicalIndex);

    void on_notifButton_clicked();

private:
    Ui::Dashboard *ui;
    Notifikasi *notif;
    MainWindow *tasklist;
    QMovie *gifMovie, *gifMovie2;

    QVBoxLayout *mainLayout;
    QVBoxLayout *leftLayout;
    QVBoxLayout *rightLayout;
    QLabel *namaLabel;
    QLabel *deskripsiLabel;
    QLabel *deadlineLabel;

    void sortDeadlineColumnTableView();
    void sortPrioritasColumnTableView();
    void sortStatusColumnTableView();


};

#endif // DASHBOARD_H
