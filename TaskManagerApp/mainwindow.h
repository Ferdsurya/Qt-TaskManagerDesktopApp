#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QDebug>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QTimer>
#include <QTextEdit>
#include <QDateEdit>
#include <QTimeEdit>
#include <QCalendarWidget>
#include <QTableWidgetItem>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QClipboard>


#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>


QT_BEGIN_NAMESPACE

namespace Ui { class MainWindow;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int comparePrioritas(const QString &prioritas1, const QString &prioritas2);

private slots:
    void on_addrowButton_clicked();

    void addComboBoxToRow(int row, int column, const QStringList &options);
    void addCheckBoxToRow(int row, int column);

    void onCheckBoxStateChanged(int state);
    void moveRowFromTableWidget2(int row);


    void applyTableWidgetStyle(QTableWidget *tableWidget);

    void addTimeDatePickersToRow(int row, int column, QTableWidget *tableWidget);
    void onDateButtonClicked(int row, QPushButton *dateButton, QTableWidget *tableWidget);
    void replaceButtonWithDateTimeEdit(int row, int column, QTableWidget *tableWidget, const QDateTime &dateTime);


    //BARIS KODE SORTING MULAI------------------------------------------------------------------------------------------------------------------------
    void onComboBoxIndexChanged();
    void onTableWidgetNameHeaderClicked(int logicalIndex);

    void onTableWidgetDeadlineHeaderClicked(int logicalIndex);
    void sortDeadlineColumn();

    void onTableWidgetStatusHeaderClicked(int logicalIndex);
    void sortStatusColumn();

    void onTableWidgetPrioritasHeaderClicked(int logicalIndex);
    void sortPrioritasColumn();

    //SEARCH BAR-----------------------------------------------------------------
       void onSearchTextChanged(const QString &text);

    void on_dashboardButton_clicked();

    //database
        void on_saveButton_clicked();
        void saveDataToDatabase();
        void saveNewDataToDatabase();

        void loadDataActiveTask();
        void loadDataCompletedTask();
        void saveDataToDatabaseCompletedTask();




        void on_tombolHapus_clicked();

private:
    Ui::MainWindow *ui;
    QTabWidget* tabWd;
    QPushButton *addButton;


    Qt::SortOrder deadlineSortOrder = Qt::AscendingOrder;


};
#endif // MAINWINDOW_H



//mainwindow.h selesai
