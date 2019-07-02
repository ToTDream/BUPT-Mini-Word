#ifndef MYWORD_H
#define MYWORD_H
#include <QMainWindow>
class QMenu;        //主菜单
class MyChild;      //子窗口
class QAction;      //动作
class QMdiArea;     //MyChild
class QComboBox;
class QFontComboBox;
class QMdiSubWindow;
class QSignalMapper;//信号映射器，它可以实现对多个子窗口部件的相同信号进行映射，首先为其添加字符串或者数值参数，然后在发送出去。
class Myword : public QMainWindow
{
    Q_OBJECT
public:
    Myword(QWidget *parent = 0);
    ~Myword();
private:
    QMdiArea *mdiArea;
    void createMenus();         //创建主菜单
    void createActions();       //创建动作
    void createToolBars();      //创建工具栏
    void createStatusBar();     //创建状态栏
    void enabledText();         //设置格式菜单及其对应的工具按钮变为可用状态
    MyChild *activeMyChild();   //活动窗口
    QMdiSubWindow *findMyChild(const QString &fileName);    //查找子窗口
    QSignalMapper *windowMapper;
    //菜单
    QMenu *fileMenu;        //文件菜单
    QMenu *editMenu;        //编辑菜单
    QMenu *windowMenu;      //窗口菜单
    QMenu *helpMenu;        //帮助菜单
    //【文件】主菜单
    QAction *newAct;        //新建
    QAction *openAct;       //打开
    QAction *saveAct;       //保存
    QAction *saveAsAct;     //另存为
    QAction *quitAct;       //退出
    //【编辑】主菜单
    QAction *undoAct;       //撤销
    QAction *redoAct;       //重做
    QAction *cutAct;        //剪切
    QAction *copyAct;       //复制
    QAction *pasteAct;      //粘贴
    QAction *searchAct;     //查找
    QAction *replaceAct;    //替换
    // 【窗口】主菜单
    QAction *closeAct;      //关闭
    QAction *closeAllAct;   //关闭所有
    QAction *tileAct;       //平铺
    QAction *cascadeAct;    //层叠
    QAction *nextAct;       //下一个
    QAction *previousAct;   //前一个
    QAction *separatorAct;
    //【帮助】主菜单
    QAction *aboutAct;          //关于本软件
    QAction *aboutQtAct;        //关于Qt
    QToolBar *fileToolBar;      //"文件"工具条
    QToolBar *editToolBar;      //"编辑"工具条
//    QToolBar *formatToolBar;    //"格式"工具条
    QToolBar *comboToolBar;     //组合框选择栏
    QFontComboBox *comboFont;   //字体选择框
    QComboBox *comboSize;       //字号选择框
private slots:
//文件操作
    void fileNew();                         //新建文件操作
    void fileOpen();                        //打开文件操作
    void fileSave();                        //保存文件操作
    void fileSaveAs();                      //另存为文件操作
//编辑操作
    void undo();                            //撤销
    void redo();                            //重做
    void cut();                             //剪切
    void copy();                            //复制
    void paste();                           //粘贴
    void search_replace();                  //查找替换
//帮助
    void about();
    void textFamily(const QString &f);      //字体选择框
    void textSize(const QString &p);        //字号
    MyChild *createMyChild(bool);             //创建子窗口
    void updateMenus();                       //更新菜单
    void updateWindowMenu();                  //更新窗口菜单
    void setActiveSubWindow(QWidget *window); //设置活动窗口
    void showdetail(int);
protected:
    void closeEvent (QCloseEvent *event);
};
#endif // MYWORD_H
