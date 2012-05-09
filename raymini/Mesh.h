// ---------------------------------------------------------
// Mesh Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2008 Tamy Boubekeur.
// All rights reserved.
// ---------------------------------------------------------

#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>

#include "Vertex.h"
#include "Triangle.h"
#include "Edge.h"

class Mesh {
public:
    inline Mesh (): uScale(1), vScale(1) {}
    inline Mesh (const std::vector<Vertex> & v):
        vertices(v),
        uScale(1),
        vScale(1)
    {}
    inline Mesh (const std::vector<Vertex> & v,
                 const std::vector<Triangle> & t):
        vertices(v),
        triangles(t),
        uScale(1),
        vScale(1)
    {}
    inline Mesh (const Mesh & mesh):
        vertices(mesh.vertices),
        triangles (mesh.triangles),
        uScale(mesh.uScale),
        vScale(mesh.vScale)
    {}

    inline virtual ~Mesh () {}
    std::vector<Vertex> & getVertices () { return vertices; }
    const std::vector<Vertex> & getVertices () const { return vertices; }
    std::vector<Triangle> & getTriangles () { return triangles; }
    const std::vector<Triangle> & getTriangles () const { return triangles; }
    void clear ();
    void clearGeometry ();
    void clearTopology ();
    void unmarkAllVertices ();
    void recomputeSmoothVertexNormals (unsigned int weight);
    void computeTriangleNormals (std::vector<Vec3Df> & triangleNormals);
    void collectOneRing (std::vector<std::vector<unsigned int> > & oneRing) const;
    void collectOrderedOneRing (std::vector<std::vector<unsigned int> > & oneRing) const;
    void computeDualEdgeMap (EdgeMapIndex & dualVMap1, EdgeMapIndex & dualVMap2);
    void markBorderEdges (EdgeMapIndex & edgeMap);

    void renderGL (bool flat) const;

    void loadOFF (const std::string & filename);

    /** Rotate all vertices */
    void rotate(const Vec3Df &axis, const float &angle);

    /** Scale object */
    void scale(const float &s);

    class Exception {
    private:
        std::string msg;
    public:
        Exception (const std::string & msg) : msg ("[Mesh Exception]" + msg) {}
        virtual ~Exception () {}
        inline const std::string & getMessage () const { return msg; }
    };

    /** Give vertices default spherical texture mapping */
    void setDefaultTextureMapping(bool useNormals=true);

    /** Give two triangles the whole texture mapped */
    void setSquareTextureMapping(unsigned t0=0, unsigned t1=1);

    // Texture mapping coordinates
    inline float getU(unsigned int t, unsigned int i) const {return triangles[t].getU(i);}
    inline float getV(unsigned int t, unsigned int i) const {return triangles[t].getV(i);}
    inline void setU(unsigned int t, unsigned int i, float u) {triangles[t].setU(i, u);}
    inline void setV(unsigned int t, unsigned int i, float v) {triangles[t].setV(i, v);}
    inline void setUV(unsigned int t, unsigned int i, float u, float v) {triangles[t].setUV(i, u, v);}

    /*
     * uScale and vScale can be used to refine a texture, eg a texture will be used two times
     * in width if uScale == 2.0
     * Equals 1.0 by default
     */
    inline void setUVScales(float u, float v) {uScale = u; vScale = v;}
    inline float getUScale() const {return uScale;}
    inline float getVScale() const {return vScale;}

private:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;

    float uScale, vScale;
};

#endif // MESH_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
