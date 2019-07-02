#ifndef TEXT_H
#define TEXT_H
#include <QChar>
#include <QPoint>
#include <QColor>
#include <vector>
#include <QFile>
const int MIN = 30;
const int MAX = 100; //新块大小

class Cursor;
//块
class block{
public:
    int size;                             //本块字符个数
    bool select[MAX];                     //是否被选择
    QChar *word;                          //字符存储区域
    block *preblock, *nextblock;          //前一块，后一块

    block(int, block*, block*);
    ~block();
    void merge(block*, block*, Cursor *); //合并任意两块
    void link(block*, block*);            //连接任意两块
    void split(int, Cursor*);             //分裂
    void del();                           //删除
};

//块状链表
class line{
public:
    int sum;                    //本行字数
    line *preline, *nextline;   //前一行，后一行
    block *firstblock;          //本行第一个块

    line(line*, line*);
    ~line();

    void del();                 //删除本行（不释放内存）
    void balance(Cursor *);     //重塑本行链表，优化效率
};

//光标
class Cursor
{
public:
    int n;                          //光标在块中的位置
    line *linecur;                  //光标所在的链表头结点指针
    block *blockcur;                //光标所在的块结点指针

    Cursor(int, line*, block*);
    ~Cursor();

    void pre();                     //光标前移
    bool next();                    //光标后移
    void set(Cursor);               //设置光标位置
    void set(int, line*, block*);   //设置光标位置
};
class selectcur
{
public:
    Cursor start, end;
    selectcur(Cursor, Cursor);
};
//文本
class text{
public:
    line *firstline;                //第一行
    Cursor *cursor;                 //光标位置
    Cursor *start, *end;            //鼠标选择区域，首尾
    bool hasSelection;              //是否有选择区域
    int findPos;                    //当前选择的目标串
    std::vector<selectcur> sltcur;  //查找结果保存数组
    text(bool);
    ~text();

    void load(QFile&);              //加载文件到块状链表中

    void enter();                   //换行
    void delet();                   //后向删除
    void backspace();               //前向删除
    void insert(QChar);             //插入单个字符

    void cut();                     //剪贴
    void del();                     //删除选中区域
    void copy();                    //复制
    void paste();                   //粘贴
    void search(QString);           //查找（高亮）
    void find_next(QString);        //查找下一个
    void replace_all(QString, QString); //全部替换
    void replace_one(QString, QString); //替换当前

    bool trans();                   //调整选择区域首尾光标
    void showFind();                //标记查找结果，画图时高亮显示
    void hideFind();                //清除查找结果标记
    void setSelect();               //设置选择文本
    void hideSelect();              //隐藏选择文本
    void clearSelect();             //清除选择文本
    int  comparecur();              //比较选择区域首尾光标
    QString get_selectedText();         //获取选中的字符串
private:
    int size;                       //剪贴板大小
    int sum, lines;                 //文本字符数，行数
    QString clipboard;              //剪贴板
    int account();                  //返回选中区域字符数
    void DEBUG();                   //debug函数
    void delet(int);                //后向删除k个字符
    void insert(int, QChar *);      //插入字符串
};
#endif // TEXT_H
