#ifndef MYCHILD_H
#define MYCHILD_H
#include <QPen>
#include "timer.h"
#include "text.h"
#include <QSlider>
#include <QPixmap>
#include <QObject>
#include <QWidget>
#include <QSpinBox>
#include <QPainter>
#include <QTextEdit>
#include <QKeyEvent>
#include <QScrollBar>
#include <QWheelEvent>
#include <QMouseEvent>
class text;
class Timer;
class MyChild : public QWidget
{
    Q_OBJECT
public:
    bool isUntitled, isModified, fix;       //命名状态，保存状态，文本跟随光标状态
    int paintX, paintY;                     //绘图起始点坐标
    MyChild(bool isNew, QWidget *parent = nullptr);
    ~MyChild();
    //文件
    void newFile();                         //新建操作
    bool save();                            //保存(直接保存后另存为)
    bool saveAs();                          //另存为
    bool saveFile(QString fileName);        //保存文件
    bool loadFile(const QString &fileName); //加载文件
    //编辑
    void cut();                             //剪贴
    void copy();                            //复制
    void redo();                            //重做
    void undo();                            //撤销
    void paste();                           //粘贴
    void search_replace();                  //查找替换
    //字体，字号
    void settextSize(float);                //设置字号
    void settextFamily(QString);            //设置字体

    void drawPix();                         //绘图
    QString userFriendlyCurrentFile();               //提取文件名
    inline QString currentFile() {return curFile;}   //返回当前文件路径
    inline bool hasSelection(){return Text -> hasSelection;}//返回文本选择状态
protected:
    void del();                                     //删除选中区域
    void enter();                                   //插入换行
    void backspace();                               //前向删除
    void insert(QChar);                             //插入换行
    //光标
    void up();                                      //光标上移
    void down();                                    //光标下移
    Cursor locate(QPoint);                          //由鼠标点击位置定位光标
    void locatePos(int&, int&, const Cursor*);      //由光标定位显示位置
    //滚轮
    void createScroll();                            //创建滚动条
    void updateScrollSize();                        //更新滚动条位置
    //事件
    void paintEvent(QPaintEvent *);                 //画图事件
    void wheelEvent(QWheelEvent *e);                //滚轮事件
    void keyPressEvent(QKeyEvent *k);               //键盘事件
    void resizeEvent(QResizeEvent *e);              //窗口大小改变事件
    void closeEvent(QCloseEvent *event);            //关闭事件
    void mouseMoveEvent(QMouseEvent *e);            //鼠标移动
    void mouseDoubleClickEvent(QMouseEvent* event);//双击事件
    void mousePressEvent(QMouseEvent *e);           //鼠标点击
    void mouseReleaseEvent(QMouseEvent *e);         //鼠标释放
private slots:
    void search(QString);                   //查找
    void find_next(QString);                //查找下一个
    void updateFix();                       //更新跟随光标状态
    void updateScrollValue(int, int);       //更新滚动条最大值
    void replace_one(QString, QString);     //单个替换
    void replace_all(QString, QString);     //全部替换
    void VscrollChange(int);                            //竖直滚动条位置更改槽函数
    void HscrollChange(int);                            //水平滚动条位置更改槽函数
    void documentWasModified();                         //文档被修改时，窗口显示更改状态标识
private:
    text *Text;
    Timer *timer;                     //定时器
    QPixmap *pix;                     //画布
    int textSize;                     //字号
    int VscrollMax;                   //滚动条最大值
    int HscrollMax;
    QString curFile;                  //保存当前文件路径
    QString textFamily;               //字体
    QScrollBar *Vscroll;              //竖直滚动条
    QScrollBar *Hscroll;              //水平滚动条
    void preUpdate();                 //预处理绘画的起始点,实现光标超出屏幕范围时自行滚屏
    bool maybeSave();                                   //提醒保存文件操作
    void setCurrentFile(const QString &fileName);       //设置当前文件的属性
    QString strippedName(const QString &fullFileName);  //获取较短的绝对路径
signals:
    Changed();                        //文本更改信号
    selectText(bool);                 //选中信号
    postnum(int);                     //发送行或文本信息（字符数
    postFindStr(QString);
};
#endif // MYCHILD_H
