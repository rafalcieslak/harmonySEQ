/*
    Copyright (C)  2011 Rafał Cieślak

    This file is part of harmonySEQ.

    HarmonySEQ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    HarmonySEQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with HarmonySEQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PATTERNWIDGET_H
#define	PATTERNWIDGET_H
#include "gtkmm.h"
/**PatternWidget is basically a GUI for NoteContainer, that uses DrawingArea to display a piano-roll interface.*/
class PatternWidget : public Gtk::DrawingArea {
public:
    PatternWidget();
    virtual ~PatternWidget();
    void SetInternalHeight(int h);
    
    void SetResolution(int r);
    int GetResoution();
    
    void Redraw();
    
protected:
    //Override default signal handler:
  virtual bool on_expose_event(GdkEventExpose* event);
    
private:
    int internal_height;
    
    int resolution;
    
};

#endif	/* PATTERNWIDGET_H */

