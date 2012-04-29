/**
 * A model handling any Window view state
 */

#pragma once

#include "Observable.h"

class Controller;

class WindowModel: public Observable {
public:
    WindowModel(Controller *);

private:
    Controller *controller;
};
