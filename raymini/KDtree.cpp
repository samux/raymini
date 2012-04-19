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
    splitTriangles(lb, rb, lt, rt);

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

void KDtree::splitTriangles(const BoundingBox & lb, const BoundingBox & rb,
                            std::vector<unsigned> &left, std::vector<unsigned> &right) const {
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

bool KDtree::intersect(const Ray &ray, Vertex & intersectionPoint, const Object & o, const Vec3Df & camPos) const {
    const Mesh & mesh = o.getMesh();
    const Vec3Df & transform = o.getTrans();

    if(splitAxis ==  Axis::NONE) {
        float smallestIntersectionDistance = 1000000.f;
        bool hasIntersection = false;
        for(unsigned idT : triangles) {
            const Triangle & t = mesh.getTriangles()[idT];
            const Vertex & v0 = mesh.getVertices() [t.getVertex(0)];
            const Vertex & v1 = mesh.getVertices() [t.getVertex(1)];
            const Vertex & v2 = mesh.getVertices() [t.getVertex(2)];
            Vertex intersection;
            bool testIntersection = ray.intersect(v0, v1, v2, intersection);
            if (testIntersection) {
                hasIntersection = true;
                float intersectionDistance = Vec3Df::squaredDistance (intersection.getPos() + transform, camPos);
                if(intersectionDistance < smallestIntersectionDistance) {
                    smallestIntersectionDistance = intersectionDistance;
                    intersectionPoint = intersection;
                }
            }
        }
        return hasIntersection;
    }
    else {
        Vec3Df lI, rI;
        bool leftIntersection = ray.intersect(left->bBox, lI);
        bool rightIntersection = ray.intersect(right->bBox, rI);

        if(!rightIntersection &&  !leftIntersection)
            return false;

        if(leftIntersection && rightIntersection) {
            Vertex lIi, rIi;

            leftIntersection = left->intersect(ray, lIi, o, camPos);
            rightIntersection = right->intersect(ray, rIi, o, camPos);

            if(!leftIntersection) {
                intersectionPoint = rIi;
                return rightIntersection;
            }
            if(!rightIntersection) {
                intersectionPoint = lIi;
                return leftIntersection;
            }

            float iDistanceL = Vec3Df::squaredDistance (lIi.getPos() + transform, camPos);
            float iDistanceR = Vec3Df::squaredDistance (rIi.getPos() + transform, camPos);

            if(iDistanceL < iDistanceR)
                intersectionPoint = lIi;
            else
                intersectionPoint = rIi;

            return true;
        }

        if(leftIntersection)
                return left->intersect(ray, intersectionPoint, o, camPos);
        else
                return right->intersect(ray, intersectionPoint, o, camPos);
    }
}

