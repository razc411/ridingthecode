#include "clientmanager.h"

ClientManager::ClientManager(Ui::MainWindow *ui, QObject *parent):
    ui(ui), QObject(parent)
{
    connect(ui->connect, SIGNAL(pressed()), this, SLOT(StartClient()));
    connect(ui->disconnect, SIGNAL(pressed()), this, SLOT(StopClient()));
}

void ClientManager::StartClient()
{
    thread = new ClientConnector(ui);
    connect(thread, SIGNAL(setValueProgress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(thread, SIGNAL(setRangeProgress(int, int)), ui->progressBar, SLOT(setRange(int, int)));
    connect(thread, SIGNAL(resetProgress()), ui->progressBar, SLOT(reset()));
    connect(thread, SIGNAL(appendStatus(QString)), ui->statusBox, SLOT(append(QString)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

void ClientManager::StopClient()
{
    thread->exit();
}
