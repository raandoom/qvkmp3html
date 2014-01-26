#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "vkhtmlparse.h"
#include "filedownloader.h"
#include <QDropEvent>
#include <QMimeData>
#include <QDir>
#include <QThread>
#include <QDebug>

// column names for table
enum TC {
    Check,
    Artist,
    Title,
    Status
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::MainWindow *ui;
    QThread thread;             // thread for parser
    VKHtmlParse parser;         // html parser
    FileDownloader* fdl;        // class for downloading list of files
    QList<SongStruct> songs;    // all songs in table
    bool downloadActive;        // is download in process

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void dropEvent(QDropEvent *ev)
    {
        QString path = QDir::toNativeSeparators(ev->mimeData()->urls().at(0).toLocalFile());
        if (QFileInfo(path).isFile())
        {
            // if file - use as html file for parsing
            ui->pathLine->setText(path);
            findAudio(path);
        }
        else if (QFileInfo(path).isDir())
        {
            // if folder - use as folder for saving files
            ui->saveLine->setText(path);
            ui->start->setEnabled(true);
            checkSongsExist();
        }
    }

    void dragEnterEvent(QDragEnterEvent *ev)
    {
        QUrl f = ev->mimeData()->urls().at(0);
        // accept grag only one item
        // and it is file ends with '.htm(l)'
        // or it is folder
        if (ev->mimeData()->urls().size() == 1)
        {
            if ((QFileInfo(f.toLocalFile()).isFile() &&
                 f.fileName().contains(QRegExp("([.][hH][tT][mM][lL]?)$")))
                    ||
                    QFileInfo(f.toLocalFile()).isDir())
                ev->acceptProposedAction();
        }
    }

private slots:
    void browse();
    void browseSave();
    void findAudio(QString htmlPath);
    void addSong(SongStruct);

    void checkSongs(bool);
    void checkSongsExist();

    void download();
    void downloaded(QUrl link, bool ok, QString err);
    void downloadFinished();
    void progress(QUrl link, qint64 rcv, qint64 total);

    int songIndexByLink(QList<SongStruct> &s, QString link)
    {
        for (int i = 0; i < s.size(); i++)
            if (s.at(i).link == link)
                return i;
        return -1;
    }

    void enableCheckboxes(bool en)
    {
        ui->checkAll->setEnabled(en);
        for (int i = 0; i < ui->audioTable->rowCount(); i++)
        {
            QTableWidgetItem* chb = tableItem(ui->audioTable,i,Check);
            if (en)
                chb->setFlags(chb->flags() | Qt::ItemIsEnabled);
            else
                chb->setFlags(chb->flags() ^ Qt::ItemIsEnabled);
        }
    }
};

#endif // MAINWINDOW_H
