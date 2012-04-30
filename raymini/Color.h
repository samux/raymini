#pragma once

#include "Vec3D.h"

class Color {
private:
    Vec3Df color;
    unsigned number;

public:
    Color() :
        color(Vec3Df()), number(0) {}

    Color(const Vec3Df &init) :
        color(init), number(1) {}

    Color & operator+=(const Vec3Df &c) {
        add(c);
        return *this;
    }

    Color & operator+=(const Color &c) {
        color += c.color;
        if(c.number == 0 || number == 0)
            number++;
        number += c.number;
        return *this;
    }

    Color & operator*=(float k) {
        color*=k;
        return *this;
    }

    Color & operator=(const Vec3Df &c) {
        set(c);
        return *this;
    }

    Vec3Df operator()() const {
        return get();
    }

    float operator[](unsigned i) const {
        return get()[i];
    }

private:
    void add(const Vec3Df &c) {
        if(number) {
            number++;
            color+=c;
        }
        else
            set(c);
    }

    void set(const Vec3Df &init) {
        color = init;
        number = 1;
    }

    Vec3Df get() const {
        if(number)
            return color/float(number);
        else
            return color;
    }
};
