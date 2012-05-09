/**
 * An observable object which have a list of observers and can notify them
 */

#pragma once

#include <vector>

class Observer;

class Observable {
public:
    Observable();

    /** Add an observer to the list */
    void addObserver(Observer *observer);

    /** Remove an observer from the list */
    void removeObserver(Observer *observer);

    /** Notify any observer in observers */
    void notifyAll();

    /**
     * Set changed for an offset
     * Each flag is a bit of the unsigned long flag
     */
    void setChanged(unsigned long offset);

    /**
     * Return true if flag is at 1 for this offset
     */
    bool isChanged(unsigned long offset) const;

protected:
    /** List of observers */
    std::vector<Observer*> observers;

    /** State flag */
    unsigned long flag;

    /** Clear all changed flag */
    void clearChanged();
};
