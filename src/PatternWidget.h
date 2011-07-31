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
    
    void EnterAddMode();
    void LeaveAddMode();
    
    void DeleteSelected();
    
    sigc::signal<void> on_add_mode_left;
    
    void ClearSelection();
    
    void SetSnap(bool s);
    bool GetSnap();
    
    void SetVelocity(int v);
    /**Emitted when selection is changed. Provides an argument that is equal to number of notes in selection.*/
    sigc::signal<void,int> on_selection_changed;
    
    int velocity;
protected:
    //Override default signal handler:
   virtual bool on_expose_event(GdkEventExpose* event);
    
   virtual bool on_button_press_event(GdkEventButton* event);
   virtual bool on_button_release_event(GdkEventButton* event);
   
   virtual void on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context);
   virtual bool on_motion_notify_event(GdkEventMotion* event);
   virtual bool on_leave_notify_event(GdkEventCrossing* event);
private:
    bool add_mode;
    
    std::set<int> selection;
    
    bool snap;
    std::set<int> drag_temporary_selection;
    int drag_beggining_x, drag_beggining_y;
    int drag_beggining_line;
    double drag_beggining_time;
    int drag_current_x, drag_current_y;
    int drag_current_line;
    double drag_current_time;
    bool drag_in_progress;
    int drag_mode;
    int drag_note_dragged;
    double drag_time_offset_to_dragged_note;
    enum DragModes{
        DRAG_MODE_MOVE_SELECTION,
        DRAG_MODE_SELECT_AREA
    };
    
    void UpdateSizeRequest();
    int CalculateAverageVelocity();
    double Snap(double t);
    void DeleteNth(int n);
    int internal_height;
    double vert_size; //used to controll zooming
    AtomContainer* container;
    
    void InitDrag();
    void ProcessDrag(double x, double y,bool shift_key=false);
    
};

#endif	/* PATTERNWIDGET_H */

