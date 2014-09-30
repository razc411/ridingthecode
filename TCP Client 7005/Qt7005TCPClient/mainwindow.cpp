#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBox->setHtml("<b>Welcome to the QTransfer application!</b>");
    ui->fileBox->addItem("Not connected to a fileserver.");
    ui->hostEdit->setText("localhost");
    ui->portEdit->setText("7005");
    ui->directoryEdit->setText("C:/Users/Raz");
    ui->progressBar->reset();
    connect(ui->sndfile, SIGNAL(pressed()), this, SLOT(openDialog()));
    connect(ui->clear, SIGNAL(pressed()), ui->statusBox, SLOT(clear()));
}

Ui::MainWindow * MainWindow::getMainWindowHandle()
{
    return ui;
}

void MainWindow::openDialog()
{
    QStringList fileNames;
    QFileDialog dialog(this);
    if (dialog.exec())
    fileNames = dialog.selectedFiles();
    if(!fileNames.empty())
    {
        emit fileSelected(fileNames.at(0));
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}
