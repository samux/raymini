/**
 * An observer, can be notified and added to an observable list
 */
#pragma once

#include "Observable.h"

class Observer {
public:
    /** Called whenever an Observable cals notify */
    virtual void update(const Observable *caller) = 0;
};
