#include "timer.h"
#include <QDebug>

Timer::Timer(QObject *parent, MyChild* _f) : QObject(parent)
{
    TimerId = startTimer(500);
    showcursor = true;
    f = _f;
}

//每隔一秒重绘光标
void Timer::timerEvent(QTimerEvent *)
{
    showcursor = !showcursor;
    f -> drawPix();
}
Timer::~Timer()
{
    if(TimerId)killTimer(TimerId);
}
