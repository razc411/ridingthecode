#include "filemanager.h"

/**
 * @brief FileManager::FileManager - Constructor for FileManager
 * @param parent - parent of calling QObject
 */
FileManager::FileManager(QString dir, QObject *parent) :
    QObject(parent)
{
    setDirectory(dir);
}

/**
 * @brief FileManager::doesFileExist - checks if the file specified exists.
 * @param fname - the name of the file to be checked for.
 * @return an int of 0 if the file exists, 1 if it does not.
 */
int FileManager::doesFileExist(QString fname)
{
    for(int i = 0; i < fileListing.size(); i++)
    {
        if(fileListing.at(i) == fname)
        {
            return 1;
        }
    }

    return 0;
}
/**
 * @brief FileManager::grabFileHandle - grabs the handle for the requested file.
 * @param fname - the file to grab the handle for.
 * @return the handle of the file requested.
 */
QFile * FileManager::grabFileHandle(QString fname)
{
    for(int i = 0; i < fileListing.size(); i++)
    {
        if(fileListing.at(i) == fname)
        {
            QString dir(directory.absolutePath() + "/" + fname);
            QFile * file = new QFile(dir);
            return file;
        }
    }

    return NULL;
}
/**
 * @brief FileManager::grabFileListing - grabs the file listing for the directory.
 * @return A QStringList of all files in the directory.
 */
QStringList FileManager::grabFileListing()
{
    return fileListing;
}
/**
 * @brief FileManager::setDirectory - sets the directory for the file manager to act on.
 * @param dir - the directory to set the file manager to. ie. "/home/raz/files"
 */
void FileManager::setDirectory(QString dir)
{
    directory = QDir(dir);
    loadFileListing();
}
/**
 * @brief FileManager::loadFileListing - loads file listing.
 *      Retrieves the file listing from the QDir of the file manager in the form of
 *      a QStringList.
 */
void FileManager::loadFileListing()
{
    QStringList temp = directory.entryList();

    for(int i = 0; i < temp.size(); i++)
    {
        if(temp.at(i) == "." || temp.at(i) == "..")
        {
            temp.removeAt(i);
        }
    }

    fileListing = temp;
}
