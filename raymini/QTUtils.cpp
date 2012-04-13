// **************************************************
// QTUtils module.
// Author : Tamy Boubekeur (boubek@gamil.com).
// Copyright (C) 2008 Tamy Boubekeur.
// All rights reserved.
// **************************************************

#include "QTUtils.h"

#include <cmath>

#include <QHBoxLayout>

IntegerWidget::IntegerWidget (const QString & name,
                              int minValue, 
                              int maxValue, 
                              int value,
                              QWidget * parent) : QWidget (parent) {
    label = new QLabel (name, parent);
    
    slider = new QSlider (Qt::Horizontal, this);
    slider->setMinimum (minValue);
    slider->setMaximum (maxValue);
    slider->setValue (value);
    
    unsigned int numOfDigits = static_cast<unsigned int>(log10 (maxValue)) + 1; 
    LCDNumber = new QLCDNumber (numOfDigits, this);
    LCDNumber->display (value);
    LCDNumber->setSegmentStyle (QLCDNumber::Flat);
    LCDNumber->setFrameShape (QFrame::NoFrame);

    QHBoxLayout * layout = new QHBoxLayout (this);
    layout->addWidget (label);
    layout->addWidget (slider);
    layout->addWidget (LCDNumber);

    connect (slider, SIGNAL (valueChanged (int)),
             LCDNumber, SLOT (display (int)));
    connect (slider, SIGNAL (valueChanged (int)),
             this, SIGNAL (valueChanged (int)));
}

IntegerWidget::~IntegerWidget () {

}

DoubleWidget::DoubleWidget (const QString & name,
                            double minValue, 
                            double maxValue, 
                            double value,
                            QWidget * parent) : QWidget (parent) {
    label = new QLabel (name, parent);
    
    slider = new QSlider (Qt::Horizontal, this);
    slider->setMinimum (1000*minValue);
    slider->setMaximum (1000*maxValue);
    slider->setValue (1000*value);
        
    unsigned int numOfDigits = 3; 
    LCDNumber = new QLCDNumber (numOfDigits, this);
    LCDNumber->setSegmentStyle (QLCDNumber::Flat);
    LCDNumber->setFrameShape (QFrame::NoFrame);
    LCDNumber->setSmallDecimalPoint (true);
    LCDNumber->display (value);
    
    QHBoxLayout * layout = new QHBoxLayout (this);
    layout->addWidget (label);
    layout->addWidget (slider);
    layout->addWidget (LCDNumber);

    connect (slider, SIGNAL (valueChanged (int)),
             this, SLOT (changeValue (int)));
}

DoubleWidget::~DoubleWidget () {

}

void DoubleWidget::changeValue (int i) {
    double s = i/1000.0;
    LCDNumber->display (s);
    emit valueChanged (s);
}


void setBoubekQTStyle (QApplication & app) {
    QPalette p (app.palette());
#ifdef _MSC_VER
    p.setColor (QPalette::Background, QColor (240, 240, 255)); // general background
    p.setColor (QPalette::WindowText, QColor (50, 50, 50)); // general foreground
    p.setColor (QPalette::Base, QColor (60, 60, 60)); // text entry widget
    p.setColor (QPalette::AlternateBase, QColor (238, 112, 0)); // Text used on 'Base'
    p.setColor (QPalette::Text, QColor (145, 145, 0)); // Text used on 'Base'
    p.setColor (QPalette::Button, QColor (240, 240, 255)); // general bg button color
    p.setColor (QPalette::ButtonText, QColor (48, 48, 48)); // general foreground button color
    p.setColor (QPalette::BrightText, QColor (238, 112, 0)); // to ensure contrast
    p.setColor (QPalette::Highlight, QColor (0, 0, 255)); // marked element
    p.setColor (QPalette::HighlightedText, QColor (255, 255, 0)); // marked element text
#else
    p.setColor (QPalette::Background, QColor (48, 48, 48/*39, 39, 39*/)); // general background
    p.setColor (QPalette::WindowText, QColor (203, 203, 203)); // general foreground
    p.setColor (QPalette::Base, QColor (60, 60, 60)); // text entry widget
    p.setColor (QPalette::AlternateBase, QColor (238, 112, 0)); // Text used on 'Base'
    p.setColor (QPalette::ToolTipBase, QColor (48, 48, 48));
    p.setColor (QPalette::ToolTipText, QColor (48, 48, 48));
    p.setColor (QPalette::Text, QColor (145, 145, 145)); // Text used on 'Base'
    p.setColor (QPalette::Button, QColor (48, 48, 48)); // general bg button color
    p.setColor (QPalette::ButtonText, QColor (203, 203, 203)); // general foreground button color
    p.setColor (QPalette::BrightText, QColor (238, 112, 0)); // to ensure contrast
    p.setColor (QPalette::Highlight, QColor (238, 112, 0)); // marked element
    p.setColor (QPalette::HighlightedText, QColor (31, 13, 0)); // marked element text
    p.setColor (QPalette::Light, QColor (100, 100, 100)); // marked element
#endif
    app.setPalette(p);
}
