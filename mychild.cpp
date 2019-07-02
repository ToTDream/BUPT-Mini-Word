#include "text.h"
#include <cmath>
#include <QFont>
#include <QPoint>
#include <QBrush>
#include <QDebug>
#include <QWidget>
#include <QObject>
#include "dialog.h"
#include "mychild.h"
#include <QtWidgets>
#include <QMessageBox>
#include <QTextStream>
#include <QInputDialog>
#include <QFontMetrics>

MyChild::MyChild(bool isNew, QWidget *parent)
    : QWidget(parent)
{
    setAttribute (Qt::WA_DeleteOnClose);    //设置在子窗口关闭时销毁这个类的对象
    setMinimumSize(600, 400);
    textSize = 9;                           //初始字号
    textFamily = QString("MS Shell Dlg 2"); //初始字体
    fix = false;                            //跟随光标
    isUntitled = true;                      //未命名
    isModified = false;                     //文本未修改
    Text = new text(isNew);
    pix  = new QPixmap(size());
    pix -> fill(Qt::white);
    timer = new Timer(nullptr, this);
    paintX = textSize * 3, paintY = textSize * 2;
    createScroll();
}
/*
 * 新建文件操作
 * 1、设置窗口编号
 * 2、设置文件未被保存过“isUntitled = true”
 * 3、保存文件路径，为curFile赋初值
 * 4、设置子窗口标题
 * 5、文联文档内容改变信号contentsChanged()至显示文档更改状态标识槽documentWasModified()
 * static 局部变量只被初始化一次，下一次依据上一次的结果值
*/
void MyChild::newFile ()
{
    //设置窗口编号，因为编号会一直被保存，所以需要使用静态变量
    static int sequenceNumber = 1;
    //新建的文档默认未命名
    isUntitled = true;
    //将当前文件命名为“文档+编号”的形式，编号先使用在加1
    curFile = tr("文档 %1").arg (sequenceNumber++);
    //设置窗口标题，使用[*]可以在文档被更改在文件名称后显示“*”号
    setWindowTitle (curFile + "[*]" + tr("- Myself Word"));
    //文档更改时发送contentSChanged()信号，执行documentWasModified()槽函数
    connect (this, SIGNAL(Changed()), this, SLOT(documentWasModified()));
}
/*
 * 加载文件操作步骤
 * 1、打开指定的文件，并读取文件内容到编辑器
 * 2、设置当前文件的setCurrentFile()，该函数可以获取文件路径，完成文件和窗口状态的设置
 * 3、关联文档内容改变信号到显示文档更改状态标志槽documentWasModified()。加载文件操作采用loadFile()函数实现
*/
bool MyChild::loadFile (const QString &fileName)
{
    if(!fileName.isEmpty ())
    {
        if(!QFile::exists (fileName))   //文件不存在
        {
            return false;
        }
        QFile file(fileName);
        if(!file.open (QFile::ReadOnly))
            return false;
        Text -> load(file);             //加载文件
        setCurrentFile (fileName);      //设置文件属性
        connect(this, SIGNAL(Changed()), this, SLOT(documentWasModified()));    //文本修改信号与槽
        drawPix();                      //绘图
    }
    return true;
}
/*
 * 保存文件
 * 1、如果文件没有被保存过(用isUntitled判断)，则执行“另存为”操作saveAs()。
 * 2、否则直接“保存”文件saveFile()，该函数首先打开指定文件，然后将编辑器的内容写入该文件，最后设置当前文件setCurrentFIle()
*/
bool MyChild::save()
{
    isModified = false;
    emit Changed();
    if(isUntitled)
        return saveAs();            //另存为
    return saveFile(curFile);       //直接保存
}
//直接保存文件
bool MyChild::saveFile(QString filename)
{
     if(!filename.endsWith (".txt", Qt::CaseInsensitive))
    {
        filename += ".txt";    //默认保存为txt文档
    }
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
         QMessageBox::warning(this,"file write","can't open",QMessageBox::Yes);
       return false;
    }
    QTextStream out(&file);
    line *lp = Text -> firstline;
    while(lp)
    {
        block *bp = lp -> firstblock;
        while(bp)
        {
            for(int i = 1; i <= bp -> size; i ++)
            {
                out << bp -> word[i];
            }
            bp = bp -> nextblock;
        }
        if(lp -> nextline)out << "\n";
        lp = lp -> nextline;
    }
    out.flush();
    file.close();
    return true;
}
/*
 * 另存为
 * 1、从文件对话框获取文件路径。
 * 2、如果路径不为空，则保存文件saveFile()
*/
bool MyChild::saveAs ()
{
    QString fileName = QFileDialog::getSaveFileName (this, tr("保存为"), curFile, tr("HTML 文档(*.htm *.html); 所有文件(*.*)"));
    //获取文件路径，如果为空则返回false，否则保存文件
    QMessageBox::StandardButton reply= QMessageBox::question(this, "注意", "点击确定修改文件名，反之另存为", QMessageBox::Yes | QMessageBox::No);
    if(reply == QMessageBox::Yes)
    {
        QFile file(curFile);
        if (file.exists())
        {
            file.rename(curFile, fileName);
            setCurrentFile (fileName);
        }
    }
    else
    {
        if(fileName.isEmpty())
            return false;
        return saveFile(fileName);
    }
}

