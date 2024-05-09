#include <QApplication>
#include "dashboard.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Dashboard dashboard; // Gantilah dengan kelas yang diinginkan
    dashboard.show();

    return a.exec();
}
