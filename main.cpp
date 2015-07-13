#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QPalette>

QPalette defaultPalette;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));

    defaultPalette = a.palette();
    MainWindow w;

    return a.exec();
}