// 提示文件保存及选择操作
bool MyChild::maybeSave ()
{
    //首先判断文档是否被修改过，如果文档被修改过执行if(){}下面的内容
    if(!isModified) return true;//已保存，直接返回
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning
            (
                this,
                tr("Myself Qt Word"),
                tr("文档'%1'已被修改，保存吗?").arg (userFriendlyCurrentFile()),
                QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard
            );
    if(ret == QMessageBox::Save)        return save();//保存
    else if(ret == QMessageBox::Cancel) return false; //取消操作
                                        return true;  //不保存，直接退出
}

//复制
void MyChild::copy()
{
    Text -> copy();
    drawPix();
}

//剪切
void MyChild::cut()
{
    Text -> cut();
    drawPix();
    emit Changed();
}
//粘贴
void MyChild::paste()
{
    if(hasSelection())Text -> del();
    Text -> paste();
    drawPix();
    emit Changed();
}
//查找替换
void MyChild::search_replace()
{
    Dialog *dlg = new Dialog;
    connect(dlg, SIGNAL(send_find(QString)), this, SLOT(search(QString)));
    connect(dlg, SIGNAL(nextone(QString)), this, SLOT(find_next(QString)));
    connect(dlg, SIGNAL(replace_one(QString, QString)), this, SLOT(replace_one(QString, QString)));
    connect(dlg, SIGNAL(replace_all(QString, QString)), this, SLOT(replace_all(QString, QString)));
    dlg->show();
    if(hasSelection())
    {
        connect(this, SIGNAL(postFindStr(QString)), dlg, SLOT(setString(QString)));
        QString str = Text -> get_selectedText();
        Text -> hasSelection = false;
        emit postFindStr(str);
    }
}

//查找
void MyChild::search(QString str)
{
    Text -> search(str);
    drawPix();
}
//全部替换
void MyChild::replace_all(QString s, QString t)
{
    Text -> replace_all(s, t);
    drawPix();
    isModified = true;
    emit Changed();
}
//替换单个
void MyChild::replace_one(QString s, QString t)
{
    Text -> replace_one(s, t);
    drawPix();
    isModified = true;
    emit Changed();
}
//替换多个
void MyChild::find_next(QString s)
{
    Text -> find_next(s);
    drawPix();
    emit Changed();
}
void MyChild::redo()
{
//    Text -> redo();
    drawPix();

}
void MyChild::undo()
{
//    Text -> undo();
    drawPix();
}
//更改字体
void MyChild::settextFamily(QString s)
{
    textFamily = s;
}

