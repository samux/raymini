#include "Observable.h"

#include "Observer.h"

using namespace std;

Observable::Observable():
    // Put flag to 0xffff...
    flag((unsigned long)(-1))
{}

void Observable::addObserver(Observer *observer) {
    for (Observer *o : observers) {
        if (observer == o) {
            return;
        }
    }
    observers.push_back(observer);
}

void Observable::removeObserver(Observer *observer) {
    auto pos(observers.begin());
    for (Observer *o : observers) {
        if (observer == o) {
            break;
        }
        pos++;
    }
    observers.erase(pos);
}

void Observable::notifyAll() {
    if (flag) {
        for (Observer *observer: observers) {
            observer->update(this);
        }
    }
    clearChanged();
}

void Observable::clearChanged() {
    flag = 0;
}

void Observable::setChanged(unsigned long o) {
    flag |= o;
}

bool Observable::isChanged(unsigned long o) const {
    return flag & o;
}
