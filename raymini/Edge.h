// *********************************************************
// Edge Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2008 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef EDGE_H
#define EDGE_H

#include <map>

// -------------------------------------------------
// Intermediate Edge structure for hashed adjacency
// -------------------------------------------------

struct Edge {
public:
  inline Edge (unsigned int v0, unsigned int v1) { 
    if (v0 < v1) {v[0] = v0; v[1] = v1; } else {v[0] = v1; v[1] = v0; }  
  }
  inline Edge (const Edge & e) { v[0] = e.v[0]; v[1] = e.v[1]; }
  inline virtual ~Edge () {}
  inline Edge & operator= (const Edge & e) { v[0] = e.v[0]; v[1] = e.v[1]; return (*this); }
  inline bool operator== (const Edge & e) { return (v[0] == e.v[0] && v[1] == e.v[1]); }
  inline bool operator< (const Edge & e) { return (v[0] < e.v[0] || (v[0] == e.v[0] && v[1] < e.v[1])); }
  inline bool contains (unsigned int i) const { return (v[0] == i || v[1] == i); }
  unsigned int v[2];
};

struct compareEdge {
  inline bool operator()(const Edge e1, const Edge e2) const { 
    if (e1.v[0] < e2.v[0])
      return true;
    if (e1.v[0] > e2.v[0])
      return false;
    if (e1.v[1] > e2.v[1])
      return true;
    return false;
  }
};

typedef std::map<Edge, unsigned int, compareEdge> EdgeMapIndex;

#endif // EDGE_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
