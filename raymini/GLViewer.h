// *********************************************************
// OpenGL Viewer Class, based on LibQGLViewer, compatible
// with most hardware (OpenGL 1.2).
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef GLVIEWER_H
#define GLVIEWER_H

#include <GL/glew.h>
#include <QGLViewer/qglviewer.h>
#include <vector>
#include <string>

#include "Scene.h"

class GLViewer : public QGLViewer  {
    Q_OBJECT
public:
    
    typedef enum {Flat=0, Smooth=1} RenderingMode;
    typedef enum {OpenGLDisplayMode=0, RayDisplayMode=1} DisplayMode;
    
    GLViewer ();
    virtual ~GLViewer ();
    
    inline bool isWireframe () const { return wireframe; }
    inline int getRenderingMode () const { return renderingMode; }
    inline const QImage & getRayImage () const { return rayImage; }
    
    class Exception  {
    public:
        Exception (const std::string & msg) : message ("[GLViewer]"+msg) {}
        virtual ~Exception () {}
        const std::string & getMessage () const { return message; }
    private:
        std::string message;
    }; 
     
public slots :
    void setWireframe (bool b);
    void setRenderingMode (RenderingMode m);
    void setRenderingMode (int m) { setRenderingMode (static_cast<RenderingMode>(m)); }
    void setDisplayMode (DisplayMode m);
    void setDisplayMode (int m) { setRenderingMode (static_cast<DisplayMode>(m)); }
    void setRayImage (const QImage & image);
    
protected :
    void init();
    void draw ();
    QString helpString() const;

    virtual void keyPressEvent (QKeyEvent * event);
    virtual void keyReleaseEvent (QKeyEvent * event);
    virtual void mousePressEvent (QMouseEvent * event);
    virtual void wheelEvent (QWheelEvent * e);

private:
    bool wireframe;
    RenderingMode renderingMode;
    DisplayMode displayMode;
    QImage rayImage;
};

#endif // GLVIEWER_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
