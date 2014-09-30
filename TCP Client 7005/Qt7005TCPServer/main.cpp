#include "mainwindow.h"
#include "connectionmanager.h"
#include <QApplication>
/**
 * @brief main : main loop of the program. Intiates objects and goes into an event loop.
 * @param argc - unused
 * @param argv - unused
 * @return - 0 on proper termination.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    ConnectionManager server(&w);

    return a.exec();
}


