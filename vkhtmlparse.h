#ifndef VKHTMLPARSE_H
#define VKHTMLPARSE_H

#include "htmlcxx/ParserDom.h"
#include "useful_funcs.h"
#include <QObject>
#include <QMetaType>
#include <QDebug>

struct SongStruct {
    QString artist;
    QString title;
    QString link;
};
Q_DECLARE_METATYPE(SongStruct)

using namespace htmlcxx;

class VKHtmlParse : public QObject
{
    Q_OBJECT

    QString htmlPath;

public:
    explicit VKHtmlParse(QObject *parent = 0);
    void setPath(QString path);

signals:
    void found(SongStruct);
    void finished();

public slots:
    void parse();

private slots:
    bool checkTag(const tree<HTML::Node>::iterator &it,
                  std::string tag,
                  std::string att,
                  std::string val = std::string());
    std::string tagContent(tree<HTML::Node> const &dom,
                           tree<HTML::Node>::iterator const &parent);
};

#endif // VKHTMLPARSE_H
