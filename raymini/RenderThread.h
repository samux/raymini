#pragma once

#include <QThread>
#include <QImage>
#include <QTime>
#include <QMutex>

#include "Vec3D.h"
#include "Observable.h"

class Controller;

/** A thread computing ray traced image */
class RenderThread: public QThread, public Observable {
public:
    static const unsigned long RENDER_CHANGED = 1<<0;

    RenderThread(Controller *);
    /** Change RENDER_CHANGED */
    void startRendering(const Vec3Df & camPos,
                   const Vec3Df & viewDirection,
                   const Vec3Df & upVector,
                   const Vec3Df & rightVector,
                   float fieldOfView,
                   float aspectRatio,
                   unsigned int screenWidth,
                   unsigned int screenHeight);

    bool isRendering() const;
    bool hasRendered();
    /** Might change RENDER_CHANGED */
    void stopRendering();
    const QImage &getLastRendered();
    inline float getPercent() const {return percent;}
    /** Change RENDER_CHANGED */
    void setPercent(float p);

    bool isEmergencyStop() const;

    /** Notify thread that it has to render again */
    void hasToRedraw();

    inline bool isReallyWorking() const {return reallyWorking;}

    void run();
private:
    Controller *controller;

    // Result
    QImage resultImage;

    // Params
    Vec3Df camPos;
    Vec3Df viewDirection;
    Vec3Df upVector;
    Vec3Df rightVector;
    float fieldOfView;
    float aspectRatio;
    unsigned int screenWidth;
    unsigned int screenHeight;
    float percent;

    // Thread attributes
    bool emergencyStop;
    QMutex emergencyStopMutex;
    QTime time;
    bool haveToRedraw;
    bool optimalDone;
    QMutex hasToRedrawMutex;
    QMutex reallyWorkingMutex;
    bool reallyWorking;

    inline void prepare(const Vec3Df & camPos,
            const Vec3Df & viewDirection,
            const Vec3Df & upVector,
            const Vec3Df & rightVector,
            float fieldOfView,
            float aspectRatio,
            unsigned int screenWidth,
            unsigned int screenHeight) {
        this->camPos = camPos;
        this->viewDirection = viewDirection;
        this->upVector = upVector;
        this->rightVector = rightVector;
        this->fieldOfView = fieldOfView;
        this->aspectRatio = aspectRatio;
        this->screenWidth = screenWidth;
        this->screenHeight = screenHeight;
    }
};
