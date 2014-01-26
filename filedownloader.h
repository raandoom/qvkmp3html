#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>

class FileDownloader : public QObject
{
    Q_OBJECT

    QNetworkAccessManager nam;
    QNetworkReply* reply;       // (only one reply at time)
    QStringList l;              // list with links for downloading
    QStringList p;              // list with paths for saving
    QFile file;                 // (only one file at time)
    bool enabled;               // false for abort downloadNext()

public:
    explicit FileDownloader(QStringList links, QStringList paths, QObject *parent = 0);
    ~FileDownloader();

signals:
    void progress(QUrl link, qint64 rcv, qint64 total);
    void downloaded(QUrl link, bool ok, QString err);
    void finishedAll();

private slots:
    void downloadNext();
    void finishedOne(QNetworkReply*);
    void progressOne(qint64 rcv, qint64 total);
    void saveData();
};

#endif // FILEDOWNLOADER_H
