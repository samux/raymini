// *********************************************************
// Triangle Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2008 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Triangle.h"

using namespace std;

ostream & operator<< (ostream & output, const Triangle & t) {
    output << t.getVertex (0) << " " << t.getVertex (1) << " " << t.getVertex (2);
    return output;
}
