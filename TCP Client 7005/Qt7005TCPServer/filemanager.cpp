#include "filemanager.h"

FileManager::FileManager(QObject *parent) :
    QObject(parent)
{
}

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

QStringList FileManager::grabFileListing()
{
    return fileListing;
}

void FileManager::setDirectory(QString dir)
{
    directory = QDir(dir);
    loadFileListing();
}

void FileManager::loadFileListing()
{
    fileListing = directory.entryList();
}
