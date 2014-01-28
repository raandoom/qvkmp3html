#include "mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    fdl(0),
    downloadActive(false)
{
    ui->setupUi(this);
    setAcceptDrops(true);

    parser.moveToThread(&thread);
    connect(&thread,SIGNAL(started()),&parser,SLOT(parse()));
    connect(&parser,SIGNAL(found(SongStruct)),this,SLOT(addSong(SongStruct)));
    connect(&parser,SIGNAL(finished()),&thread,SLOT(quit()));
    connect(&parser,SIGNAL(finished()),this,SLOT(checkSongsExist()));

    ui->pathLine->setFrame(false);
    connect(ui->pathBrowse,SIGNAL(clicked()),this,SLOT(browse()));

    ui->saveLine->setFrame(false);
    connect(ui->saveBrowse,SIGNAL(clicked()),this,SLOT(browseSave()));

    connect(ui->checkAll,SIGNAL(clicked(bool)),this,SLOT(checkSongs(bool)));

    ui->audioTable->horizontalHeader()->
#if QT_VERSION >= 0x050000
            setSectionResizeMode(Check,QHeaderView::Fixed);
#else
            setResizeMode(Check,QHeaderView::Fixed);
#endif

    ui->audioTable->resizeColumnToContents(Check);

    ui->start->setDisabled(true);
    connect(ui->start,SIGNAL(clicked()),this,SLOT(download()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::browse()
{
    QString path = QFileDialog::getOpenFileName(0,
                                                "Choose saved HTML file:",
                                            #ifdef Q_OS_WIN
                                                QDir::rootPath(),
                                            #endif
                                            #ifdef Q_OS_UNIX
                                                QDir::homePath(),
                                            #endif
                                                "Web page (*.html *.htm)"
                                                );
    if (!path.isEmpty())
    {
        ui->pathLine->setText(QDir::toNativeSeparators(path));
        findAudio(path);
    }
}

void MainWindow::browseSave()
{
    QString path = QFileDialog::getExistingDirectory(0,
                                                     "Choose download path:",
                                                 #ifdef Q_OS_WIN
                                                     QDir::rootPath()
                                                 #endif
                                                 #ifdef Q_OS_UNIX
                                                     QDir::homePath()
                                                 #endif
                                                     );
    if (!path.isEmpty())
    {
        ui->saveLine->setText(QDir::toNativeSeparators(path));
        ui->start->setEnabled(true);
        checkSongsExist();
    }
}

void MainWindow::findAudio(QString htmlPath)
{
    ui->progress->setValue(0);
    ui->progress->setProperty("step",0);
    ui->audioTable->setRowCount(0);
    songs.clear();

    parser.setPath(htmlPath);
    thread.start();
}

void MainWindow::addSong(SongStruct s)
{
    int row = ui->audioTable->rowCount();
    ui->audioTable->insertRow(row);

    songs.append(s);

    Qt::CheckState cs = Qt::Checked;
    if (fileExist(songPath(ui->saveLine->text(),s.artist,s.title)))
        cs = Qt::Unchecked;
    tableItem(ui->audioTable,row,Check)->setCheckState(cs);
    tableItem(ui->audioTable,row,Artist)->setText(s.artist);
    tableItem(ui->audioTable,row,Title)->setText(s.title);
}

void MainWindow::checkSongs(bool state)
{
    Qt::CheckState cs = Qt::Unchecked;
    if (state)
        cs = Qt::Checked;

    for (int i = 0; i < ui->audioTable->rowCount(); i++)
    {
        tableItem(ui->audioTable,i,Check)->setCheckState(cs);
    }
}

void MainWindow::checkSongsExist()
{
    QString p = ui->saveLine->text();
    if (!p.isEmpty())
    {
        for (int i = 0; i < songs.size(); i++)
        {
            Qt::CheckState cs = Qt::Checked;
            if (fileExist(songPath(p,songs.at(i).artist,songs.at(i).title)))
                cs = Qt::Unchecked;
            tableItem(ui->audioTable,i,Check)->setCheckState(cs);
        }
    }
}

void MainWindow::download()
{
    downloadActive = !downloadActive;
    if (!downloadActive) // if something downloading - stop it
    {
        ui->start->setText("Download");

        delete fdl;
        fdl = 0;

        ui->pathBrowse->setEnabled(true);
        ui->saveBrowse->setEnabled(true);
        enableCheckboxes(true);
        checkSongsExist();
    }
    else                // if nothing downloading - start it
    {
        // find first checked row
        int row = tableFirstCheckedRow(ui->audioTable,Check);
        if (row != -1)
        {
            ui->start->setText("Cancel");
            ui->pathBrowse->setDisabled(true);
            ui->saveBrowse->setDisabled(true);
            enableCheckboxes(false);

            QStringList links;
            QStringList paths;
            for (int i = 0; i < songs.size(); i++)
            {
                if (tableItem(ui->audioTable,i,Check)->checkState() == Qt::Checked)
                {
                    links.append(songs.at(i).link);
                    paths.append(songPath(ui->saveLine->text(),
                                          songs.at(i).artist,
                                          songs.at(i).title));
                }
            }

            ui->progress->setMaximum(links.size() * 100);

            fdl = new FileDownloader(links,paths,this);
            connect(fdl,SIGNAL(downloaded(QUrl,bool,QString)),this,SLOT(downloaded(QUrl,bool,QString)));
            connect(fdl,SIGNAL(finishedAll()),this,SLOT(downloadFinished()));
            connect(fdl,SIGNAL(progress(QUrl,qint64,qint64)),this,SLOT(progress(QUrl,qint64,qint64)));
        }
    }
}

void MainWindow::downloaded(QUrl link, bool ok, QString err)
{
    int step = ui->progress->property("step").toInt() + 1;
    ui->progress->setProperty("step",step);

    int row = songIndexByLink(songs,link.toString());
    if (row != -1)
    {
        QTableWidgetItem* item = tableItem(ui->audioTable,row,Status);
        if (ok)
            item->setText("Downloaded");
        else
            item->setText(err);
    }
}

void MainWindow::downloadFinished()
{
    if (downloadActive)
        download();
}

void MainWindow::progress(QUrl link, qint64 rcv, qint64 total)
{
    double part = double(rcv)/total;

    int step = ui->progress->property("step").toInt();
    ui->progress->setValue((step + part) * 100);

    int row = songIndexByLink(songs,link.toString());
    if (row != -1)
    {
        ui->songName->setText(songs.at(row).artist + " - " + songs.at(row).title);
        ui->songProgress->setValue(part * 100);
    }
}
