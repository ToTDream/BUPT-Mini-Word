#include "text.h"
#include <cmath>
#include <QDebug>
#include <QStringList>
#include <cstring>
#include <QTextCodec>
#include <queue>
using namespace std;
extern text *Text;
// class block
block::block(int n = 0, block *pre = nullptr, block *next = nullptr)
{
    size = n;
    preblock = pre;
    nextblock = next;
    word  = new QChar[MAX];
    memset(select, 0, sizeof(select));
}
void block::merge(block *a, block *b, Cursor *cursor)
{
    if(!b)return;
    for(int i = 1; i <= b -> size; i ++)a -> word[a -> size + i] = b -> word[i];
    if(cursor -> blockcur == b)cursor -> blockcur = a, cursor -> n += a -> size;
    a -> size += b -> size;
    a -> nextblock = b;
    b -> preblock = a;
    b -> del();
}
void block::link(block *a, block *b)
{
    if(a)a -> nextblock = b;
    if(b)b -> preblock = a;
}
void block::split(int pos, Cursor *cursor)
{
    if(pos == size)return;
    block *tb = nextblock;
    nextblock = new block(size - pos, this, tb);
    size = pos;
    for(int i = 1; i <= nextblock -> size; i++)nextblock -> word[i] = word[pos+i];
    link(nextblock, tb);
    if(cursor -> blockcur ==  this && cursor -> n > pos)
        cursor -> blockcur = nextblock, cursor -> n -= size;
}
void block::del()
{
    preblock -> nextblock = nextblock;
    if(nextblock)nextblock -> preblock = preblock;
    delete this;
}
block::~block()
{
    delete word;
    preblock = nullptr;
    nextblock = nullptr;
}


//class line
line::line(line* pre = nullptr, line* next = nullptr)
{
    sum = 0;
    preline = pre;
    nextline = next;
    firstblock = new block();
}
void line::balance(Cursor *cursor)
{
    int maxsize = min(MAX, (int)sqrt(sum) << 1);
    int minsize = (int)sqrt(sum) >> 1;
    block *b = firstblock;
    while(b)
    {
        while(b -> size < minsize || b -> size > maxsize)
        {
            if(b -> size < minsize)
            {
                b -> merge(b, b -> nextblock, cursor);
                if(!b -> nextblock)break;
            }
            if(b -> size > maxsize)
                b -> split(b -> size >> 1, cursor);
        }
        b = b -> nextblock;
    }
}
void line::del()
{
    preline = nextline = nullptr;
    firstblock = nullptr;
}
line::~line()
{
    delete firstblock;preline = nextline = nullptr;
}


