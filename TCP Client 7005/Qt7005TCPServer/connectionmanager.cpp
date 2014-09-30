#include "connectionmanager.h"

ConnectionManager::ConnectionManager(MainWindow * win, QObject *parent) :
    win(win), QTcpServer(parent)
{
     win->ui->saveDir->setText("C:/");
     win->ui->portEdit->setText("7005");
     win->ui->directoryEdit->setText("C:/");
     connect(win->ui->startServer, SIGNAL(pressed()), this, SLOT(StartServer()));
     connect(win->ui->stopServer, SIGNAL(pressed()), this, SLOT(StopServer()));
     connect(win->ui->clearStatus, SIGNAL(pressed()), win->ui->statusBox, SLOT(clear()));
     connect(win->ui->settings, SIGNAL(pressed()), this, SLOT(applySettings()));
}

void ConnectionManager::appendFileListing()
{
    QStringList files = fManager->grabFileListing();
    win->ui->fileListing->addItems(files);
}

void ConnectionManager::applySettings()
{
    directory = win->ui->directoryEdit->text();
    saveDirectory = win->ui->saveDir->text();
    port = win->ui->portEdit->text().toInt();
}

void ConnectionManager::StartServer()
{
    fManager = new FileManager(directory);
    appendFileListing();

    if(!this->listen(QHostAddress::Any, port))
    {
        win->ui->statusBox->append("Could not start server.");
    }
    else
    {
        win->ui->statusBox->append("Listening...");
        win->ui->settings->setEnabled(false);
    }
}

void ConnectionManager::StopServer()
{
   win->ui->statusBox->append("Closing the connection...");

   for(int i = 0; i < threadList.size(); i++)
   {
       threadList.at(i)->exit();
   }
   this->close();
   win->ui->settings->setEnabled(true);
   win->ui->statusBox->append("Connection closed.");
}


void ConnectionManager::incomingConnection(qintptr socketDescriptor)
{
    win->ui->statusBox->append(QString("Established socket %1, connecting...").arg(socketDescriptor));
    ClientHandler *thread = new ClientHandler(socketDescriptor, fManager, win, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(appendStatus(QString)), win->ui->statusBox, SLOT(append(QString)));
    thread->start();
}

