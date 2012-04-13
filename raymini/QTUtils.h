// **************************************************
// QTUtils module.
// Author : Tamy Boubekeur (boubek@gamil.com).
// Copyright (C) 2008 Tamy Boubekeur.
// All rights reserved.
// **************************************************

#ifndef QTUTILS_H
#define QTUTILS_H

#include <QApplication>
#include <QLabel>
#include <QSlider>
#include <QLCDNumber>

class IntegerWidget : public QWidget {
    Q_OBJECT
public:
    IntegerWidget (const QString & name, int minValue, int maxValue, int value,
                   QWidget * parent = NULL);
    virtual ~IntegerWidget ();

    QString getName () const { return label->text (); }
    int getMin () const { return slider->minimum (); }
    int getMax () const { return slider->minimum (); }
    int getValue () const { return slider->value (); }

signals:
    void valueChanged (int);

private:
    QLabel * label;
    QSlider * slider;
    QLCDNumber * LCDNumber;
};

class DoubleWidget : public QWidget {
    Q_OBJECT
public:
    DoubleWidget (const QString & name, double minValue, double maxValue, double value,
                   QWidget * parent = NULL);
    virtual ~DoubleWidget ();

    QString getName () const { return label->text (); }
    double getMin () const { return slider->minimum (); }
    double getMax () const { return slider->minimum (); }
    double getValue () const { return slider->value (); }

signals:
    void valueChanged (double);

private slots:
    void changeValue (int i);

private:
    QLabel * label;
    QSlider * slider;
    QLCDNumber * LCDNumber;
};


void setBoubekQTStyle (QApplication & app);

#endif // QTUTILS_H


// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
