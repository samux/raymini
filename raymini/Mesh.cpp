// ---------------------------------------------------------
// Mesh Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2008 Tamy Boubekeur.
// All rights reserved.
// ---------------------------------------------------------

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>

using namespace std;

void Mesh::clear () {
    clearTopology ();
    clearGeometry ();
}

void Mesh::clearGeometry () {
    vertices.clear ();
}

void Mesh::clearTopology () {
    triangles.clear ();
}

void Mesh::unmarkAllVertices () {
    for (unsigned int i = 0; i < vertices.size (); i++)
        vertices[i].unmark ();
}

void Mesh::computeTriangleNormals (vector<Vec3Df> & triangleNormals) {
    for (vector<Triangle>::const_iterator it = triangles.begin ();
         it != triangles.end ();
         it++) {
        Vec3Df e01 (vertices[it->getVertex (1)].getPos () - vertices[it->getVertex (0)].getPos ());
        Vec3Df e02 (vertices[it->getVertex (2)].getPos () - vertices[it->getVertex (0)].getPos ());
        Vec3Df n (Vec3Df::crossProduct (e01, e02));
        n.normalize ();
        triangleNormals.push_back (n);
    }
}

void Mesh::recomputeSmoothVertexNormals (unsigned int normWeight) {
    vector<Vec3Df> triangleNormals;
    computeTriangleNormals (triangleNormals);
    for (std::vector<Vertex>::iterator it = vertices.begin (); it != vertices.end (); it++)
        it->setNormal (Vec3Df (0.0, 0.0, 0.0));
    vector<Vec3Df>::const_iterator itNormal = triangleNormals.begin ();
    vector<Triangle>::const_iterator it = triangles.begin ();
    for ( ; it != triangles.end (); it++, itNormal++)
        for (unsigned int  j = 0; j < 3; j++) {
            Vertex & vj = vertices[it->getVertex (j)];
            float w = 1.0; // uniform weights
            Vec3Df e0 = vertices[it->getVertex ((j+1)%3)].getPos () - vj.getPos ();
            Vec3Df e1 = vertices[it->getVertex ((j+2)%3)].getPos () - vj.getPos ();
            if (normWeight == 1) { // area weight
                w = Vec3Df::crossProduct (e0, e1).getLength () / 2.0;
            } else if (normWeight == 2) { // angle weight
                e0.normalize ();
                e1.normalize ();
                w = (2.0 - (Vec3Df::dotProduct (e0, e1) + 1.0)) / 2.0;
            }
            if (w <= 0.0)
                continue;
            vj.setNormal (vj.getNormal () + (*itNormal) * w);
        }
    Vertex::normalizeNormals (vertices);
}

void Mesh::collectOneRing (vector<vector<unsigned int> > & oneRing) const {
    oneRing.resize (vertices.size ());
    for (unsigned int i = 0; i < triangles.size (); i++) {
        const Triangle & ti = triangles[i];
        for (unsigned int j = 0; j < 3; j++) {
            unsigned int vj = ti.getVertex (j);
            for (unsigned int k = 1; k < 3; k++) {
                unsigned int vk = ti.getVertex ((j+k)%3);
                if (find (oneRing[vj].begin (), oneRing[vj].end (), vk) == oneRing[vj].end ())
                    oneRing[vj].push_back (vk);
            }
        }
    }
}

void Mesh::collectOrderedOneRing (vector<vector<unsigned int> > & oneRing) const {
    oneRing.resize (vertices.size ());
    for (unsigned int t = 0; t < triangles.size (); t++) {
        const Triangle & ti = triangles[t];
        for (unsigned int i = 0; i < 3; i++) {
            unsigned int vi = ti.getVertex (i);
            unsigned int vj = ti.getVertex ((i+1)%3);
            unsigned int vk = ti.getVertex ((i+2)%3);
            vector<unsigned int> & oneRingVi = oneRing[vi];
            vector<unsigned int>::iterator begin = oneRingVi.begin ();
            vector<unsigned int>::iterator end = oneRingVi.end ();
            vector<unsigned int>::iterator nj = find (begin, end, vj);
            vector<unsigned int>::iterator nk = find (begin, end, vk);
            if (nj != end && nk == end) {
                if (nj == begin)
                    nj = end;
                nj--;
                oneRingVi.insert (nj, vk);
            } else if (nj == end && nk != end)
                oneRingVi.insert (nk, vj);
            else if (nj == end && nk == end) {
                oneRingVi.push_back (vk);
                oneRingVi.push_back (vj);
            }
        }
    }
}

