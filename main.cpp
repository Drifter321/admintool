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

    defaultPalette.setColor(QPalette::Link,Qt::black);
    defaultPalette.setColor(QPalette::LinkVisited, Qt::black);

    a.setPalette(defaultPalette);

    MainWindow w;

    return a.exec();
}
