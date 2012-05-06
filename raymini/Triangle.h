// *********************************************************
// Triangle Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2008 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <iostream>
#include <vector>

class Triangle {
public:
    inline Triangle () { init (0, 0, 0, (unsigned int[]){0, 0, 0}, (unsigned int[]){0, 0, 0}); }
    inline Triangle (unsigned int v0, unsigned int v1, unsigned int v2) { init (v0, v1, v2, (unsigned int[]){0, 0, 0}, (unsigned int[]){0, 0, 0}); }
    inline Triangle (const unsigned int * vp) { init (vp[0], vp[1], vp[2], (unsigned int[]){0, 0, 0}, (unsigned int[]){0, 0, 0}); }
    inline Triangle (const Triangle & it) { init (it.vertices[0], it.vertices[1], it.vertices[2], it.us, it.vs); }
    inline virtual ~Triangle () {}
    inline Triangle & operator=(const Triangle & it) {
        init(it.vertices[0], it.vertices[1], it.vertices[2], it.us, it.vs);
        return (*this);
    }
    inline bool operator==(const Triangle & t) const {
        bool equals = true;
        for (unsigned int i=0; i<3; i++) {
            equals &= vertices[i] == t.vertices[i];
            equals &= us[i] == t.us[i];
            equals &= vs[i] == t.vs[i];
            if (!equals) {
                break;
            }
        }
        return equals;
    }
    inline unsigned int getVertex (unsigned int i) const { return vertices[i]; }
    inline unsigned int getU(unsigned int i) const {return us[i];}
    inline unsigned int getV(unsigned int i) const {return vs[i];}
    inline void setU(unsigned int i, unsigned int u) {us[i] = u;}
    inline void setV(unsigned int i, unsigned int v) {vs[i] = v;}
    inline void setUV(unsigned int i, unsigned int u, unsigned int v) {us[i] = u; vs[i] = v;}
    inline void setVertex (unsigned int i, unsigned int vertex) { vertices[i] = vertex; }
    inline bool contains (unsigned int vertex) const { return (vertices[0] == vertex || vertices[1] == vertex || vertices[2] == vertex); }

protected:
    inline void init (unsigned int v0, unsigned int v1, unsigned int v2, const unsigned int us[3], const unsigned int vs[3]) {
        vertices[0] = v0; vertices[1] = v1; vertices[2] = v2;
        for (unsigned int i=0; i<3; i++) {
            this->us[i] = us[i];
            this->vs[i] = vs[i];
        }
    }

private:
    unsigned int vertices[3];
    unsigned int us[3];
    unsigned int vs[3];
};

extern std::ostream & operator<< (std::ostream & output, const Triangle & t);


#endif // TRIANGLE_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
