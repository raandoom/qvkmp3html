#include "useful_funcs.h"

QTableWidgetItem* tableItem(QTableWidget* t, int r, int c)
{
    QTableWidgetItem* item = t->item(r,c);
    if (item == 0)
    {
        item = new QTableWidgetItem;
        t->setItem(r,c,item);
    }
    return item;
}

int tableFirstCheckedRow(QTableWidget* t, int c)
{
    for (int i = 0; i < t->rowCount(); i++)
    {
        QTableWidgetItem* item = 0;
        if ((item = tableItem(t,i,c))->checkState() == Qt::Checked)
        {
            return i;
        }
    }
    return -1;
}

QString fromHtmlEscaped(QString t)
{
    t.replace("&lt;","<");      // "&lt;"  represents the < sign.
    t.replace("&gt;",">");      // "&gt;"  represents the > sign.
    t.replace("&amp;","&");     // "&amp;" represents the & sign.
    t.replace("&quot;","\"");   // "&quot; represents the " mark.

    QRegExp rx("&#([xX][0-9a-fA-F]+|\\d+);");
    int pos = 0;
    while ((pos = rx.indexIn(t,pos)) != -1)
    {
        QChar ch;
        if (rx.cap(1).at(0).isDigit())  // if digit - not hex
            ch = QChar(rx.cap(1).toInt(0,10));
        else                            // hex
            ch = QChar(rx.cap(1).mid(1).toInt(0,16));
        t.replace(pos,rx.matchedLength(),ch);
        pos += rx.matchedLength();
    }

    return t;
}

QString directLink(QString t)
{
    return t.left(QRegExp("[?]").indexIn(t));
}

QString songPath(QString path, QString artist, QString title)
{
    if (path.isEmpty())
        return path;
    // need to replace windows special characters: \ / : * ? " < > |
    QRegExp rx("[\\\\/:*\?\"<>|]");
    return QDir::toNativeSeparators(path + "/" +
                                    artist.simplified().remove(rx) +
                                    " - " +
                                    title.simplified().remove(rx) +
                                    ".mp3");
}

bool fileExist(QString path)
{
    return (!path.isEmpty() && QFileInfo(path).isFile());
}