//class Cursor
Cursor::Cursor(int _n = 0, line* lp = nullptr, block *bp = nullptr)
{
    n = _n, linecur = lp, blockcur = bp;
}
void Cursor::pre()
{
    if(n){n--; return;}
    if(blockcur -> preblock){blockcur = blockcur -> preblock; n = blockcur -> size - 1; return;}
    if(linecur -> preline)
        {
            linecur = linecur -> preline;
            block *bp = linecur -> firstblock;
            while(bp -> nextblock)bp = bp -> nextblock;
            blockcur = bp;
            n = blockcur -> size;
        }
}
bool Cursor::next()
{
    if(n < blockcur -> size)
        {n++; return true;}
    if(blockcur -> nextblock)
        {
        blockcur = blockcur -> nextblock;
        n = blockcur -> size ? 1 : 0; return true;
        }
    if(linecur -> nextline)
        {linecur = linecur -> nextline; blockcur = linecur -> firstblock; n = 0; return true;}
    return false;
}
void Cursor::set(int _n, line *lp, block *bp)
{
    n = _n;
    linecur = lp;
    blockcur = bp;
}
void Cursor::set(Cursor cur)
{
    *this = cur;
}
Cursor::~Cursor()
{
    linecur = nullptr;
    blockcur = nullptr;
}
selectcur::selectcur(Cursor a, Cursor b)
{
    start = a;
    end = b;
}
//class text
text::text(bool isNew)
{
    lines = 0;
    if(isNew)
    {
        firstline = new line;
        cursor = new Cursor(0, firstline, firstline -> firstblock);
    }
    else
    {
        firstline = nullptr;
        cursor = new Cursor;
    }
    sltcur.clear();
    hasSelection = false;
    start  = new Cursor;
    end = new Cursor;
}
void text::load(QFile &file)
{
    QTextStream in(&file);
//    QTextCodec *codec=QTextCodec::codecForName("GBK");
    QTextCodec *codec = QTextCodec::codecForName("utf8");
    QTextCodec::setCodecForLocale(codec);
    in.setCodec(codec);
    if(in.readAll().isEmpty())
    {
        firstline = new line;
        cursor = new Cursor(0, firstline, firstline -> firstblock);
        return;
    }
    bool head = true;
    line *lp = nullptr;
    block *bp = nullptr;
    in.seek(0);
    while(!in.atEnd())
    {
        QString str = codec->fromUnicode(in.readLine());
        if(head)lp = firstline = new line(firstline, nullptr), head = false;
        else lp -> nextline = new line(lp, nullptr), lp = lp -> nextline;
        bp = lp -> firstblock;
        foreach(QChar c, str)
            {
                bp -> word[++(bp -> size)] = c; lp -> sum ++;
                if(bp -> size == MAX)bp = bp -> nextblock = new block(0, bp, nullptr);
            }
        lp -> balance(cursor);
    }
    cursor -> set(bp -> size, lp, bp);

//文件读取结果检查
#ifdef DEBUG
    DEBUG();
#endif
}
void text::enter()
{
    line *newline = new line(cursor -> linecur, cursor -> linecur -> nextline);
    cursor -> linecur -> nextline = newline;
    if(newline -> nextline)newline -> nextline -> preline = newline;

    if(cursor -> blockcur -> size == cursor -> n && !cursor -> blockcur -> nextblock)
    {
        cursor -> linecur = cursor -> linecur -> nextline;
        cursor -> blockcur = cursor -> linecur -> firstblock;
        cursor -> set(0, newline, newline -> firstblock);
        return;
    }
    if(cursor -> blockcur == cursor -> linecur -> firstblock && !cursor -> n)
    {
        block *bp = new block();
        bp -> link(bp, cursor -> linecur -> firstblock);
        cursor -> linecur -> firstblock = bp;
        cursor -> set(0, cursor -> linecur, bp);
    }
    cursor -> blockcur -> split(cursor -> n, cursor);

    delete newline -> firstblock;
    newline -> firstblock = cursor -> blockcur -> nextblock;
    cursor -> blockcur -> nextblock -> preblock = nullptr;
    cursor -> blockcur -> nextblock = nullptr;
    cursor -> set(0, newline, newline -> firstblock);
}
void text::backspace()
{
    if(cursor -> n)
    {
        for(int i = cursor -> n; i < cursor -> blockcur -> size; i++)
            cursor -> blockcur -> word[i] = cursor -> blockcur -> word[i+1];
        cursor -> blockcur -> size --;
        cursor -> n --;
        cursor -> linecur -> sum --;
        return;
    }
    if(cursor -> blockcur -> preblock)
    {
        block *t = cursor -> blockcur;
        cursor -> blockcur = cursor -> blockcur -> preblock;
        cursor -> linecur -> sum --;
        cursor -> n = -- (cursor -> blockcur -> size);
        if(!t -> size)t -> del();
        return;
    }
    if(cursor -> linecur -> preline)
    {
        line *t = cursor -> linecur;
        t -> preline -> nextline = t -> nextline;
        if(t -> nextline)t -> nextline -> preline = t -> preline;
        t -> preline -> sum += t -> sum;
        cursor -> linecur = t -> preline;

        block *bp = t -> preline -> firstblock;
        while(bp -> nextblock)bp = bp -> nextblock;
        if(bp -> size + cursor -> blockcur -> size < MAX)
            bp -> merge(bp, cursor -> blockcur, cursor);
        else
        {
            bp -> link(bp, cursor -> blockcur);
            cursor -> set(bp -> size, t -> preline, bp);
        }
        t -> del();
    }
}

void text::delet()
{
    if(cursor -> next())
        backspace();
}
void text::insert(QChar c)
{
    int maxsize = min(MAX, (int)sqrt(sum+1) << 1);

    if(cursor -> n != cursor -> blockcur -> size)
            cursor -> blockcur -> split(cursor -> n, cursor);
    if(cursor -> blockcur -> size == maxsize)
    {
            block *bp = new block();
            bp -> link(bp, cursor -> blockcur -> nextblock);
            bp -> link(cursor -> blockcur, bp);
            cursor -> set(0, cursor -> linecur, bp);
    }
    cursor -> blockcur -> word[++cursor -> n] = c;
    cursor -> blockcur -> size ++;
    cursor -> linecur -> sum ++;
}

void text::DEBUG()
{
    int Line = 0, Block = 0;
    line* lp = firstline;
    while(lp)
    {
//        qDebug() << "Line" << ++Line << "Linesum:  " << lp -> sum;
        Block = 0;
        block *bp = lp -> firstblock;
        while(bp)
        {
            qDebug() << "Block" << ++Block << "Blocksize:  " << bp -> size;
            for(int i = 0; i <= bp -> size; i++)
            {
                if(i)qDebug() << bp -> word[i];
                if(cursor -> blockcur == bp && cursor -> n == i)
                    qDebug() << "cursor is here";
            }
            bp = bp -> nextblock;
        }
        lp = lp -> nextline;
    }
}

