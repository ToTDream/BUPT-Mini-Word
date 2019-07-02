#include "myword.h"
#include "mychild.h"
#include <QIcon>
#include <QFont>
#include <QPixmap>
#include <QMdiArea>
#include <QtWidgets>
#include <QSignalMapper>
#include <QFontDatabase>
/*
 * setHorizontalScrollBarPolicy和setVerticalScrollBarPolicy函数，只要设置
 * Qt::ScrollBarAlwaysOff就可以隐藏滚动条，此处设置为Qt::ScrollBarAsNeeded表示
 * 滚动条在需要（子窗口较多，主区域显示不下）时才出现
*/
class QString rsrcPath = ":/images";
Myword::Myword(QWidget *parent)
    : QMainWindow(parent)
{
    move(200, 150);
    resize (800, 500);
//    setWindowIcon (QIcon(rsrcPath + "main.png"));
    setWindowTitle (tr("Mini Word"));
    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy (Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy (Qt::ScrollBarAsNeeded);
    setCentralWidget (mdiArea);
    /*当有活动窗口时更新菜单*/
    connect (mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
    //创建信号映射器
    windowMapper = new QSignalMapper(this);
    //映射器重新发送信号，根据信号设置活动窗口
    connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget *)));
    createActions ();
    createMenus ();
    createToolBars();
    updateMenus ();
}
Myword::~Myword()
{

    delete comboFont; delete comboSize;
    delete mdiArea; delete windowMapper;
    delete separatorAct; delete aboutQtAct; delete aboutAct;
    delete fileToolBar; delete editToolBar;delete comboToolBar;
    delete fileMenu; delete editMenu; delete windowMenu; delete helpMenu;
    delete newAct; delete openAct; delete saveAct; delete saveAsAct; delete quitAct;
    delete cutAct; delete copyAct; delete pasteAct; delete searchAct; delete replaceAct;
    delete closeAct; delete closeAllAct; delete cascadeAct; delete nextAct; delete previousAct;
}
/*
 * 关于软件
*/
void Myword::about ()
{
    QMessageBox::about (this, tr("关于"), tr("这是一个基于Qt实现的文字处理软件，具备类似于window 记事本的功能。可支持多文档编辑"));
}
/*
 * 更新菜单的槽函数
*/
void Myword::updateWindowMenu ()
{
    //首先清空菜单，然后再添加各个菜单动作
    windowMenu->clear ();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();        //分割线
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();        //分割线
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);
    //如果有活动窗口，则显示分隔条
    QList<QMdiSubWindow *> windows = mdiArea->subWindowList ();
    separatorAct->setVisible (!windows.isEmpty ());
    //遍历各个子窗口，显示当前已打开的文档子窗口项
    for(int i = 0; i < windows.size (); i++)
    {
        MyChild *child = qobject_cast<MyChild*>(windows.at (i)->widget ());
        QString text;
        if(i < 9)
        {
            text = tr("&%1 %2").arg (i + 1).arg (child->userFriendlyCurrentFile ());
        }
        else
        {
            text = tr("%1 %2").arg (i + 1).arg (child->userFriendlyCurrentFile ());
        }
        //添加动作作到菜单，设置动作可以选择
        QAction *action = windowMenu->addAction(text);
        action->setCheckable (true);
        //设置当前活动窗口动作作为选中状态
        action->setChecked(child == activeMyChild ());
        //关联动作的触发信号到信号映射器的map()槽，这个槽会发送mapped()信号
        connect (action, SIGNAL(triggered(bool)), windowMapper, SLOT(map()));
        //动作作与相应的窗口部件进行映射，在发送mapped()信号时就会以这个窗口部件为参数
        windowMapper->setMapping (action, windows.at (i));
    }
}
/*
 * 主菜单
*/
void Myword::createMenus ()
{
    //"文件"主菜单
    fileMenu = menuBar ()->addMenu (tr("文件(&F)"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator ();  //分割线
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator ();
    fileMenu->addSeparator ();
    fileMenu->addAction(quitAct);
    //“编辑”主菜单
    editMenu = menuBar ()->addMenu (tr("编辑(&E)"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator ();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addAction(searchAct);
    editMenu->addAction(replaceAct);
    //"窗口"主菜单
    windowMenu = menuBar ()->addMenu (tr("窗口(&W)"));
    updateWindowMenu ();
    connect (windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
    menuBar ()->addSeparator ();
    //”帮助“主菜单
    helpMenu = menuBar ()->addMenu (tr("帮助(&H)"));
    helpMenu->addAction(aboutAct);
    helpMenu->addSeparator ();
    helpMenu->addAction(aboutQtAct);
}
/*
 * 文件主菜单动作集
*/
void Myword::createActions ()
{
    //文件菜单动作集
    newAct = new QAction(QIcon(rsrcPath + "/filenew.png"), tr("新建(&N)"), this);
    newAct->setShortcut (QKeySequence::New);  //设置快捷键
    newAct->setToolTip ("新建");               //设置工具栏按钮的提示文本
    newAct->setStatusTip ("新建一个新文档");     //设置按钮状态栏提示文本
     connect(newAct, SIGNAL(triggered(bool)), this, SLOT(fileNew()));
    openAct = new QAction(QIcon(rsrcPath + "/fileopen.png"), tr("打开(&O)"), this);
    openAct->setShortcut (QKeySequence::Open);
    openAct->setToolTip ("打开");
    openAct->setStatusTip ("打开已存在的文档");
     connect (openAct, SIGNAL(triggered(bool)), this, SLOT(fileOpen()));
    saveAct = new QAction(QIcon(rsrcPath + "/filesave.png"), tr("保存(&S)"), this);
    saveAct->setShortcut (QKeySequence::Save);
    saveAct->setToolTip ("保存");
    saveAct->setStatusTip ("将当前文档存盘");
     connect (saveAct, SIGNAL(triggered(bool)), this, SLOT(fileSave()));
    saveAsAct = new QAction(tr("另存为(&A)..."), this);
    saveAsAct->setShortcut (QKeySequence::SaveAs);
    saveAsAct->setStatusTip (tr("以一个新名字保存文档"));
     connect (saveAsAct, SIGNAL(triggered(bool)), this, SLOT(fileSaveAs()));
    quitAct = new QAction(tr("退出"), this);
    quitAct->setShortcut (tr("Ctrl+Q"));
    quitAct->setStatusTip (tr("退出应用程序"));
     connect (quitAct, SIGNAL(triggered(bool)), qApp, SLOT(closeAllWindows()));
    //编辑菜单动作集
    undoAct = new QAction(QIcon(rsrcPath + "/editundo.png"), tr("撤销(&U)"), this);
    undoAct->setShortcut (QKeySequence::Undo);
    undoAct->setToolTip (tr("撤销"));
    undoAct->setStatusTip ("撤销当前操作");
     connect (undoAct, SIGNAL(triggered(bool)), this, SLOT(undo()));
    redoAct = new QAction(QIcon(rsrcPath + "/editredo.png"), tr("重做(&R)"), this);
    redoAct->setShortcut (QKeySequence::Redo);
    redoAct->setToolTip (tr("重做"));
    redoAct->setStatusTip (tr("恢复之前操作"));
     connect (redoAct, SIGNAL(triggered(bool)), this, SLOT(redo()));
    cutAct = new QAction(QIcon(rsrcPath + "/editcut.png"), tr("剪切(&T)"), this);
    cutAct->setShortcut (QKeySequence::Cut);
    cutAct->setToolTip (tr("剪切"));
    cutAct->setStatusTip (tr("从文档中裁剪所选内容，并将其放入剪贴板"));
     connect (cutAct, SIGNAL(triggered(bool)), this, SLOT(cut()));
    copyAct = new QAction(QIcon(rsrcPath + "/editcopy.png"), tr("复制(&C)"), this);
    copyAct->setShortcut (QKeySequence::Copy);
    copyAct->setToolTip (tr("复制"));
    copyAct->setStatusTip (tr("拷贝所选内容，并将其放入剪切板"));
     connect (copyAct, SIGNAL(triggered(bool)), this, SLOT(copy()));
    pasteAct = new QAction(QIcon(rsrcPath + "/editpaste.png"), tr("粘贴(&P)"), this);
    pasteAct->setShortcut (QKeySequence::Paste);
    pasteAct->setToolTip (tr("粘贴"));
    pasteAct->setStatusTip (tr("将剪粘板的内容粘贴到文档"));
     connect (pasteAct, SIGNAL(triggered(bool)), this, SLOT(paste()));
    searchAct = new QAction(QIcon(rsrcPath + "/editsearch.png"), tr("查找(&F)"), this);
    searchAct->setShortcut (QKeySequence::Find);
    searchAct->setToolTip (tr("查找"));
    searchAct->setStatusTip (tr("查找字符串"));
     connect (searchAct, SIGNAL(triggered(bool)), this, SLOT(search_replace()));
    replaceAct = new QAction(QIcon(rsrcPath + "/editreplace.png"), tr("替换(&R)"), this);
    replaceAct->setShortcut (QKeySequence::Replace);
    replaceAct->setToolTip (tr("替换"));
    replaceAct->setStatusTip (tr("替换字符串"));
     connect (replaceAct, SIGNAL(triggered(bool)), this, SLOT(search_replace()));
    //”窗口“主菜单动作集
    closeAct = new QAction(tr("关闭(&O)"), this);
    closeAct->setStatusTip (tr("关闭活动文档子窗口"));
     connect (closeAct, SIGNAL(triggered(bool)), mdiArea, SLOT(closeActiveSubWindow()));
    closeAllAct = new QAction(tr("关闭所有(&A)"), this);
    closeAllAct->setStatusTip (tr("关闭所有子窗口"));
     connect(closeAllAct, SIGNAL(triggered(bool)), mdiArea, SLOT(closeAllSubWindows()));
    tileAct = new QAction(tr("平铺(&T)"), this);
    tileAct->setStatusTip (tr("平铺子窗口"));
     connect(tileAct, SIGNAL(triggered(bool)), mdiArea, SLOT(tileSubWindows()));
    cascadeAct = new QAction(tr("层叠(&C)"), this);
    cascadeAct->setStatusTip (tr("曾跌子窗口"));
     connect (cascadeAct, SIGNAL(triggered(bool)), mdiArea, SLOT(cascadeSubWindows()));
    nextAct = new QAction(tr("下一个(&X)"), this);
    nextAct->setShortcut (QKeySequence::NextChild);
    nextAct->setStatusTip (tr("移动焦点到下一个子窗口"));
     connect(nextAct, SIGNAL(triggered(bool)), mdiArea, SLOT(activateNextSubWindow()));
    previousAct = new QAction(tr("前一个(&V)"), this);
    previousAct->setShortcut (QKeySequence::PreviousChild);
    previousAct->setStatusTip (tr("移动焦点到前一个子窗口"));
     connect(previousAct, SIGNAL(triggered(bool)), mdiArea, SLOT(activatePreviousSubWindow()));
    separatorAct = new QAction(this);
    separatorAct->setSeparator (true);
    //"帮助"主菜单动作集
    aboutAct = new QAction(tr("关于(&A)"), this);
    aboutAct->setStatusTip (tr("Myself Word"));
     connect (aboutAct, SIGNAL(triggered(bool)), this, SLOT(about()));
    aboutQtAct = new QAction(tr("关于Qt(&Q)"), this);
    aboutQtAct->setStatusTip (tr("关于Qt库"));
     connect(aboutQtAct, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));
}
/*
*  当有活动窗口时更新菜单
*/
void Myword::updateMenus ()
{
    //至少有一个子文档打开时的情况
    bool hasMyChild = (activeMyChild () != 0);
    saveAct->setEnabled(hasMyChild);
    saveAsAct->setEnabled(hasMyChild);
    closeAct->setEnabled(hasMyChild);
    closeAllAct->setEnabled(hasMyChild);
    tileAct->setEnabled(hasMyChild);
    cascadeAct->setEnabled(hasMyChild);
    nextAct->setEnabled(hasMyChild);
    previousAct->setEnabled(hasMyChild);
    separatorAct->setVisible(hasMyChild);
    //文档打开着并且其中有内容被选中的情况
    bool hasSelection = (activeMyChild() && activeMyChild()->hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
}
/*
 * 创建状态栏
*/
void Myword::createStatusBar ()
{
    statusBar()->showMessage (tr("就绪"));
}
/*
 * 工具栏设计
*/
void Myword::createToolBars ()
{
    //”文件“工具栏
    fileToolBar = addToolBar(tr("文件"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addSeparator();   //分隔条
    //”编辑“工具栏
    editToolBar = addToolBar(tr("编辑"));
    editToolBar->addAction(undoAct);
    editToolBar->addAction(redoAct);
    editToolBar->addSeparator();
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    editToolBar->addAction(replaceAct);
    editToolBar->addAction(searchAct);
    //组合选择栏
    //可以使工具条分多行显示。这里是将组合选择条在工具栏上另起一行显示，以使界面看起来更加美观，大方。
    addToolBarBreak (Qt::TopToolBarArea);
    comboToolBar = addToolBar (tr("组合选择框"));
    comboFont = new QFontComboBox();
    comboToolBar->addWidget (comboFont);
    comboFont->addItem ("更改字体");
    connect (comboFont, SIGNAL(activated(QString)), this, SLOT(textFamily(QString)));
    comboSize = new QComboBox();
    comboToolBar->addWidget (comboSize);
    comboSize->setEditable (true);
    comboSize->setStatusTip ("更改字号");
    QFontDatabase db;
    foreach(int size, db.standardSizes ())
    {
        comboSize->addItem (QString::number (size));
        connect(comboSize, SIGNAL(activated(QString)), this, SLOT(textSize(QString)));
        comboSize->setCurrentIndex (comboSize->findText (QString::number (QApplication::font ().pointSize ())));
    }
}
/*
 * 创建MyChild部件，并将它作为子窗口的中心部件，添加到多文档区域。紧接着关联编辑器的信号和菜单动作，
 * 让它们可以随着文档的改变而改变状态。最后返回MyChild对象指针。
*/
MyChild *Myword::createMyChild (bool isNew)
{
    MyChild *child = new MyChild(isNew);       //创建MyChild部件
    mdiArea->addSubWindow (child);             //向多文档区域添加子窗口，child为中心部件
    child->setFocusPolicy(Qt::StrongFocus);
    child -> setEnabled(true);
    //根据QTextEdit类是否可以复制信号、设置剪切、复制动作是否可用
    connect(child, SIGNAL(selectText(bool)),cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(selectText(bool)),copyAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(postnum(int)),this, SLOT(showdetail(int)));
    return child;
}
/*
* 判断是否有活动子窗口用activeMyChild()函数
*/
MyChild *Myword::activeMyChild ()
{
   //若有活动窗口，则将其内的中心部件转换为MyChild类型，若没有直接返回0
   if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
       return qobject_cast<MyChild *>(activeSubWindow->widget());
   return 0;
}
/*
 * 发现子窗口操作
*/
QMdiSubWindow *Myword::findMyChild (const QString &fileName)
{
    /*canonicalFilePath ()可以除去路径中符号链接，如“.”和“..”等符号。这个
     *函数只是将加载文件的路径首先保存到canonicalFilePath中，然后再进行一些状态的设置*/
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath ();
    //遍历整个多文档区域的所有子窗口
    foreach(QMdiSubWindow *window, mdiArea->subWindowList ())
    {
        MyChild *myChild = qobject_cast<MyChild *>(window->widget ());
        if(myChild->currentFile () == canonicalFilePath)
        {
            return window;
        }
    }
    return 0;
}
/*
 * 设置活动子窗口
*/
void Myword::setActiveSubWindow (QWidget *window)
{
    if(!window) return;
    mdiArea->setActiveSubWindow (qobject_cast<QMdiSubWindow*>(window));
}
/*
 * 新建文件函数
*/
void Myword::fileNew ()
{
    MyChild *child = createMyChild (true);
    child -> newFile ();
    child -> show ();
}
/*
 * 打开文件操作
*/
void Myword::fileOpen ()
{
    QString fileName = QFileDialog::getOpenFileName (this, tr("打开"), QString(), tr("HTML 文档 (*.html);所有文件(*.*)"));
    if(!fileName.isEmpty())
    {
        QMdiSubWindow *existing = findMyChild (fileName);
        //如果发现该文件已经打开，则直接设置为子窗口为活动窗口
        if(existing)
        {
            mdiArea -> setActiveSubWindow(existing);
            return;
        }
        //如果文件没有打开则直接加载要打开的文件，并添加新的子窗口
        MyChild *child = createMyChild(false);
        if(child->loadFile(fileName))
        {
            child -> isUntitled = false;
            statusBar() -> showMessage(tr("文件已加入"), 2000);
            child -> show();;
        }
        else
        {
            child->close();
        }
    }
}
/*
 * 保存文件操作
*/
void Myword::fileSave ()
{
    /*如果有活动窗口并且活动点击saveAct动作*/
    if(activeMyChild() && activeMyChild()->save())
    {
        statusBar() -> showMessage(tr("保存成功"), 2000);
    }
}
void Myword::fileSaveAs ()
{
    /*如果有活动窗口并且点击saveAsAct动作*/
    if(activeMyChild() && activeMyChild()->saveAs ())
    {
        statusBar() -> showMessage(tr("保存成功"), 2000);
    }
}
/*
 * 关闭事件
*/
void Myword::closeEvent (QCloseEvent *event)
{
    mdiArea -> closeAllSubWindows();
    if(mdiArea -> currentSubWindow())
    {
        event -> ignore ();
    }
    else
    {
        event -> accept ();
    }
}
//撤销
void Myword::undo()
{
    //判断是否是当前活动窗口
    if(activeMyChild ())
        activeMyChild ()->undo();
}
//重做
void Myword::redo ()
{
    //判断是否是当前活动窗口
    if(activeMyChild ())
        activeMyChild ()->redo ();
}
//剪切
void Myword::cut ()
{
    //判断是否是当前活动窗口
    if(activeMyChild ())
        activeMyChild ()->cut ();
}
//复制
void Myword::copy ()
{
    //判断是否是当前活动窗口
    if(activeMyChild ())
        activeMyChild ()->copy ();
}
//粘贴
void Myword::paste ()
{
    //判断是否是当前活动窗口
    if(activeMyChild ())
        activeMyChild ()->paste ();
}
//查找替换
void Myword::search_replace ()
{
    if(activeMyChild ())
        activeMyChild ()->search_replace ();
}
//字体
void Myword::textFamily (const QString &f)
{
    if(activeMyChild ())
        activeMyChild() -> settextFamily(f);
}
//字号
void Myword::textSize (const QString &p)
{
    qreal pointsize = p.toFloat ();
    if(p.toFloat () > 0)
    {
        if(activeMyChild ())
            activeMyChild ()->settextSize(pointsize);
    }
}
void Myword::showdetail(int num)
{
    statusBar() -> showMessage(QString::number(num), 200);
}
