#ifndef TIMER_H
#define TIMER_H
#include <QObject>
#include "mychild.h"

class MyChild;
class Timer : public QObject
{
    Q_OBJECT
public:
    explicit Timer(QObject *parent = nullptr, MyChild *_f = nullptr);
    bool showcursor;
    ~Timer();
signals:

public slots:
protected:
    MyChild *f;
    void timerEvent(QTimerEvent *);
    int TimerId;
};

#endif // TIMER_H