void Mesh::computeDualEdgeMap (EdgeMapIndex & dualVMap1, EdgeMapIndex & dualVMap2) {
    for (vector<Triangle>::iterator it = triangles.begin ();
         it != triangles.end (); it++) {
        for (unsigned int i = 0; i < 3; i++) {
            Edge eij (it->getVertex (i), it->getVertex ((i+1)%3));
            if (dualVMap1.find (eij) == dualVMap1.end ())
                dualVMap1[eij] = it->getVertex ((i+2)%3);
            else
                dualVMap2[eij] = it->getVertex ((i+2)%3);
        }
    }
}

void Mesh::markBorderEdges (EdgeMapIndex & edgeMap) {
    for (vector<Triangle>::iterator it = triangles.begin ();
         it != triangles.end (); it++) {
        for (unsigned int i = 0; i < 3; i++) {
            unsigned int j = (i+1)%3;
            Edge eij (it->getVertex (i), it->getVertex (j));
            if (edgeMap.find (eij) == edgeMap.end ())
                edgeMap[eij] = 0;
            else
                edgeMap[eij] += 1;
        }
    }
}

inline void glVertexVec3Df (const Vec3Df & v) {
    glVertex3f (v[0], v[1], v[2]);
}

inline void glNormalVec3Df (const Vec3Df & n) {
    glNormal3f (n[0], n[1], n[2]);
}

inline void glDrawPoint (const Vec3Df & pos, const Vec3Df & normal) {
    glNormalVec3Df (normal);
    glVertexVec3Df (pos);
}

inline void glDrawPoint (const Vertex & v) {
    glDrawPoint (v.getPos (), v.getNormal ());
}

void Mesh::renderGL (bool flat) const {
    glBegin (GL_TRIANGLES);
    for (unsigned int i = 0; i < triangles.size (); i++) {
        const Triangle & t = triangles[i];
        Vertex v[3];
        for (unsigned int j = 0; j < 3; j++)
            v[j] = vertices[t.getVertex(j)];
        if (flat) {
            Vec3Df normal = Vec3Df::crossProduct (v[1].getPos () - v[0].getPos (),
                                                  v[2].getPos () - v[0].getPos ());
            normal.normalize ();
            glNormalVec3Df (normal);
        }
        for (unsigned int j = 0; j < 3; j++)
            if (!flat)
                glDrawPoint (v[j]);
            else
                glVertexVec3Df (v[j].getPos ());
    }
    glEnd ();
}

void Mesh::loadOFF (const std::string & filename) {
    clear ();
    ifstream input (filename.c_str ());
    if (!input)
        throw Exception ("Failing opening the file.");
    string magic_word;
    input >> magic_word;
    if (magic_word != "OFF")
        throw Exception ("Not an OFF file.");
    unsigned int numOfVertices, numOfTriangles, numOfWhat;
    input >> numOfVertices >> numOfTriangles >> numOfWhat;
    for (unsigned int i = 0; i < numOfVertices; i++) {
        Vec3Df pos;
        Vec3Df col;
        input >> pos;
        vertices.push_back (Vertex (pos, Vec3Df (1.0, 0.0, 0.0)));
    }
    for (unsigned int i = 0; i < numOfTriangles; i++) {
        unsigned int polygonSize;
        input >> polygonSize;
        vector<unsigned int> index (polygonSize);
        for (unsigned int j = 0; j < polygonSize; j++)
            input >> index[j];
        for (unsigned int j = 1; j < (polygonSize - 1); j++)
            triangles.push_back (Triangle (index[0], index[j], index[j+1]));
    }
    input.close ();
    recomputeSmoothVertexNormals (0);
    setDefaultTextureMapping();
}

void Mesh::rotate(const Vec3Df &axis, const float &angle) {
    Vec3Df normalizedAxis = axis;
    normalizedAxis.normalize();

    for (Vertex & v : vertices) {
        v.setPos(v.getPos().rotate(normalizedAxis, angle));
    }

    recomputeSmoothVertexNormals (0);
}


void Mesh::scale(const float &s) {
    for (Vertex & v : vertices) {
        v.setPos(v.getPos().scale(s));
    }
}

void Mesh::setDefaultTextureMapping(bool useNormals) {
    // See: http://www.mvps.org/directx/articles/spheremap.htm
    for (Triangle &t : triangles) {
        for (unsigned int i=0; i<3; i++) {
            const Vertex &v = vertices[t.getVertex(i)];
            if (useNormals) {
                Vec3Df normal = v.getNormal();
                t.setUV(i, asin(normal[0])/M_PI+0.5, asin(normal[2])/M_PI+0.5);
            } else {
                Vec3Df pos = v.getPos();
                pos.normalize();
                t.setUV(i, asin(pos[0])/M_PI+0.5, asin(pos[2])/M_PI+0.5);
            }
        }
    }
}

