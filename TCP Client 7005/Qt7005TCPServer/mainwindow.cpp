#include "mainwindow.h"
#include "ui_mainwindow.h"
/**
 * MainWindow Class extensd QtMainWindow
 * @author Ramzi Chennafi
 *
 * Creates and controls the main GUI.
 *
 * Functions
 * ----------------------------------------------
 * MainWindow(QWidget *parent)
 * ~MainWindow()
 * ----------------------------------------------
 */

/**
 * @brief MainWindow::MainWindow : creates and controls the main GUI.
 * @param parent - calling object, for QT use.
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}
/**
 * @brief MainWindow::~MainWindow : mainwindow destructor.
 */
MainWindow::~MainWindow()
{
    delete ui;
}
