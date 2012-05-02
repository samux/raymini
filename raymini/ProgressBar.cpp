#include <iomanip>

#include "ProgressBar.h"

using namespace std;

ProgressBar::ProgressBar(unsigned nbIter):
    max(nbIter),
    current(0),
    start(chrono::system_clock::now())
{
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

void ProgressBar::operator()() {
    lock();
    float percent = 100.f*float(current)/float(max);
    cerr << '\r'
        << fixed << setprecision(2) << setw (5) << percent
        << "% >";
    for(unsigned i = 0 ; i < unsigned(percent)+1 ; i++)
        cerr << '*';
    current++;
    if(current==max) {
        auto now = chrono::system_clock::now();
        chrono::microseconds u = now -start;
        cerr << "< " << u.count()/1000 << "ms "
            << '\r' << setw (5) << 100.00;
    }
    unlock();
}
