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
    thread->start();

}

void ClientManager::StopClient()
{
    thread->exit();
}
