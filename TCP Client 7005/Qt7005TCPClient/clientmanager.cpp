#include "clientmanager.h"
/**
 * ClientManager Class extends QObject
 * @author Ramzi Chennafi
 *
 * Class which manages the starting and stopping as well as intial connecting of the client
 * network thread.
 *
 * Functions
 * -----------------------------
 * ClientManager(Ui::MainWindow *ui, MainWindow *w, QObject *parent)
 * void StartClient()
 * void addFileItem(QString)
 * void changeSettings()
 * -----------------------------
 */

/**
 * @brief ClientManager::ClientManager : Constructor for the ClientManager
 *          Connects several key slots and establishes default values for the client.
 * @param ui - ui of the mainwindow. Used for GUI connection.
 * @param w - the MainWindow. Used for GUI connection.
 * @param parent - Sets default object.
 */
ClientManager::ClientManager(Ui::MainWindow * ui, MainWindow * w, QObject *parent):
    ui(ui), mainW(w), QObject(parent)
{
    connect(ui->connect, SIGNAL(pressed()), this, SLOT(StartClient()));
    connect(ui->settings, SIGNAL(pressed()), this, SLOT(changeSettings()));
}
/**
 * @brief ClientManager::StartClient : starts the client thread.
 *          Connects several key slots and disables the settings menu.
 */
void ClientManager::StartClient()
{
    QThread * thread = new QThread;
    cConnector = new ClientConnector(ui, addr, port, directory);

    connect(cConnector, SIGNAL(setValueProgress(int)), ui->progressBar, SLOT(setValue(int))); // GUI
    connect(cConnector, SIGNAL(setRangeProgress(int, int)), ui->progressBar, SLOT(setRange(int, int))); // GUI
    connect(cConnector, SIGNAL(resetProgress()), ui->progressBar, SLOT(reset())); // GUI
    connect(cConnector, SIGNAL(appendStatus(QString)), ui->statusBox, SLOT(append(QString))); // GUI
    connect(cConnector, SIGNAL(clearFilebox()), ui->fileBox, SLOT(clear())); // GUI

    connect(cConnector, SIGNAL(connectFailure(bool)), ui->settings, SLOT(setEnabled(bool)));
    connect(cConnector, SIGNAL(addListItem(QString)), this, SLOT(addFileItem(QString)));
    connect(mainW, SIGNAL(fileSelected(QString)), cConnector, SLOT(sendFile(QString)));

    ui->settings->setEnabled(false);

    cConnector->moveToThread(thread);
    thread->start();
    cConnector->run();
}
/**
 * @brief ClientManager::addFileItem : slot that adds items to the file listing.
 * @param item - item to add the the file listing.
 */
void ClientManager::addFileItem(QString item)
{
    ui->fileBox->addItem(item);
}
/**
 * @brief ClientManager::changeSettings : slot that changes the host, port and directory settings when "Apply Settings" is pressed.
 *          Cannot be called while client is actively connected.
 */
void ClientManager::changeSettings()
{
    ui->statusBox->append("Applying new connection settings.");
    this->addr = ui->hostEdit->text();
    this->port = ui->portEdit->text().toInt();
    this->directory = ui->directoryEdit->text();
}