void text::delet(int tot)
{
    int k = tot;
    if(!k) return;
    if(cursor -> n)
    {
        cursor -> blockcur -> split(cursor -> n, cursor);
        cursor -> blockcur = cursor -> blockcur -> nextblock; cursor -> n = 0;
    }
    line *lp = cursor -> linecur;
    while(k && lp)
    {

        while(k > cursor -> blockcur -> size && !cursor -> blockcur -> nextblock)
        {
            k -= cursor -> blockcur -> size;
            block *t = cursor -> blockcur;
            cursor -> blockcur = cursor -> blockcur -> nextblock;
            t -> del();
        //  cursor -> n = 0;
        }
        if(k == cursor -> blockcur -> size)
            cursor -> blockcur -> size = 0, k = 0;
        else if(cursor -> blockcur -> nextblock)
            {
                cursor -> blockcur -> split(k, cursor);
                cursor -> blockcur -> nextblock -> del();
                k = 0;
            }
            else
            {
                k -= cursor -> blockcur -> size;
                cursor -> blockcur -> size = 0;
                cursor -> linecur = lp -> nextline;
                cursor -> blockcur = cursor -> linecur -> firstblock;
            }

        lp -> sum -= tot - k;
        tot -= k;
        lp -> balance(cursor);
        lp = lp -> nextline;
    }
}
int text::account()
{
    int pos = -1;
    line *lp = start -> linecur;
    block *bp = start -> blockcur;
    if(bp == end -> blockcur)
    {
        for(int i = start -> n + 1; i <= end -> n; i++)++pos;
        return pos;

    }
    if(lp == end -> linecur)
    {
        for(int i = start -> n + 1; i <= bp -> size; i++)++pos;

        while(bp = bp -> nextblock, bp != end -> blockcur)
            for(int i = 1; i <= bp -> size; i ++)++pos;

        for(int i = 1; i <= end -> n; i ++)++pos;
        return pos;
    }
    for(int i = start -> n + 1; i <= bp -> size; i++)++pos;

    while(bp = bp -> nextblock, bp)
        for(int i = 1; i <= bp -> size; i ++)++pos;

    ++pos;

    while(lp = lp -> nextline, bp = lp -> firstblock, lp != end -> linecur)
    {
        while(bp)
        {
            for(int i = 1; i <= bp -> size; i ++)++pos;bp = bp -> nextblock;
        }
        ++pos;
    }
    while(bp != end -> blockcur)
    {
        for(int i = 1; i <= bp -> size; i ++)++pos;bp = bp -> nextblock;
    }
    for(int i = 1; i <= end -> n; i ++)++pos;
    return pos;
}
void text::del()
{
    int num = account();
    cursor -> set(*end);
    start -> set(0, nullptr, nullptr);
    end -> set(0, nullptr, nullptr);
    for(int i = 0; i <= num; i++)backspace();
    hasSelection = false;
}
QString text::get_selectedText()
{
    int pos = -1;
    QString _clipboard = "";
    line *lp = start -> linecur;
    block *bp = start -> blockcur;
    if(bp == end -> blockcur)
    {
        for(int i = start -> n + 1; i <= end -> n; i++)_clipboard.append(bp-> word[i]), pos++;
        size = pos;
        return _clipboard;
    }
    if(lp == end -> linecur)
    {
        for(int i = start -> n + 1; i <= bp -> size; i++)_clipboard.append(bp-> word[i]), pos++;

        while(bp = bp -> nextblock, bp != end -> blockcur)
            for(int i = 1; i <= bp -> size; i ++)_clipboard.append(bp-> word[i]), pos++;

        for(int i = 1; i <= end -> n; i ++)_clipboard.append(bp-> word[i]), pos++;
        size = pos;
        return _clipboard;
    }
    for(int i = start -> n + 1; i <= bp -> size; i++)_clipboard.append(bp-> word[i]), pos++;
    while(bp = bp -> nextblock, bp)
        for(int i = 1; i <= bp -> size; i ++)_clipboard.append(bp-> word[i]), pos++;

    _clipboard.append('\n'), pos++;

    while(lp = lp -> nextline, bp = lp -> firstblock, lp != end -> linecur)
    {
        while(bp)
        {
            for(int i = 1; i <= bp -> size; i ++)_clipboard.append(bp-> word[i]), pos++;;bp = bp -> nextblock;
        }
        _clipboard.append('\n'), pos++;
    }
    while(bp != end -> blockcur)
    {
        for(int i = 1; i <= bp -> size; i ++)_clipboard.append(bp-> word[i]), pos++;;bp = bp -> nextblock;
    }
    for(int i = 1; i <= end -> n; i ++)_clipboard.append(bp-> word[i]), pos++;;
    size = pos;
    return _clipboard;
}
void text::copy()
{
    clipboard = get_selectedText();
}
void text::cut()
{
    copy();
    cursor -> set(*end);
    start -> set(0, nullptr, nullptr);
    end -> set(0, nullptr, nullptr);
    for(int i = 0; i <= size; i++)backspace();
    hasSelection = false;
}