//更改字号
void MyChild::settextSize(float size)
{
    int gap = 2 * textSize - paintX;
    textSize = size;
    Hscroll -> setValue(2 * textSize - gap);
}
void MyChild::createScroll()
{
    VscrollMax = 200, HscrollMax = 200;
    Vscroll = new QScrollBar(Qt::Vertical, this);   //滚动条
    Hscroll = new QScrollBar(Qt::Horizontal, this);
    Vscroll -> setMinimum(0);
    Hscroll -> setMinimum(0);
    connect(Vscroll, SIGNAL(valueChanged(int)), this, SLOT(VscrollChange(int)));
    connect(Hscroll, SIGNAL(valueChanged(int)), this, SLOT(HscrollChange(int)));
    connect(Vscroll, SIGNAL(sliderPressed()), this, SLOT(updateFix()));
    connect(Hscroll, SIGNAL(sliderPressed()), this, SLOT(updateFix()));
    updateScrollSize();
    updateScrollValue(0, 0);
    Vscroll -> show();
    Hscroll -> show();
}
void MyChild::updateScrollValue(int x, int y)
{
    VscrollMax += y, HscrollMax += x;
    Vscroll -> setMaximum(VscrollMax);
    Hscroll -> setMaximum(HscrollMax);
}
void MyChild::updateScrollSize()
{
    Vscroll -> move(QPoint(width() - 15, 0));           //竖直滚动条位置为窗口边缘宽为width() - 15,长为15的矩阵
    Vscroll -> setMinimumSize(QSize(15, height() - 15));
    Vscroll -> setMaximumSize(QSize(15, height() - 15));

    Hscroll -> move(QPoint(0, height() - 15));
    Hscroll -> setMinimumSize(QSize(width(), 15));
    Hscroll -> setMaximumSize(QSize(width(), 15));
}

//窗口大小更改事件
void MyChild::resizeEvent(QResizeEvent *e)
{
    // 多窗口大小更改时即时调整滚动条的位置及大小
    if(height() != pix->height() + 15|| width() != pix-> width() + 15)
    {
        pix =  new QPixmap(width() - 15, height() - 15);    //更新画布大小
        updateScrollSize();                                 //更行滚动条位置
        drawPix();                                          //重新画图
    }
    QWidget::resizeEvent(e);
}

void MyChild::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawPixmap(QPoint(0, 0), *pix);                       //将直接画完的画布贴在窗口上
}
//键盘点击事件
void MyChild::keyPressEvent(QKeyEvent *k)
{
    fix = true;                                             //键盘事件下默认跟随光标
    QKeySequence key = k -> key();
    if(key == Qt::Key_Control || key == Qt::Key_Shift)return;
    Text -> sltcur.clear();                                //清空查找结果
    switch(k -> key())
    {
        case Qt::Key_Up: up();break;
        case Qt::Key_Down: down(); break;
        case Qt::Key_Left: Text -> cursor -> pre(); return;
        case Qt::Key_Right: Text -> cursor -> next(); return;
        case Qt::Key_Return: enter();break;
        case Qt::Key_Delete: del(); break;
        case Qt::Key_Backspace: backspace(); break;
        default:
            insert(k -> text()[0]);break;
    }
    isModified = true;                                      //文本内容发送修改
    emit postnum(Text -> cursor -> linecur -> sum);
    emit Changed();                                         //发送修改信号
    drawPix();
}

//滚轮事件
void MyChild::wheelEvent(QWheelEvent *e)
{
    fix = false;                                    //滚轮事件下屏幕默认不跟随光标
    int value = e -> delta();                       //获取滚轮变化值
    if(e->orientation() == Qt::Vertical)
    {
        if(e -> modifiers() == Qt::Key_Control)
        {
            textSize += value;
        }
        else
        {
            int Y = paintY + value;
            Vscroll -> setValue(textSize * 2 - Y);  //通过更新滚动条位置更新起始绘图点纵坐标
        }
    }
}

