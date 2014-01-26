#include "vkhtmlparse.h"
#include <QFile>
#include <QTextCodec>

VKHtmlParse::VKHtmlParse(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<SongStruct>("SongStruct");
}

void VKHtmlParse::setPath(QString path)
{
    htmlPath = path;
}

void VKHtmlParse::parse()
{
    QFile f(htmlPath);
    f.open(QFile::ReadOnly);
    QByteArray htmlData(f.readAll());
    f.close();

    QString htmlText = QTextCodec::codecForHtml(htmlData)->toUnicode(htmlData);

    HTML::ParserDom parser;
    tree<HTML::Node> dom = parser.parseTree(htmlText.toStdString());

    // root
    tree<HTML::Node>::iterator end = dom.end();
    tree<HTML::Node>::iterator it = dom.begin();

    // find first <div class="audio  fl_l">
    while (it != end)
    {
        if(checkTag(it,"div","class","audio  fl_l"))
        {
            // parse only <div class="audio  fl_l"> contents
            SongStruct s;
            tree<HTML::Node> song_info(it);
            tree<HTML::Node>::iterator song_end = song_info.end();
            tree<HTML::Node>::iterator song = song_info.begin();
            while (song != song_end)
            {
                if (checkTag(song,"input","type","hidden"))
                {
                    s.link = directLink(
                                QString::fromStdString(song->attribute("value").second));
                }
                else if (checkTag(song,"span","class","title"))
                {
                    // one child for lyrics
                    song += (song.number_of_children() - 1);
                    s.title = fromHtmlEscaped(
                                QString::fromStdString(tagContent(song_info,song)));
                }
                else if (checkTag(song,"a","onclick"))
                {
                    // find by 'onclick' attribute, because it used only for artist
                    s.artist = fromHtmlEscaped(
                                QString::fromStdString(tagContent(song_info,song)));
                }
                song++;
            }
            emit found(s);
        }
        it++;
    }
    emit finished();
}

std::string VKHtmlParse::tagContent(tree<HTML::Node> const &dom, tree<HTML::Node>::iterator const &parent)
{
    std::string result;
    for (unsigned int i = 0; i < dom.number_of_children(parent); i++)
    {
        tree<HTML::Node>::iterator it = dom.child(parent,i);
        if (!it->isTag() && !it->isComment())
            result += it->text();
    }
    return result;
}

bool VKHtmlParse::checkTag(const tree<HTML::Node>::iterator &it,
                           std::string tag,
                           std::string att,
                           std::string val)
{
    if (it->tagName() == tag)
    {
        it->parseAttributes();
        std::pair<bool, std::string> att_pair = it->attribute(att);
        if (att_pair.first &&
                ((att_pair.second == val) || val.empty()))
        {
            return true;
        }
    }
    return false;
}
