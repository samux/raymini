// *********************************************************
// OpenGL Viewer Class, based on LibQGLViewer, compatible
// with most hardware (OpenGL 1.2).
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#pragma once

#include <GL/glew.h>
#include <QGLViewer/qglviewer.h>
#include <vector>
#include <string>

#include "Scene.h"
#include "Material.h"
#include "Object.h"
#include "Observer.h"

class Controller;

class GLViewer : public QGLViewer, public Observer  {
    Q_OBJECT
public:
    GLViewer(Controller *);
    virtual ~GLViewer ();

    class Exception  {
    public:
        Exception (const std::string & msg) : message ("[GLViewer]"+msg) {}
        virtual ~Exception () {}
        const std::string & getMessage () const { return message; }
    private:
        std::string message;
    };

public:
    virtual void update(Observable *);

protected :
    void init();
    void draw ();
    QString helpString() const;

    virtual void keyPressEvent (QKeyEvent * event);
    virtual void keyReleaseEvent (QKeyEvent * event);
    virtual void mousePressEvent (QMouseEvent * event);
    virtual void wheelEvent (QWheelEvent * e);

private:
    Controller *controller;

    void updateLights();
    void updateWireframe();

    void changeFocusPoint();
};

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
