#include "Observable.h"

#include "Observer.h"

using namespace std;

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
    for (Observer *observer: observers) {
        observer->update(this);
    }
}
