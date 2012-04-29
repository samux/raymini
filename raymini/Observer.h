/**
 * An observer, can be notified and added to an observable list
 */
#pragma once

class Observer {
public:
    /** Called whenever an Observable cals notify */
    virtual void update(Observable *caller) = 0;
};
