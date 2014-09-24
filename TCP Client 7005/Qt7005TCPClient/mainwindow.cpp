#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBox->setHtml("<b>Welcome to the QTransfer application!</b>");
    ui->fileBox->addItem("Not connected to a fileserver.");
}

Ui::MainWindow * MainWindow::getMainWindowHandle()
{
    return ui;
}

MainWindow::~MainWindow()
{
    delete ui;
}
