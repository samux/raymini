// *********************************************************
// Vertex Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2008 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <vector>

#include "Vec3D.h"

class Vertex {
public:
    inline Vertex () 
        : pos (Vec3Df (0.0,0.0,0.0)), normal (Vec3Df (0.0, 0.0, 1.0)), 
          marked (false), id (-1) {}
    inline Vertex (const Vec3Df & pos) 
        : pos (pos), normal (Vec3Df (0.0, 0.0, 1.0)), 
          marked (false), id (-1) {}
    inline Vertex (const Vec3Df & pos, const Vec3Df & normal) 
        : pos (pos), normal (normal), 
          marked (false), id (-1) {}
    inline Vertex (const Vertex & v) : pos (v.pos), normal (v.normal), 
                                       marked (v.marked), id (-1) {}
    inline virtual ~Vertex () {}
    inline Vertex & operator= (const Vertex & vertex) {
        pos = vertex.pos;
        normal = vertex.normal;
        marked = vertex.marked;
        id = -1;
        return (*this);
    }
    inline const Vec3Df & getPos () const { return pos; }
    inline const Vec3Df & getNormal () const { return normal; }  
    inline bool isMarked () const { return marked; }
    inline int getId () const { return id; }
    inline void setPos (const Vec3Df & newPos) { pos = newPos; }
    inline void setNormal (const Vec3Df & newNormal) { normal = newNormal; }
    inline void mark () { marked = true; }
    inline void unmark () { marked = false; }
    inline void setId (int newId) { id = newId; } 
    inline bool operator== (const Vertex & v) { return (v.pos == pos && v.normal == normal); }
    void interpolate (const Vertex & u, const Vertex & v, float alpha = 0.5);

    static void computeAveragePosAndRadius (std::vector<Vertex> & vertices, 
                                            Vec3Df & center, float & radius);
    static void scaleToUnitBox (std::vector<Vertex> & vertices, 
                                Vec3Df & center, float & scaleToUnitBox);
    static void normalizeNormals (std::vector<Vertex> & vertices);

private:
    Vec3Df pos;
    Vec3Df normal;
    bool marked;
    int id;
};

extern std::ostream & operator<< (std::ostream & output, const Vertex & v);

#endif // VERTEX_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
