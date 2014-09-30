#include "clientmanager.h"

ClientManager::ClientManager(Ui::MainWindow * ui, MainWindow * w, QObject *parent):
    ui(ui), mainW(w), QObject(parent)
{
    connect(ui->connect, SIGNAL(pressed()), this, SLOT(StartClient()));
    connect(ui->disconnect, SIGNAL(pressed()), this, SLOT(StopClient()));
    connect(ui->settings, SIGNAL(pressed()), this, SLOT(changeSettings()));
}

void ClientManager::StartClient()
{
    thread = new ClientConnector(ui, addr, port, directory);
    connect(thread, SIGNAL(setValueProgress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(thread, SIGNAL(setRangeProgress(int, int)), ui->progressBar, SLOT(setRange(int, int)));
    connect(thread, SIGNAL(resetProgress()), ui->progressBar, SLOT(reset()));
    connect(thread, SIGNAL(appendStatus(QString)), ui->statusBox, SLOT(append(QString)));
    connect(thread, SIGNAL(clearFilebox()), ui->fileBox, SLOT(clear()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(connectFailure(bool)), ui->settings, SLOT(setEnabled(bool)));
    connect(thread, SIGNAL(addListItem(QString)), this, SLOT(addFileItem(QString)));
    connect(mainW, SIGNAL(fileSelected(QString)), thread, SLOT(sendFile(QString)));

    thread->start();

    ui->settings->setEnabled(false);
}

void ClientManager::clearButton()
{
    ui->settings->setEnabled(true);
}

void ClientManager::addFileItem(QString item)
{
    ui->fileBox->addItem(item);
}

void ClientManager::changeSettings()
{
    ui->statusBox->append("Applying new connection settings.");
    this->addr = ui->hostEdit->text();
    this->port = ui->portEdit->text().toInt();
    this->directory = ui->directoryEdit->text();
}


void ClientManager::StopClient()
{
    thread->exit();
    ui->settings->setEnabled(true);
}
