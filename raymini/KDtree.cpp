#include "KDtree.h"
#include "Object.h"

using namespace std;

KDtree::KDtree(const Object &o):mesh(o.getMesh()),
                                left(nullptr), right(nullptr),
                                splitAxis(Axis::NONE),
                                bBox(o.getBoundingBox()) {
    triangles.resize(mesh.getTriangles().size());
    for(unsigned int i = 0 ; i < mesh.getTriangles().size() ; i++)
        triangles[i] = i;
    next();
}

void KDtree::next() {
    if(triangles.size() <= MIN_TRIANGLES) return;//leaf

    findSplitAxis();
    float med = bBox.getMiddle(splitAxis);

    BoundingBox lb, rb;
    bBox.split(med, splitAxis, lb, rb);

    vector<unsigned> lt, rt;
    splitTriangles(med, lt, rt);

    triangles.clear();//not a leaf

    left = new KDtree(mesh, lt, lb);
    right = new KDtree(mesh, rt, rb);
}

void KDtree::findSplitAxis() {
    Vec3Df delta = bBox.getMax()-bBox.getMin();

    if(delta[0] <= delta[1]) {
        if(delta[1] <= delta[2])
            splitAxis = Axis::Z;
        else
            splitAxis = Axis::Y;
    }
    else {
        if(delta[0] <= delta[2])
            splitAxis = Axis::Z;
        else
            splitAxis = Axis::X;
    }
}

void KDtree::splitTriangles(float med, std::vector<unsigned> &left, std::vector<unsigned> &right) const {
    for(unsigned t : triangles) {
        bool isInLeft = false;
        bool isInRight = false;

        for(unsigned i = 0 ; i<3 ; i++) {
            unsigned v = mesh.getTriangles()[t].getVertex(i);
            if(mesh.getVertices()[v].getPos()[splitAxis] <= med) {
                isInLeft = true;
            }
            else
                isInRight = true;
        }

        if(isInLeft)
            left.push_back(t);
        if(isInRight)
            right.push_back(t);
    }
}

