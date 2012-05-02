#pragma once

#include <QObject>
#include <chrono>
#include <omp.h>
#include <iostream>

class ProgressBar: public QObject {
    Q_OBJECT
private:
    unsigned max;
    unsigned current;
    omp_lock_t lck;
    std::chrono::time_point<std::chrono::system_clock> start;

    void lock() { omp_set_lock(&lck); }
    void unlock() { omp_unset_lock(&lck); }

public:
    ProgressBar(unsigned nbIter);
    virtual ~ProgressBar();

    void operator()();
};
