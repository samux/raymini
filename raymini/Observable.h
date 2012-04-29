/**
 * An observable object which have a list of observers and can notify them
 */

#pragma once

#include <vector>

class Observer;

class Observable {
public:
    /** Add an observer to the list */
    void addObserver(Observer *observer);

    /** Remove an observer from the list */
    void removeObserver(Observer *observer);

protected:
    /** Notify any observer in observers */
    void notifyAll();

private:
    /** List of observers */
    std::vector<Observer*> observers;
};