// 鼠标双击事件
void MyChild::mouseDoubleClickEvent(QMouseEvent* event)
{
    //计算文本总字符数
    int ans = 0;
    line *lp = Text -> firstline;
    while(lp)
    {
        ans += lp -> sum;
        lp = lp -> nextline;
    }
    // 向上层主窗口传送信号
    emit postnum(ans);
}

//鼠标点击事件
void MyChild::mousePressEvent(QMouseEvent *e)
{
    Text -> sltcur.clear();                         //清空查找结果数组
    if(e->button() == Qt::LeftButton)               //点击左键
    {
        Text -> start -> set(locate(e -> pos()));   //更新选择区域
        *(Text -> end) = *(Text -> start);          //选择区域首尾统一
    }
    emit postnum(Text -> cursor -> linecur -> sum); //向上层主窗口传送本行字符数
}

//鼠标移动事件
void MyChild::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() && Qt::LeftButton)              //点击左键
    {
        Text -> end -> set(locate(e -> pos()));     //定位块尾光标
        Text -> hasSelection = Text -> comparecur();//判断是否有选择区域
        emit selectText(Text -> hasSelection);      //更新文本选择状态
        drawPix();                                  //实时更新屏幕
    }
}

//鼠标释放事件
void MyChild::mouseReleaseEvent(QMouseEvent *e)
{
    Text -> end -> set(locate(e -> pos())); //定位块尾光标
    Text -> hasSelection = Text -> trans(); //判断是否有选择区域
    emit selectText(Text -> hasSelection);  //更新文本选择状态
}
//关闭文件事件
void MyChild::closeEvent (QCloseEvent *event)
{
    if(maybeSave()) event->accept (); //已保存，接受该事件，即直接关闭
    else            event->ignore (); //取消操作或保存失败，忽略该事件
}

/*
 * 光标处理
 */

//光标上移
void MyChild::up()
{
    int x, y;
    locatePos(x, y, Text -> cursor);//确定光标所在屏幕位置
    int stepy = textSize * 2;       //行间隔
    if(y < stepy)return;            //若超过屏幕上方，则结束
    y -= stepy+1;                   //否则更新目标位置---上移
    Text -> cursor -> set(locate(QPoint(x, y)));//由新坐标确定新光标位置
}

