#include "KDtree.h"
#include "Object.h"

using namespace std;

KDtree::KDtree(Object &o):o(o),
                                left(nullptr), right(nullptr),
                                splitAxis(Axis::NONE),
                                bBox(o.getBoundingBox()) {
    const Mesh & mesh = o.getMesh();
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
    splitTriangles(lb, rb, lt, rt);

    triangles.clear();//not a leaf

    left = new KDtree(o, lt, lb);
    right = new KDtree(o, rt, rb);
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

void KDtree::splitTriangles(const BoundingBox & lb, const BoundingBox & rb,
                            std::vector<unsigned> &left, std::vector<unsigned> &right) const {
    const Mesh & mesh = o.getMesh();
    for(unsigned t : triangles) {
        bool isInLeft = false;
        bool isInRight = false;

        for(unsigned i = 0 ; i<3 ; i++) {
            unsigned v = mesh.getTriangles()[t].getVertex(i);
            const Vec3Df & p = mesh.getVertices()[v].getPos();
            if(lb.contains(p))
                isInLeft = true;
            else if(rb.contains(p))
                isInRight = true;
        }

        if(isInLeft)
            left.push_back(t);
        if(isInRight)
            right.push_back(t);
    }
}

bool KDtree::intersect(Ray &ray) const {
    const Mesh & mesh = o.getMesh();

    if(splitAxis ==  Axis::NONE) {
        for(unsigned idT : triangles) {
            const Triangle & t = mesh.getTriangles()[idT];
            const Vertex & v0 = mesh.getVertices() [t.getVertex(0)];
            const Vertex & v1 = mesh.getVertices() [t.getVertex(1)];
            const Vertex & v2 = mesh.getVertices() [t.getVertex(2)];

            ray.intersect(t, v0, v1, v2, &o);
        }
        return ray.intersect();
    }
    else {
        Vec3Df lI, rI;
        bool leftIntersection = ray.intersect(left->bBox, lI);
        bool rightIntersection = ray.intersect(right->bBox, rI);

        if(leftIntersection && rightIntersection) {
            KDtree * far;
            KDtree * near;

            float iDistanceL = Vec3Df::squaredDistance (lI, ray.getOrigin());
            float iDistanceR = Vec3Df::squaredDistance (rI, ray.getOrigin());

            far = (iDistanceR > iDistanceL) ? right : left;
            near = (iDistanceR > iDistanceL) ? left : right;

            return near->intersect(ray) || far->intersect(ray);
        }

        return (leftIntersection && left->intersect(ray)) || (rightIntersection && right->intersect(ray));
    }
}

