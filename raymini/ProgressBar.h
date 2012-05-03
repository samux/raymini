#pragma once

#include <QObject>
#include <chrono>
#include <omp.h>
#include <iostream>

class Controller;

class ProgressBar: public QObject {
    Q_OBJECT
private:
    Controller *controller;
    unsigned max;
    unsigned current;
    omp_lock_t lck;
    std::chrono::time_point<std::chrono::system_clock> start;
    float lastPercent;
    std::chrono::time_point<std::chrono::system_clock> lastNow;

    void lock() { omp_set_lock(&lck); }
    void unlock() { omp_unset_lock(&lck); }

public:
    ProgressBar(Controller *, unsigned nbIter);
    virtual ~ProgressBar();

    void operator()();

signals:
    void updatedPercent(float);
};