//光标下移
void MyChild::down()
{
    int x, y;
    locatePos(x, y, Text -> cursor);
    int stepy = textSize * 2;
    y += stepy;
    Text -> cursor -> set(locate(QPoint(x, y)));
}
//确定光标在文本中的位置（行，列，位）
Cursor MyChild::locate(QPoint cur)
{
    int curx = cur.x();         //点击位置横坐标
    int cury = cur.y();         //点击位置纵坐标
    int stepy = textSize * 2;   //行间隔
    int y = paintY;             //绘图起始纵坐标
    QFont font;                 //字体样式
    font.setFamily(textFamily);
    font.setPointSize(textSize);
    line *lp = Text -> firstline;
    block *bp;
    int i;
    while(lp)
    {
        int x = paintX;
        if(cury <= y)
        {
            bp = lp -> firstblock;
            while(bp)
            {
                for(i = 0; i < bp -> size; i ++)
                {
                    QChar pre = bp -> word[i];          //前一个字符
                    QChar next = bp -> word[i+1];       //后一个字符
                    //分别计算宽度
                    int preWidth = QFontMetrics(font).width(pre);
                    int nextWidth = QFontMetrics(font).width(next);
                    if(!i)preWidth = 4;//首位字符初始为4
                    //定位光标为像素位于前后字符半个字符宽度之间的位置
                    if(curx * 1.0 >= x - preWidth/2.0 &&
                     curx * 1.0 <= x + nextWidth/2.0)
                        {
                            return Cursor(i, lp, bp);//返回光标
                        }
                    x += nextWidth;//更新X坐标
                }
                if(bp -> nextblock)bp = bp -> nextblock;
                else break;
            }
            //若（curx, cury)位于该行结尾字符后面，则定位为该行末尾
            return Cursor(i, lp, bp);
        }
        y += stepy;//更新纵坐标
        //若（curx, cury)位于文本结尾字符后面，则定位为该文本末尾
        if(lp -> nextline)lp = lp -> nextline;
        else break;
    }
    bp = lp -> firstblock;
    while(bp -> nextblock)bp = bp -> nextblock;
    return Cursor(bp -> size, lp, bp);
}
//反向确定光标在画布上的位置（像素x, y)
void MyChild::locatePos(int &posX, int &posY, const Cursor* c)
{
    QFont font;
    font.setFamily(textFamily);
    font.setPointSize(textSize);
    line *lp = Text -> firstline;
    int y = paintY, stepy = textSize * 2;
    while(lp)
    {
        int x = paintX;
        block *bp = lp -> firstblock;
        while(bp)
        {
            for(int i = 0; i <= bp -> size; i++)
            {
                if(i)
                {
                    QChar ch = bp -> word[i];
                    int width = QFontMetrics(font).width(ch);
                    x += width;
                }
                if(c -> blockcur == bp && c -> n == i)
                {
                    posX = x, posY = y;
                    return;
                }
            }
            bp = bp -> nextblock;
        }
        y += stepy;
        lp = lp -> nextline;
    }
}
//预处理绘画的起始点,实现光标超出屏幕范围时自行滚屏
void MyChild::preUpdate()
{
    QFont font;
    font.setFamily(textFamily);
    font.setPointSize(textSize);
    line *lp = Text -> firstline;
    int y = paintY, stepy = textSize * 2;
    while(lp)
    {
        int x = paintX;
        block *bp = lp -> firstblock;
        while(bp)
        {
            for(int i = 0; i <= bp -> size; i++)
            {
                if(i)
                {
                    QChar c= bp -> word[i];
                    int wordWidth = QFontMetrics(font).width(c);
                    x += wordWidth;
                }
                if(Text -> cursor -> blockcur == bp && Text -> cursor -> n == i)
                {
                    int X = paintX, Y = paintY;
                    if(x >= width() - 2 * textSize)X -= QFontMetrics(font).width('m');
                    if(y >= height() - stepy)Y -= stepy;
                    if(x <= textSize * 3)X += QFontMetrics(font).width('m');
                    if(y <= stepy)Y += stepy;
                    Vscroll -> setValue(stepy - Y);
                    Hscroll -> setValue(textSize * 2 - X);
                    return;
                }
            }
            bp = bp -> nextblock;
        }
        y += stepy;
        lp = lp -> nextline;
    }
}

