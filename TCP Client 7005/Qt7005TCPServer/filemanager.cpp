#include "filemanager.h"

FileManager::FileManager(QString dir, QObject *parent) :
    QObject(parent)
{
    directory(dir);
    loadFileListing();
}

int FileManager::doesFileExist(QString fname)
{
    for(int i = 0; i < fileListing; i++)
    {
        if(fileListing.at(i) == fname)
        {
            return 1;
        }
    }

    return 0;
}

QFile FileManager::grabFileHandle(QString fname)
{
    for(int i = 0; i < fileListing; i++)
    {
        if(fileListing.at(i) == fname)
        {
            QFile file(directory.absolutePath() + "/" + fname);
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
}

void loadFileListing()
{
    fileListing = directory.entryList;
}