void Mesh::returnTriangle(unsigned int t) {
    Triangle &triangle = triangles[t];
    unsigned int temp = triangle.getVertex(0);
    float tempU = triangle.getU(0);
    float tempV = triangle.getV(0);
    triangle.setVertex(0, triangle.getVertex(1));
    triangle.setUV(0, triangle.getU(1), triangle.getV(1));
    triangle.setVertex(1, temp);
    triangle.setUV(1, tempU, tempV);
}

void Mesh::returnAllTriangles() {
    for (unsigned i=0; i<triangles.size(); i++) {
        returnTriangle(i);
    }
}

void Mesh::setSquareTextureMapping(unsigned indexTriangle0,
                                   unsigned indexTriangle1,
                                   float uMin,
                                   float uMax,
                                   float vMin,
                                   float vMax) {
    Triangle &triangle0 = triangles[indexTriangle0];
    Triangle &triangle1 = triangles[indexTriangle1];
    // Be able to detect equals triangles
    unsigned int commonIndexes0[3];
    unsigned int commonIndexes1[3];
    int found = 0;
    for (unsigned int i=0; i<3; i++) {
        for (unsigned int j=0; j<3; j++) {
            if (triangle0.getVertex(i) == triangle1.getVertex(j)) {
                commonIndexes0[found] = i;
                commonIndexes1[found] = j;
                found++;
                break;
            }
        }
    }
    if (found != 2) {
        cerr<<__FUNCTION__<<": the two triangles are not a square !"<<endl;
        return;
    }
    triangle0.setUV(commonIndexes0[0], uMin, vMin);
    triangle1.setUV(commonIndexes1[0], uMin, vMin);
    triangle0.setUV(commonIndexes0[1], uMax, vMax);
    triangle1.setUV(commonIndexes1[1], uMax, vMax);
    triangle0.setUV(3-commonIndexes0[0]-commonIndexes0[1], uMin, vMax);
    triangle1.setUV(3-commonIndexes1[0]-commonIndexes1[1], uMax, vMin);
}

Mesh Mesh::loadCube() {
    Mesh cube;
    cube.vertices.resize(8);
    cube.triangles.resize(12);

    for (unsigned int i=0; i<8; i++) {
        float x = (i%4)<2?-0.5:0.5;
        float y = ((i+1)%4)<2?0.5:-0.5;
        float z = i<4?0.5:-0.5;
        cube.vertices[i] = Vertex(Vec3Df(x, y, z));
    }

    // Top
    cube.triangles[8] = Triangle(0, 1, 2);
    cube.triangles[9] = Triangle(0, 2, 3);

    // Bottom
    cube.triangles[10] = Triangle(5, 4, 7);
    cube.triangles[11] = Triangle(5, 7, 6);

    // Sides
    for (unsigned i=0; i<4; i++) {
        cube.triangles[2*i] = Triangle(i, i+4, ((i+1)%4)+4);
        cube.triangles[2*i+1] = Triangle(i, ((i+1)%4)+4, (i+1)%4);
    }

    return cube;
}

void Mesh::setCubeTextureMapping(const Material *mat, unsigned widthGappedPixels, unsigned heightGappedPixels) {
    // Box size in texture space
    float boxWidth = 0.25;
    float boxHeight = 1.0/3.0;

    // Pixel size in both dimensions
    // to avoid JPG glitches
    float pixelHeight = 0;
    float pixelWidth = 0;
    const ImageTexture *texture = dynamic_cast<const ImageTexture*>(mat->getTexture());
    if (texture) {
        pixelHeight = 1.0/(float)texture->getImage()->height();
        pixelWidth = 1.0/(float)texture->getImage()->width();
    }

    // Sides
    for (unsigned int side=0; side<4; side++) {
        setSquareTextureMapping(
                2*side,
                2*side+1,
                (float)side*boxWidth,
                (float)(side+1)*boxWidth,
                boxHeight+heightGappedPixels*pixelHeight,
                2.0*boxHeight-heightGappedPixels*pixelHeight);
    }
    // TOP (side 4)
    setSquareTextureMapping(
            8,
            9,
            boxWidth+widthGappedPixels*pixelWidth,
            2.0*boxWidth-widthGappedPixels*pixelWidth,
            0,
            boxHeight);
    // BOTTOM (side 5)
    setSquareTextureMapping(
            10,
            11,
            boxWidth+widthGappedPixels*pixelWidth,
            2.0*boxWidth-widthGappedPixels*pixelWidth,
            2.0*boxHeight,
            3.0*boxHeight);
}