//画图
void MyChild::drawPix()
{
    //若光标跟随状态为真，预处理绘图初始坐标（paintX， paintY）
    if(fix)preUpdate();

    pix -> fill(Qt::white);         //画布填充为白色背景

    QPainter painter;               //设置画图工具Qpainter
    painter.begin(pix);             //设置painter绘图载体为画布pix(Qpixmap类型)

    QFont font;                     //设置样式
    font.setFamily(textFamily);     //字体
    font.setPointSize(textSize);    //字号
    painter.setFont(font);          //为painter添加该样式

    int lines = 1;                  //初始行数为1
    bool color = false;             //高亮初始为false
    int y = paintY, stepy = textSize * 2;//绘图起始点坐标
    if(hasSelection())Text -> setSelect();//有选中区域则添加选中标记
    if(!Text -> sltcur.empty())Text -> showFind();//查找结果容器非空，显示查找结果
    line *lp = Text -> firstline;   //有第一行开始遍历
    while(lp)
    {
        int x = paintX;             //每行起始点皆为paintX
        block *bp = lp -> firstblock;//该行第一个数据块

        painter.setPen(QPen(Qt::gray));//行数颜色为灰
        int linesPaintX = lines <= 9 ? 2 * textSize : (lines <= 99 ? textSize : 0);
        painter.drawText(linesPaintX, y, QString::number(lines++));

        if(color)
            painter.setPen(QPen(Qt::red));
        else
            painter.setPen(QPen(Qt::black));

        while(bp)
        {
            for(int i = 0; i <= bp -> size; i++)
            {
                if(i)
                {
                    QChar c= bp -> word[i];//当前字符
                    //获取该字符在当前样式下的宽度
                    int width = QFontMetrics(font).width(c);
                    //绘制在字符
                    painter.drawText(x+2, y, bp -> word[i]);
                    //更新横坐标，下一个字符紧接当前字符
                    x += width;

                }
                //光标在当前位置且显示状态为真
                if(Text -> cursor -> blockcur == bp &&
                 Text -> cursor -> n == i && timer -> showcursor)
                {
                    painter.drawText(x - 2, y, tr("|"));
                }
                //选中状态改变，更新画笔颜色（实现选中区域标红）
                if(bp -> select[i])
                {
                    if(!color)painter.setPen(QPen(Qt::red));
                    else      painter.setPen(QPen(Qt::black));
                    color = !color;
                }
            }
            bp = bp -> nextblock;
            //字符超过滚动区域，更新滚动区域（水平方向）
            if(x >= Hscroll -> maximum())updateScrollValue(100, 0);
        }
        y += stepy;
        //字符超过滚动区域，更新滚动区域（竖直方向）
        if(y >= Vscroll -> maximum())updateScrollValue(0, 100);
        lp = lp -> nextline;
    }
    if(hasSelection())Text -> hideSelect();//有选中区域，隐藏标记
    if(!Text -> sltcur.empty())Text -> hideFind();//隐藏查找标记
    painter.end();          //结束画图
    update();               //更新屏幕
}
void MyChild::updateFix()
{
    fix = false;
}
void MyChild::VscrollChange(int value)
{
    paintY = textSize * 2 -  value;
    drawPix();
}
void MyChild::HscrollChange(int value)
{
    paintX = textSize * 3 - value;
    drawPix();
}
/*
 * 编辑
*/
void MyChild::insert(QChar c)
{
    if(hasSelection())Text -> del();
    Text -> insert(c);
}

void MyChild::backspace()
{
    if(hasSelection())Text -> del();
    else Text -> backspace();
}

void MyChild::del()
{
    if(hasSelection())Text -> del();
    else Text -> delet();
}

void MyChild::enter()
{
    if(hasSelection())Text -> del();
    Text -> enter();
}
/*
 * 文件更改标签
 * 编辑器内容是否被更改，可以使用QTextDocument类的isModified()函数获知，这里使用了QTextEdit类，document()函数来获取
 * 它的QTextDocument类对象。然后使用setWindowModified()函数设置窗口的更改状态标志“*”，如果参数为true，则将在标题中设置
 * 了“[*]”号的地方显示“*”号，表示该文件已经被修改。
*/
 void MyChild::documentWasModified ()
 {
     //根据文档的isModified()函数的返回值，判断编译器内容是否被更改
     setWindowModified (isModified);
 }
//获取最简文件名
QString MyChild::userFriendlyCurrentFile ()
{
    return strippedName (curFile);
}
QString MyChild::strippedName (const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName ();
}
/*
 * 设置当前文件属性
*/
void MyChild::setCurrentFile (const QString &fileName)
{
    /*canonicalFilePath ()可以除去路径中符号链接，如“.”和“..”等符号。这个
     *函数只是将加载文件的路径首先保存到curFile中，然后再进行一些状态的设置*/
    curFile = QFileInfo(fileName).canonicalFilePath ();
    //文件已经被保存过
    isUntitled = false;
    //文档没有被更改过
    isModified = false;
    //窗口不显示被更改标志
    setWindowModified (false);
    //设置窗口标题，userFriendlyCurrentFile ()返回文件名
    setWindowTitle (userFriendlyCurrentFile () + "[*]");
//    qDebug() << fileName << " " << curFile << " " << userFriendlyCurrentFile();
}
MyChild::~MyChild()
{
    delete pix;
    delete Text;
    delete timer;
    delete Vscroll;
    delete Hscroll;
}
