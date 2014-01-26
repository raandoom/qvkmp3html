#ifndef USEFUL_FUNCS_H
#define USEFUL_FUNCS_H

#include <QTableWidgetItem>
#include <QDir>

QTableWidgetItem* tableItem(QTableWidget* t, int r, int c);
int tableFirstCheckedRow(QTableWidget* t, int c);
QString fromHtmlEscaped(QString t);
QString directLink(QString t);
QString songPath(QString path, QString artist, QString title);
bool fileExist(QString path);

#endif // USEFUL_FUNCS_H
