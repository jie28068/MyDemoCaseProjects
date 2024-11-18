#include "blocking_queue.h"
#include <QtCore/qmutex.h>
#include <QtCore/qwaitcondition.h>

class EventPrivate
{
public:
    EventPrivate();
    QWaitCondition condition;
    QMutex mutex;

    QAtomicInt flag;
    QAtomicInt waiters;
};

EventPrivate::EventPrivate() : flag(0), waiters(0) { }

Event::Event() : d_ptr(new EventPrivate()) { }

void Event::set()
{

    Q_D(Event);
    d->mutex.lock();
    if (!d->flag) {
        // d->flag;
        d->flag = 1;
        d->condition.wakeAll();
    }
    d->mutex.unlock();
}

void Event::clear()
{
    Q_D(Event);
    d->flag = 0;
}

bool Event::wait(unsigned long time)
{
    Q_D(Event);
    if (!d->flag) {
        d->mutex.lock();
        if (!d->flag) {
            d->waiters = d->waiters + 1;
            d->condition.wait(&d->mutex, time);
            d->waiters = d->waiters - 1;
        }
        d->mutex.unlock();
    }
    return d->flag;
}

bool Event::isSet() const
{
    Q_D(const Event);
    return d->flag;
}

quint32 Event::getting() const
{
    Q_D(const Event);
    return d->waiters;
}
