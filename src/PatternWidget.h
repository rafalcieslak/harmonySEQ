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
#include <set>
#include "AtomContainer.h"
/**PatternWidget is basically a GUI for NoteContainer, that uses DrawingArea to display a piano-roll interface.*/
class PatternWidget : public Gtk::DrawingArea {
public:
    PatternWidget();
    virtual ~PatternWidget();
    void SetInternalHeight(int h);
    
    void Redraw();
    
    void AssignPattern(AtomContainer* cont);
    
    void ZoomIn();
    void ZoomOut();
    
    void ClearSelection();
protected:
    //Override default signal handler:
   virtual bool on_expose_event(GdkEventExpose* event);
    
   virtual bool on_button_press_event(GdkEventButton* event);
   
   virtual void on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context);
private:
    
    std::set<int> selection;
    
    void UpdateSizeRequest();
    
    int internal_height;
    double vert_size; //used to controll zooming
    AtomContainer* container;
    
};

#endif	/* PATTERNWIDGET_H */