void text::paste()
{
    for(int i = 0; i <= size; i++)
        if(clipboard[i] != '\xa')
            insert(clipboard[i]);
        else
            enter();
    hasSelection = false;
}

void text::search(QString str)
{
    findPos = -1;
    sltcur.clear();
    line *lp = firstline;
    Cursor cur;
    int len = str.length();
    while(lp)
    {
        bool ok = false;
        int pos = 0;
        block *bp = lp -> firstblock;
        while(bp)
        {
            for(int i = 1; i <= bp -> size; i++)
            {
                if(bp -> word[i] == str[pos])
                {
                    pos++;
                    if(!ok){cur.set(i-1, lp, bp);ok = true;}
                    if(pos == len){sltcur.push_back(selectcur(cur, Cursor(i, lp, bp)));pos = 0;}
                }
                else
                    ok = false, pos = 0;
            }
            bp = bp -> nextblock;
        }
        lp = lp -> nextline;
    }
}

void text::replace_all(QString s, QString t)
{
    qDebug() << "comparecur";
    search(s);
    int slen = s.length(), tlen = t.length();
    foreach(selectcur one, sltcur)
    {
        cursor -> set(one.end);
        for(int i = 1; i <= slen; i++)backspace();
        for(int i = 1; i <= tlen; i++)insert(t[i-1]);
        one.start.linecur -> balance(cursor);
    }
}

void text::replace_one(QString s, QString t)
{
    if(sltcur.size() && ~findPos)
    {
        selectcur cur = sltcur[findPos];
        int slen = s.length(), tlen = t.length();
        cursor -> set(cur.end);
        for(int i = 1; i <= slen; i++)backspace();
        for(int i = 1; i <= tlen; i++)insert(t[i-1]);
        cur.start.linecur -> balance(cursor);
        std::vector<selectcur>::iterator it = sltcur.begin();
        for(int i = 0; i < findPos; i++)it++;
        sltcur.erase(it);
    }
}
void text::find_next(QString s)
{
    findPos = (findPos + 1) % sltcur.size();
    selectcur cur = sltcur[findPos];
    cursor -> set(cur.start);
}
int text::comparecur()
{
    if(start -> blockcur == end -> blockcur)return start -> n < end -> n ? 1 : start -> n == end -> n ? 0 : -1;
    if(start -> linecur == end -> linecur)
    {
        block *bp = start->blockcur;
        while(bp && bp -> nextblock != end -> blockcur)bp = bp -> nextblock;
        return bp ? 1 : -1;

    }
    line *lp = start -> linecur;
    while(lp && lp -> nextline != end -> linecur)lp = lp -> nextline;
    return lp ? 1 : -1;
}
bool text::trans()
{
    int flag = comparecur();
    if(flag == -1)
    {
        Cursor t = *start;
        start -> set(*end);
        end -> set(t);
    }
    cursor -> set(*end);
    return flag;
}
void text::setSelect()
{
    if(start -> blockcur)start -> blockcur -> select[start -> n] = true;
    if(end -> blockcur)end -> blockcur -> select[end -> n] = true;
}
void text::hideSelect()
{
    if(start -> blockcur)start -> blockcur -> select[start -> n] = false;
    if(end -> blockcur)end -> blockcur -> select[end -> n] = false;
}
void text::clearSelect()
{
    if(start -> blockcur)start -> set(0, firstline, firstline -> firstblock);
    if(end -> blockcur)end -> set(0, firstline, firstline -> firstblock);
}
void text::showFind()
{
    if(~findPos)
    {
        selectcur cur = sltcur[findPos];
        cur.start.blockcur->select[cur.start.n] = true;
        cur.end.blockcur->select[cur.end.n] = true;
    }
    else
    {
        foreach(selectcur a, sltcur)
        {
            a.start.blockcur->select[a.start.n] = !a.start.blockcur->select[a.start.n];
            a.end.blockcur->select[a.end.n] = !a.end.blockcur->select[a.end.n];
        }
    }
}
void text::hideFind()
{
    foreach(selectcur a, sltcur)
    {
        a.start.blockcur->select[a.start.n] = false;
        a.end.blockcur->select[a.end.n] = false;
    }
}
text::~text()
{
    delete cursor;
    delete start;
    delete end;
    line* lp = firstline;
    while(lp)
    {
        block *bp = lp -> firstblock;
        while(bp)
        {
            block *tp = bp -> nextblock;
            delete bp;
            bp = tp;
        }
        lp = lp -> nextline;
    }

}

