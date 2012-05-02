#include <iomanip>

#include "ProgressBar.h"
#include "Controller.h"

using namespace std;

ProgressBar::ProgressBar(Controller *c, unsigned nbIter):
    controller(c),
    max(nbIter),
    current(0),
    start(chrono::system_clock::now()),
    lastPercent(0)
{
    connect(this, SIGNAL(updatedPercent(float)), controller, SLOT(renderProgressed(float)));
    omp_init_lock(&lck);
    cerr << endl
         << setw (5) << 0
         << "% >";
    for(unsigned i = 0 ; i < 100 ; i++)
        cerr << ' ';
    cerr << '<';
}

ProgressBar::~ProgressBar() {
}

#define MIN_PERCENT 10
#define MIN_TIME 1000000

void ProgressBar::operator()() {
    lock();
    float percent = 100.f*float(current)/float(max);
    cerr << '\r'
         << fixed << setprecision(2) << setw (5) << percent
         << "% >";
    for(unsigned i = 0 ; i < unsigned(percent)+1 ; i++)
        cerr << '*';
    current++;
    float currentCurrent = current;
    unlock();
    auto now = chrono::system_clock::now();
    if(currentCurrent==max) {
        chrono::microseconds u = now -start;
        cerr << "< " << u.count()/1000 << "ms "
             << '\r' << setw (5) << 100.00;
        emit updatedPercent(100);
    }
    else if (percent - lastPercent > MIN_PERCENT) {
        lastPercent = percent;
        lastNow = now;
        emit updatedPercent(percent);
    }
    else if (chrono::microseconds(now - lastNow).count() > MIN_TIME) {
        lastPercent = percent;
        lastNow = now;
        emit updatedPercent(percent);
    }
}
