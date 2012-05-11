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

class Material;

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

    /**
     * Scale object against a given axis
     * X:0, Y:1, Z:2
     */
    void scale(const float &s, unsigned axis);


    /** Scale object */
    void scale(const float &s);

    /** Return a triangle */
    void returnTriangle(unsigned int t);

    /** Return all triangles */
    void returnAllTriangles();

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
    void setSquareTextureMapping(unsigned t0=0,
                                 unsigned t1=1,
                                 float uMin=0,
                                 float uMax=1,
                                 float vMin=0,
                                 float vMax=1);

    // Texture mapping coordinates
    inline float getU(unsigned int t, unsigned int i) const {return triangles[t].getU(i);}
    inline float getV(unsigned int t, unsigned int i) const {return triangles[t].getV(i);}
    inline void setU(unsigned int t, unsigned int i, float u) {triangles[t].setU(i, u);}
    inline void setV(unsigned int t, unsigned int i, float v) {triangles[t].setV(i, v);}
    inline void setUV(unsigned int t, unsigned int i, float u, float v) {triangles[t].setUV(i, u, v);}

    /**
     * Load a cube with those attributes:
     *
     * - centered in 0,0,0
     *
     * - size: 1x1x1
     *
     * - vertices repartition:
     *   
     *   0-3
     *   |\|
     * 0-1-2-3-0
     * |\|\|\|\|
     * 4-5-6-7-4
     *   |\|
     *   4-7
     *
     * - triangles repartition:
     *
     *       -------
     *       |08\09|
     * -------------------------
     * |00\01|02\03|04\05|06\07|
     * -------------------------
     *       |10\11|
     *       -------
     */
    void loadCube();

    /**
     * Set texture mapping for a cube
     *
     * Texture/sides mapping:
     *
     *   ---
     *   |4|
     * ---------
     * |0|1|2|3|
     * ---------
     *   |5|
     *   ---
     *
     *   Gapped pixels are the count of pixels to gap,
     *   usefull when using a low-res image like a JPG,
     *   which has glitches at texture side/black side junction
     */
    void setCubeTextureMapping(const Material *mat, unsigned widthGappedPixels=0, unsigned heightGappedPixels=0);

    /**
     * Load a square mesh of size 1, centered in 0, horizontal and facing up
     */
    void loadSquare();

    /*
     * uScale and vScale can be used to refine a texture, eg a texture will be used two times
     * in width if uScale == 2.0
     * Equals 1.0 by default
     */
    inline void setUScale(float u) {uScale = u;}
    inline void setVScale(float v) {vScale = v;}
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
