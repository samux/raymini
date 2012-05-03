#pragma once

#include <vector>

#include "BoundingBox.h"
#include "Ray.h"

class Object;

enum Axis {X = 0, Y = 1, Z = 2, NONE = -1};

class KDtree {
protected:
    const Object &o;
    std::vector<unsigned> triangles;// sth only if leaf;
    KDtree *left, *right;
    Axis splitAxis;

public:
    static const unsigned MIN_TRIANGLES = 20;
    const BoundingBox bBox;

    KDtree(const Object &o);

    ~KDtree() {
        delete left;
        delete right;
    }

    Axis getSplitAxis() const { return splitAxis; };
    std::tuple<const KDtree*, const KDtree*> getSons() const {
        return std::make_tuple(left, right);
    }
    const KDtree* getLeft() const { return left; }
    const KDtree* getRight() const { return right; }
    const std::vector<unsigned> &  getTriangles() const { return triangles; }

    bool exec(bool (*f)(const KDtree *)) const {
        return f(this) && (splitAxis == Axis::NONE ||
                           (left->exec(f) && right->exec(f)));
    }
    void exec(void (*f)(const KDtree *)) const {
        f(this);
        if(splitAxis != Axis::NONE) {
            left->exec(f);
            right->exec(f);
        }
    }
    void execLeaf(void (*f)(const KDtree *)) const {
        if(splitAxis != Axis::NONE) {
            left->exec(f);
            right->exec(f);
        }
        else
            f(this);
    }

    bool intersect(Ray &ray) const;

private:
    KDtree(const Object &o, const std::vector<unsigned> &triangles,
           const BoundingBox &boundingBox):
        o(o), triangles(triangles),
        left(nullptr), right(nullptr),
        splitAxis(Axis::NONE),
        bBox(boundingBox) {
        next();
    }

    KDtree & operator=(const KDtree &t) = delete;

    void next();

    inline void findSplitAxis();
    inline void splitTriangles(const BoundingBox & lb, const BoundingBox & rb,
                               std::vector<unsigned> &left, std::vector<unsigned> &right) const;
};
