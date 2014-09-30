#include "connectionmanager.h"
/**
 * ConnectionManager Class extends QTcpServer
 * @author Ramzi Chennafi
 *
 * Manages incoming connections and creates client handler threads for each new client.
 * Also establishes default slots and values.
 *
 * Functions
 * ----------------------------------------------
 * ConnectionManager(MainWindow *win, QObject *parent)
 * void appendFileListing()
 * void applySettings()
 * void StartServer()
 * void StopServer()
 * void incomingConnection(qintptr)
 * ----------------------------------------------
 */

/**
 * @brief ConnectionManager::ConnectionManager : intiates server defaults and connections
 * @param win - MainWindow instance of the GUI
 * @param parent - default calling object
 */
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
/**
 * @brief ConnectionManager::appendFileListing : adds the file listing to the GUI.
 */
void ConnectionManager::appendFileListing()
{
    QStringList files = fManager->grabFileListing();
    win->ui->fileListing->addItems(files);
}
/**
 * @brief ConnectionManager::applySettings : changes variables within the ConnectionManager.
 *          These variables are called when StartServer is intiated. Retrieved from the GUI
 *          when the settings button is hit. Cannot be called when server is listening.
 */
void ConnectionManager::applySettings()
{
    win->ui->statusBox->append("Settings applied.");
    directory = win->ui->directoryEdit->text();
    saveDirectory = win->ui->saveDir->text();
    port = win->ui->portEdit->text().toInt();
}
/**
 * @brief ConnectionManager::StartServer : establishes the FileManager and causes the server to listen for connections.
 *          Grabs the set directory and port from the connection manager.
 */
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
/**
 * @brief ConnectionManager::StopServer : stops the server.
 *          Terminates all client threads and closes the socket.
 */
void ConnectionManager::StopServer()
{
   win->ui->statusBox->append("Closing the connection...");
   win->ui->fileListing->clear();
   for(int i = 0; i < threadList.size(); i++)
   {
       threadList.at(i)->exit();
   }
   this->close();
   win->ui->settings->setEnabled(true);
   win->ui->statusBox->append("Connection closed.");
}
/**
 * @brief ConnectionManager::incomingConnection : called when a connection is requested with the server.
 *          Creates a new client handler thread for the one requesting a connection and passes them off
 *          to that thread.
 * @param socketDescriptor - descriptor of the incoming connection. Passed to the clienthandler to deal with
 *                  the connection.
 */
void ConnectionManager::incomingConnection(qintptr socketDescriptor)
{
    win->ui->statusBox->append(QString("Established socket %1, connecting...").arg(socketDescriptor));
    ClientHandler *thread = new ClientHandler(socketDescriptor, fManager, win, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(appendStatus(QString)), win->ui->statusBox, SLOT(append(QString)));
    thread->start();
}

