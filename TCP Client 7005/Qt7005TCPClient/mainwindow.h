#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Ui::MainWindow *ui;
    Ui::MainWindow *getMainWindowHandle();
public slots:
    void openDialog();

signals:
    void fileSelected(QString filename);
};

#endif // MAINWINDOW_H
