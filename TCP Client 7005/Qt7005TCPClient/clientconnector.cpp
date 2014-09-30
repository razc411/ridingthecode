#include "clientconnector.h"
/**
 *  Qt7005TCPClient
 *  ClientConnector Class extends QThread
 *  @author Ramzi Chennafi
 *
 *  Called whenever the client intiates the connect button. Handles all network communications
 *  so that the GUI thread can continue without hinderence.
 *
 *  Functions
 *  -------------------------------------
 *  ClientConnection(Ui::MainWindow *, QString, int, QString, QObject*)
 *  void run()
 *  void connected()
 *  void disconnectClient()
 *  void readyRead()
 *  void sendFile()
 *  void disconnectFileList(int)
 *  void grabRequestFile()
 *  QString readFilenameHeader()
 *  quint32 grabFileSize()
 *  void processFileList()
 *  void sendRequestPacket(QWidgetItem*)
 *  -------------------------------------
 */

/**
 * @brief ClientConnector::ClientConnector
 * @param ui - ui instance of the mainwindow.
 * @param addr - the address to connect the client to. Defaults to localhost.
 * @param port - the port to connect the client with. Defaults to 7005.
 * @param directory - the directory the client will save files to. Defaults to C:/
 *              Regardless of windows or linux, directory arguments are taken with forward slashes.
 * @param parent - the parent of the constructing object. Defaults to NULL.
 */
ClientConnector::ClientConnector(Ui::MainWindow * ui, QString addr, int port, QString directory, QObject *parent) :
    ui(ui), QThread(parent), addr(addr), port(port), directory(directory)
{
}
/**
 * @brief ClientConnector::run : Run method for the client connector thread.
 *          Establishes a new qtcpsocket, joins the required slots and connects to the specified host.
 */
void ClientConnector::run()
{
    socket = new QTcpSocket();
    socket->moveToThread(this);

    connect(socket, SIGNAL(connected()), this, SLOT(connected()), Qt::DirectConnection);
    connect(ui->disconnect, SIGNAL(pressed()), this, SLOT(disconnectClient()), Qt::DirectConnection);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(ui->fileBox, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(sendRequestPacket(QListWidgetItem *)));
    connect(this, SIGNAL(clearSettings(bool)), ui->settings, SLOT(setEnabled(bool)));
    emit appendStatus("Connecting to server...");

    socket->connectToHost(addr, port);
    if(!socket->waitForConnected(10000))
    {
        emit appendStatus("Error connecting to host: " + socket->errorString());
        emit connectFailure(true);
    }
    else
    {
        exec();
    }
}
/**
 * @brief ClientConnector::connected : performs the file list requesting operation on client connection.
 *              Writes a file list request to the socket.
 */
void ClientConnector::connected()
{
    emit appendStatus("Connected to " + socket->peerName());

    socket->write(FLISTREQ);
}

/**
 * @brief ClientConnector::disconnected : disconnects the client from the server.
 *          Calls a delete later on the socket and exits the thread, so that the
 *          thread may handle the disconnection.
 */
void ClientConnector::disconnectClient()
{
    emit clearFilebox();
    emit appendStatus("Disconnected from server.");
    emit clearSettings(true);
    socket->deleteLater();
    this->exit(0);
}

/**
 * @brief ClientConnector::readyRead : waits for incoming data, immediately called when data arrives.
 *          When data is found, checks the incoming data for a header.
 *          Headers may either be FLISTREQ (a filelist packet) or FRECIEVE
 *          (a data packet for file saving.)
 */
void ClientConnector::readyRead()
{
    QByteArray Data = socket->read(HEADER_SIZE);

    if(Data.startsWith(FLISTREQ)) // FILE LIST REQUEST PACKET
    {
       processFileList();
    }
    else if(Data.startsWith(FRECIEVE)) // INCOMING GET PACKET
    {
       grabRequestFile();
    }
}

/**
 * @brief ClientConnector::sendFile : slot called when user hits the sendfile button.
 *          Grabs the file the user selected and sends its header and data to the server.
 * @param filepath - the filepath of the selected file provided by the signal calling it.
 */
