#include "myword.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Myword w;
    w.show();

    return a.exec();
}
