#include "filedownloader.h"

FileDownloader::FileDownloader(QStringList links, QStringList paths, QObject *parent) :
    QObject(parent),
    reply(0),
    enabled(true)
{
    l = links;
    p = paths;
    connect(&nam,SIGNAL(finished(QNetworkReply*)),this,SLOT(finishedOne(QNetworkReply*)));
    // start download
    downloadNext();
}

FileDownloader::~FileDownloader()
{
    enabled = false;        // abort downloadNext()
    reply->abort();
    reply->deleteLater();

    if (file.isOpen())      // file not closed - this file is not downloaded yet
        file.remove();      // delete this file
}

void FileDownloader::downloadNext()
{
    file.close();
    if (reply)
        reply->deleteLater();

    if (!l.isEmpty() && !p.isEmpty())   // if not all links downloaded
    {
        file.setFileName(p.takeFirst());
        file.open(QFile::WriteOnly);

        reply = nam.get(QNetworkRequest(l.takeFirst()));
        connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(progressOne(qint64,qint64)));
        connect(reply,SIGNAL(readyRead()),this,SLOT(saveData()));
    }
    else                                // if all links downloaded
    {
        emit finishedAll();
    }
}

void FileDownloader::progressOne(qint64 rcv, qint64 total)
{
    emit progress(reply->url(),rcv,total);
}

void FileDownloader::finishedOne(QNetworkReply*)
{
    emit downloaded(reply->url(),
                    reply->error() == QNetworkReply::NoError,
                    reply->errorString());
    if (enabled)    // check if aborted
        downloadNext();
}

void FileDownloader::saveData()
{
    file.write(reply->readAll());
}