void ClientConnector::sendFile(QString filepath)
{
    disconnectFileList(OFF);

    QByteArray * data = new QByteArray();
    QDataStream out(data, QIODevice::WriteOnly);

    QFileInfo fi(filepath);
    QString filename = fi.fileName();

    QFile * file = new QFile(filepath);
    file->open(QIODevice::ReadOnly);
    emit appendStatus("Sending " + filename + " to server...");

    out.writeRawData(FSNDREQ, strlen(FSNDREQ)); // writes the GET header
    out.writeBytes(filename.toStdString().c_str(), strlen(filename.toStdString().c_str())); // writes the filename and its size

    socket->write(*data);
    data->clear();
    out.device()->seek(0);

    out.writeBytes(file->readAll(), file->size()); // writes both filesize and data

    socket->write(*data);
    socket->flush();

    emit appendStatus("Send completed.");
    disconnectFileList(ON);
}
/**
 * @brief ClientConnector::disconnectFileList : enables or disables the fielisting.
 *          Prevents the user from causing uneeded file transfer issues when an ongoing transfer is occuring.
 * @param state - 1 means the file listing will be enabled, 0 means it will be disabled.
 */
void ClientConnector::disconnectFileList(int state)
{
    if(state == OFF)
        disconnect(ui->fileBox, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(sendRequestPacket(QListWidgetItem *)));
    else if(state == ON)
        connect(ui->fileBox, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(sendRequestPacket(QListWidgetItem *)));
}

/**
 * @brief ClientConnector::requestFile : grabs the incoming file from the server.
 *          Collects the file size and header data. Saves the file to the established directory.
 */
void ClientConnector::grabRequestFile()
{
    QByteArray  * fileData = new QByteArray();

    disconnectFileList(OFF);

    QString filename = readFilenameHeader();
    quint32 size = grabFileSize();

    int totalBytesRead = 0;
    emit appendStatus("Waiting for file request...");
    emit appendStatus(QString("Retrieving " + filename + "\nFilesize: %1").arg((double)size/1000000).append(" MB."));
    emit setRangeProgress(size/100, size);

    while(totalBytesRead != size)
    {
        if((socket->peek(size).size() == size)||socket->waitForReadyRead()) // if the file size is too small, readyread can miss it
        {
            fileData->append(socket->readAll());
            totalBytesRead += fileData->size() - totalBytesRead;
            emit setValueProgress(totalBytesRead); // progress bar control
        }
    }

    QFile file(directory + "/" + filename);
    file.open(QIODevice::WriteOnly);
    file.write(*fileData);
    file.close();

    emit resetProgress();
    disconnectFileList(ON);
    emit appendStatus("File transfer completed.");
}
/**
 * @brief ClientConnector::readFilenameHeader : reads the incoming filename header.
 *          Called when accepting a requested file.
 * @return A QString containing the filename grabbed from the incoming header.
 */
QString ClientConnector::readFilenameHeader()
{
    QByteArray fileData;
    QDataStream in(&fileData, QIODevice::ReadWrite);

    while((fileData = socket->read(sizeof(quint32))).size() != sizeof(quint32)){} // reads size of filename

    quint32 size;
    in >> size;
    fileData.clear();
    in.device()->seek(0);

    while((fileData = socket->read(size)).size() != size){} // reads in filename

    QString filename = fileData;
    return filename;
}
/**
 * @brief ClientConnector::grabFileSize : grabs the incoming filesize from the file header.
 *          Called when accepting a request.
 * @return A quint32 which represents the size of the incoming data.
 */
quint32 ClientConnector::grabFileSize()
{
    QByteArray fileData;
    QDataStream in(&fileData, QIODevice::ReadWrite);
    quint32 size = 0;

    while((fileData = socket->read(sizeof(quint32))).size() != sizeof(quint32)){}

    in >> size;

    return size;
}
/**
 * @brief ClientConnector::processFileList : processes the file listing packet sent by the server.
 *          Retrieves the size of the list first and then reads in the filelisting. Emits a signal
 *          to the GUI thread to draw to the file listing box.
 */
void ClientConnector::processFileList()
{
    QByteArray data;
    QDataStream in(&data, QIODevice::ReadOnly);

    while((data = socket->read(sizeof(quint32))).size() != sizeof(quint32)){}

    quint32 size;
    in >> size;
    data.clear();

    while((data = socket->read(size)).size() != size){}

    QList<QByteArray> fileList = data.split(',');

    emit clearFilebox();

    for(int i = 0; i < fileList.size(); i++)
    {
        emit addListItem(fileList.at(i));
    }

}
/**
 * @brief ClientConnector::sendRequestPacket : slot that responds to a user double clicking a file listing item.
 *          Retrieves the filename from the QWidgetItem and sends a file request header along with the filename.
 * @param QListWidget - the widget the user selected for download.
 */
void ClientConnector::sendRequestPacket(QListWidgetItem *item)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    QString filename = item->text();

    out.writeRawData(FGRABREQ, strlen(FGRABREQ));
    out.writeBytes(filename.toStdString().c_str(), strlen(filename.toStdString().c_str()));

    socket->write(data);
    socket->flush();
}
