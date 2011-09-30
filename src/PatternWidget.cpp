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
#include <cairo/cairo.h>

#include "PatternWidget.h"
#include "cairomm/context.h"
#include "global.h"
#include "messages.h"
#include "AtomContainer.h"
#include "NoteAtom.h"
#include "Sequencer.h"
#include "Event.h"
#include "ControllerAtom.h"
#include "Configuration.h"
#include "Files.h"

int resolution_hints[65] = {1,1,2,3,2,5,3,7,4,3,5,11,3,13,7,5,4,17,6,19,5,7,11,23,6,5,13,9,7,29,5,31,4,
                                                               11,17,5,6,37,19,39,10,41,6,43,11,7,23,47,6,49,5,51,13,53,27,11,7,57,29,59,6,61,31,63,8};

//constants used for selecting ctrl atoms
const double ctrl_Vsurrounding = 8.0;
const double crtl_Tsurrounding = 8.0;

const double handle_size = 15.0;

PatternWidget::PatternWidget(){
    internal_height=50; //random guess. will be reset soon anyway by the SequencerWidget, but better protect from 0-like values.
    horiz_size = 450.0; //adjust for better default size
    snap = true;
    add_slope_type = SLOPE_TYPE_LINEAR;
    add_events(Gdk::BUTTON_PRESS_MASK);
    add_events(Gdk::SCROLL_MASK);
    add_events(Gdk::BUTTON_RELEASE_MASK);
    add_events(Gdk::BUTTON1_MOTION_MASK);
    add_events(Gdk::BUTTON2_MOTION_MASK);
    add_events(Gdk::BUTTON3_MOTION_MASK);
    add_events(Gdk::KEY_PRESS_MASK);
    set_can_focus(1);//required to receive key_presses
    cr_buffer_surface = Cairo::ImageSurface::create(Cairo::FORMAT_RGB24,2400,300);
    cr_buffer_context = Cairo::Context::create(cr_buffer_surface); 
    cr_atoms_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32,2400,300);
    cr_atoms_context = Cairo::Context::create(cr_atoms_surface); 
    cr_grid_surface = Cairo::ImageSurface::create(Cairo::FORMAT_RGB24,2400,300);
    cr_grid_context = Cairo::Context::create(cr_grid_surface); 
    grid_lock = 0;
    atoms_lock = 0;
    man_i_wanted_to_redraw_atoms_but_it_was_locked_could_you_please_do_it_later_for_me = 0;
    man_i_wanted_to_redraw_grid_but_it_was_locked_could_you_please_do_it_later_for_me = 0;
    last_drawn_height = 0;
    last_drawn_width = 0;
    
    button_pressed = NONE;
    drag_in_progress = 0;
    select_only_this_atom_on_LMB_release = NULL;
}

PatternWidget::~PatternWidget(){
}

void PatternWidget::SetInternalHeight(int h){
    internal_height = h;
    UpdateSizeRequest();
}

void PatternWidget::UpdateSizeRequest(){
    //*dbg << "sizerequest " << vert_size << " " << internal_height+20 << ENDL;
    set_size_request(horiz_size,internal_height);
}

void PatternWidget::ZoomIn(){
    horiz_size = horiz_size*1.2;
    if(horiz_size > 2400.0)  horiz_size = 2400.0;
    UpdateSizeRequest();
}

void PatternWidget::ZoomOut(){
    horiz_size = horiz_size/1.2;
    if(horiz_size < 150.0)  horiz_size = 150.0;
    UpdateSizeRequest();
}

int PatternWidget::GetSelectionSize(){
    return selection.size();
}


void PatternWidget::AssignPattern(AtomContainer* cont, SeqType_t type){
    *dbg << "assigning pattern \n";
    container = cont;
    seq_type = type;
    RedrawGrid(); //ClearSelection will redraw atoms and everything anyway
    ClearSelection();
}

void PatternWidget::ClearSelection(){
    //Note that clearing selection must not delete the pointers - notes are still kept in aproprieate AtomContainer
    selection.clear();
    on_selection_changed.emit(0);
    RedrawAtoms();
}

void PatternWidget::DeleteNth(int n){
    /*container->Remove(n);
    std::set<int>::iterator it = selection.begin();
    for (; it != selection.end(); it++) {
        if(*it == n) selection.erase(it);
        if(*it < n) selection.;
    }   */ 
}

void PatternWidget::DeleteSelected(){
    container->RemoveList(&selection);
    for(std::set<Atom*, AtomComparingClass>::iterator it = selection.begin(); it != selection.end(); it++) delete *it;
    selection.clear();
    Files::FileModified();
    on_selection_changed.emit(0);
    RedrawAtoms();
}

void PatternWidget::SetSnap(bool s){
    snap = s;
}

bool PatternWidget::GetSnap(){
    return snap;
}

void PatternWidget::SetSelectionVelocity(int v){
    if(seq_type != SEQ_TYPE_NOTE) return;
    std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
    for (; it != selection.end(); it++) {
        NoteAtom* note = dynamic_cast<NoteAtom*>(*it);
        note->velocity = v;
    }    
    Files::FileModified();
    RedrawAtoms();
}

int PatternWidget::GetSelectionVelocity(){
    if(seq_type != SEQ_TYPE_NOTE) return -1;
    int sum = 0;
    std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
    for (; it != selection.end(); it++) {
        NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
        sum += note->velocity;
    }    
    if(selection.size()==0) return 0;
    return sum/(selection.size());
}

void PatternWidget::SetSelectionValue(int v){
    if(seq_type != SEQ_TYPE_CONTROL) return;
    std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
    for (; it != selection.end(); it++) {
        ControllerAtom* ctrl = dynamic_cast<ControllerAtom*>(*it);
        ctrl->value = v;
    }    
    Files::FileModified();
    RedrawAtoms();
}

int PatternWidget::GetSelectionValue(){
    if(seq_type != SEQ_TYPE_CONTROL) return -1;
    int sum = 0;
    std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
    for (; it != selection.end(); it++) {
        ControllerAtom* ctrl = dynamic_cast<ControllerAtom*> (*it);
        sum += ctrl->value;
    }    
    if(selection.size()==0) return 0;
    return sum/(selection.size());
}

SlopeType PatternWidget::GetSelectionSlopeType(){
    if(seq_type != SEQ_TYPE_CONTROL) return SLOPE_TYPE_NONE;
    SlopeType s = SLOPE_TYPE_NONE;
    std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
    for (; it != selection.end(); it++) {
        ControllerAtom* ctrl = dynamic_cast<ControllerAtom*> (*it);
        if(s == SLOPE_TYPE_NONE) s = ctrl->slope_type;
        else if (s != ctrl->slope_type){ s = SLOPE_TYPE_NONE; break;}
    }    
    return s;
}

void PatternWidget::SetSlopeType(SlopeType s){
    if(seq_type != SEQ_TYPE_CONTROL) return;
    if(s == SLOPE_TYPE_NONE) return; //does not apply
    if(selection.empty()){
        add_slope_type = s;
    }else{
        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        for (; it != selection.end(); it++) {
            ControllerAtom* ctrl = dynamic_cast<ControllerAtom*>(*it);
            ctrl->slope_type = s;
        }
        Files::FileModified();
        RedrawAtoms();
    }
}

SlopeType PatternWidget::GetSlopeType(){
    if(seq_type != SEQ_TYPE_CONTROL) return SLOPE_TYPE_NONE;
    if(!selection.empty())
        return GetSelectionSlopeType();
    else
        return add_slope_type;
}

void PatternWidget::MoveSelectionUp(){
    if(seq_type == SEQ_TYPE_NOTE){
        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        std::set<Atom *, AtomComparingClass> resulting_selection;
        for (; it != selection.end(); it++) {
            NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
            note->pitch = (note->pitch+1)%6;
            resulting_selection.insert(note);
        }
        selection = resulting_selection;
        //container->Sort(); //no need to sort, when pitch or value was changed
        Files::FileModified();
        RedrawAtoms();
    }else if (seq_type == SEQ_TYPE_CONTROL)
        IncreaseSelectionValue(8);
}

void PatternWidget::MoveSelectionDown(){
    if(seq_type == SEQ_TYPE_NOTE){
        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        std::set<Atom *, AtomComparingClass> resulting_selection;
        for (; it != selection.end(); it++) {
            NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
            note->pitch = (note->pitch-1)%6;
            if(note->pitch < 0) note->pitch += 6;
            resulting_selection.insert(note);
        }
        selection = resulting_selection;
        //container->Sort(); //no need to sort, when pitch or value was changed
        Files::FileModified();
        RedrawAtoms();
    }else if (seq_type == SEQ_TYPE_CONTROL)
        DecreaseSelectionValue(8);
}

void PatternWidget::MoveSelectionRight(){
        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        std::set<Atom *, AtomComparingClass> resulting_selection;
        for (; it != selection.end(); it++) {
            Atom* atm = (*it);
            atm->time = (atm->time+1.0/(double)container->owner->resolution);
            if(atm->time >= 1.0) atm->time -= 1.0;
            if(atm->time < 0.0) atm->time += 1.0;
            resulting_selection.insert(atm);
        }
        selection = resulting_selection;
        container->Sort();
        Files::FileModified();
        RedrawAtoms();
}

void PatternWidget::MoveSelectionLeft(){
        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        std::set<Atom *, AtomComparingClass> resulting_selection;
        for (; it != selection.end(); it++) {
            Atom* atm = (*it);
            atm->time = (atm->time-1.0/(double)container->owner->resolution);
            if(atm->time >= 1.0) atm->time -= 1.0;
            if(atm->time < 0.0) atm->time += 1.0;
            resulting_selection.insert(atm);
        }
        selection = resulting_selection;
        container->Sort();
        Files::FileModified();
        RedrawAtoms();
}

void PatternWidget::IncreaseSelectionVelocity(int amount){
        if(seq_type != SEQ_TYPE_NOTE) return;
        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        std::set<Atom *, AtomComparingClass> resulting_selection;
        for (; it != selection.end(); it++) {
            NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
            note->velocity = (note->velocity+amount);
            if(note->velocity > 127) note->velocity = 127;
            if(note->velocity < 0) note->velocity = 0;
            resulting_selection.insert(note);
        }
        selection = resulting_selection;
        //container->Sort(); //no need to sort, when velocity was changed
        Files::FileModified();
        on_selection_changed.emit(selection.size());//this will update the velocity spinbutton
        RedrawAtoms();
}

void PatternWidget::DecreaseSelectionVelocity(int amount){
        if(seq_type != SEQ_TYPE_NOTE) return;
        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        std::set<Atom *, AtomComparingClass> resulting_selection;
        for (; it != selection.end(); it++) {
            NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
            note->velocity = (note->velocity-amount);
            if(note->velocity > 127) note->velocity = 127;
            if(note->velocity < 0) note->velocity = 0;
            resulting_selection.insert(note);
        }
        selection = resulting_selection;
        //container->Sort(); //no need to sort, when velocity was changed
        Files::FileModified();
        on_selection_changed.emit(selection.size());//this will update the velocity spinbutton
        RedrawAtoms();
}

void PatternWidget::IncreaseSelectionValue(int amount){
        if(seq_type != SEQ_TYPE_CONTROL) return;
        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        std::set<Atom *, AtomComparingClass> resulting_selection;
        for (; it != selection.end(); it++) {
            ControllerAtom* ctrl = dynamic_cast<ControllerAtom*> (*it);
            ctrl->value = (ctrl->value+amount);
            if(ctrl->value > 127) ctrl->value = 127;
            if(ctrl->value < 0) ctrl->value = 0;
            resulting_selection.insert(ctrl);
        }
        selection = resulting_selection;
        //container->Sort(); //no need to sort, when value was changed
        Files::FileModified();
        on_selection_changed.emit(selection.size());//this will update the value spinbutton
        RedrawAtoms();
}

void PatternWidget::DecreaseSelectionValue(int amount){
        if(seq_type != SEQ_TYPE_CONTROL) return;
        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        std::set<Atom *, AtomComparingClass> resulting_selection;
        for (; it != selection.end(); it++) {
            ControllerAtom* ctrl = dynamic_cast<ControllerAtom*> (*it);
            ctrl->value = (ctrl->value-amount);
            if(ctrl->value > 127) ctrl->value = 127;
            if(ctrl->value < 0) ctrl->value = 0;
            resulting_selection.insert(ctrl);
        }
        selection = resulting_selection;
        //container->Sort(); //no need to sort, when value was changed
        Files::FileModified();
        on_selection_changed.emit(selection.size());//this will update the value spinbutton
        RedrawAtoms();
}


double PatternWidget::Snap(double t){
    if (!container->owner) // if no owner...
        return t; //do not snap, if we can't get the resolution.
    
    t*= container->owner->resolution;
    t += 0.5;
    if(t<0) t--;
    t = (int)t;
    t /= container->owner->resolution;
    return t;
}

double PatternWidget::SnapDown(double t){
    if (!container->owner) // if no owner...
        return t; //do not snap, if we can't get the resolution.
    
    t*= container->owner->resolution;
    if(t<0) t--;
    t = (int)t;
    t /= container->owner->resolution;
    return t;
}

void PatternWidget::InitDrag(){
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    //const int height = allocation.get_height();
    
    //calculate position
    double line_d = 6.0 - drag_beggining_y / ((double)internal_height / 6.0); 
    int line = line_d;
    double time = (double) drag_beggining_x / (double) width;
    int value = 127 - drag_beggining_y *(double)127.0/ (internal_height);
    
    if(button_pressed == LMB){
        
        drag_in_progress = 1;
        select_only_this_atom_on_LMB_release = NULL;
        
        if(drag_note_dragged != NULL){
            //check what we're dragging!
            if(seq_type == SEQ_TYPE_CONTROL){
                drag_mode = DRAG_MODE_MOVE_SELECTION;
                drag_beggining_line = line;
                drag_beggining_time = time;
                drag_beggining_value = value;
                drag_note_dragged = drag_note_dragged;
                drag_time_offset_to_dragged_note = drag_beggining_time - drag_note_dragged->time;
                //Store note offsets...
                std::set<Atom*>::iterator it2 = selection.begin();
                for (; it2 != selection.end(); it2++) {
                    Atom* atm = *it2;
                    atm->drag_offset_time = atm->time - time;
                    ControllerAtom* ctrl = dynamic_cast<ControllerAtom*> (atm);
                    ctrl->drag_offset_value = ctrl->value - value;
                }
            }else if(seq_type == SEQ_TYPE_NOTE){
                NoteAtom* note = dynamic_cast<NoteAtom*> (drag_note_dragged);
                double note_start_x = note->time*width;
                double note_end_x = (note->time+note->length)*width;
                double wl = note->pitch+0.5-line_d;
                if(wl>0 && 2.0*wl*handle_size >= note_end_x - drag_beggining_x){ 
                    //dragging handle
                    drag_mode = DRAG_MODE_RESIZE;
                    drag_beggining_line = line;
                    drag_beggining_time = time;
                    //Store note offsets...
                    std::set<Atom*>::iterator it = selection.begin();
                    for (; it != selection.end(); it++) {
                        NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
                        note->drag_beggining_length = note->length;
                    }
                }else if(drag_beggining_x < note_start_x + 10.0){
                    //dragging bar
                    drag_mode = DRAG_MODE_CHANGE_VELOCITY;
                    
                    //Store velocities
                    std::set<Atom*>::iterator it = selection.begin();
                    for (; it != selection.end(); it++) {
                        NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
                        note->drag_beggining_velocity = note->velocity;
                    }
                }else{
                    //dragging body
                    drag_mode = DRAG_MODE_MOVE_SELECTION;
                    drag_beggining_line = line;
                    drag_beggining_time = time;
                    drag_beggining_value = value;
                    drag_note_dragged = drag_note_dragged;
                    drag_time_offset_to_dragged_note = drag_beggining_time - drag_note_dragged->time;
                    //Store note offsets...
                    std::set<Atom*>::iterator it2 = selection.begin();
                    for (; it2 != selection.end(); it2++) {
                        Atom* atm = *it2;
                        atm->drag_offset_time = atm->time - time;
                        NoteAtom* note = dynamic_cast<NoteAtom*> (atm);
                        note->drag_offset_line = note->pitch - line;
                    }
                    
                }
            }
        }
    }else if(button_pressed == LMBs || button_pressed == MMB){
        
        drag_in_progress = 1;
        drag_mode = DRAG_MODE_SELECT_AREA;
        drag_beggining_line = line;
        drag_beggining_time = time;
        drag_beggining_value = value;
        drag_temporary_selection.clear();
        
        cancel_unselecting = 1;
    }else if(button_pressed == RMB){
        
        selection.clear();
        drag_in_progress = 1;
        drag_mode = DRAG_MODE_SELECT_AREA;
        drag_beggining_line = line;
        drag_beggining_time = time;
        drag_beggining_value = value;
        drag_temporary_selection.clear();
        
    }
}

void PatternWidget::ProcessDrag(double x, double y){
    
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    //const int height = allocation.get_height();
    
    //count position
    int line = 6 - y / (internal_height / 6);
    double time = (double) x / (double) width;
    int value = 127 - y *(double)127.0/ (internal_height);
    
    if (drag_mode == DRAG_MODE_MOVE_SELECTION) {

        Atom* dragged_note = dynamic_cast<Atom*> (drag_note_dragged);
        double temp_time = time + dragged_note->drag_offset_time;
        temp_time = temp_time - (int) temp_time; //wrap to 0.0 - 0.9999...
        double snapped_time = temp_time;
        if (snap) { //if snap & not shift key...
            snapped_time = Snap(temp_time);
        }
        //Remembers how much was the dragged note moved due to snapping feature, so that we can move other notes by the same distance.
        double snap_offset = snapped_time - temp_time;
        // *dbg << snap_offset << ENDL;

        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        std::set<Atom *, AtomComparingClass> resulting_selection;
        if (seq_type == SEQ_TYPE_NOTE) {
            for (; it != selection.end(); it++) {
                NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
                int temp_pitch = line + note->drag_offset_line;
                temp_time = time + note->drag_offset_time + snap_offset;
                temp_pitch = temp_pitch % 6; //wrap to 0-5;
                temp_time = temp_time - (int) temp_time; //wrap to 0.0 - 0.9999...
                if (temp_pitch < 0) temp_pitch += 6;
                if (temp_time < 0) temp_time += 1.0;
                note->pitch = temp_pitch;
                note->time = temp_time;
                resulting_selection.insert(note);
                // *dbg << " " << note->pitch << " " << note->time <<ENDL;
            }
        } else if (seq_type == SEQ_TYPE_CONTROL) {
            for (; it != selection.end(); it++) {
                ControllerAtom* ctrl = dynamic_cast<ControllerAtom*> (*it);
                int temp_value = value + ctrl->drag_offset_value;
                temp_time = time + ctrl->drag_offset_time + snap_offset;
                if (temp_value > 127) temp_value = 127;
                else if (temp_value < 0) temp_value = 0;
                if (temp_time != 1.0) //leave a possibility to put control atoms at 1.0, may be useful if one wants to add an immidiate slope at bar's start
                    temp_time = temp_time - (int) temp_time; //wrap to 0.0 - 0.9999...
                if (temp_time < 0) temp_time += 1.0;
                ctrl->value = temp_value;
                ctrl->time = temp_time;
                resulting_selection.insert(ctrl);
                // *dbg << " " << note->pitch << " " << note->time <<ENDL;
            }
        }
        //important!
        selection = resulting_selection;
        container->Sort();
        Files::FileModified(); //Mark file as modified
        //additionally:
        if (seq_type == SEQ_TYPE_CONTROL) on_selection_changed.emit(selection.size()); //this will update the value spinbutton
        
    } else if (drag_mode == DRAG_MODE_SELECT_AREA) {
        drag_current_x = x;
        drag_current_y = y;
        drag_current_line = line;
        drag_current_time = time;
        drag_current_value = value;

        //Determining drag selection.
        drag_temporary_selection.clear();
        int sel_value_min = min(drag_current_value, drag_beggining_value);
        int sel_value_max = max(drag_current_value, drag_beggining_value);
        int sel_pith_min = min(drag_current_line, drag_beggining_line);
        int sel_pith_max = max(drag_current_line, drag_beggining_line);
        double sel_time_min = min(drag_current_time, drag_beggining_time);
        double sel_time_max = max(drag_current_time, drag_beggining_time);
        int size = container->GetSize();
        if (seq_type == SEQ_TYPE_NOTE) {
            for (int x = 0; x < size; x++) {
                Atom* atm = (*container)[x];
                NoteAtom* note = dynamic_cast<NoteAtom*> (atm);
                //check if pitch is in bounds...
                if (note->pitch <= sel_pith_max && note->pitch >= sel_pith_min) {
                    // *dbg << " note " << x << " pith ("<<note->pitch <<")  in bounds.\n";
                    //check time...
                    double start = note->time;
                    double end = note->time + note->length;
                    if ((start <= sel_time_max && start >= sel_time_min) || (end <= sel_time_max && end >= sel_time_min) || (start <= sel_time_min && end >= sel_time_max)) {
                        //is inside!
                        drag_temporary_selection.insert(note);
                    }
                }
            }//check next note.
        } else if (seq_type == SEQ_TYPE_CONTROL) {
            for (int x = 0; x < size; x++) {
                Atom* atm = (*container)[x];
                ControllerAtom* ctrl = dynamic_cast<ControllerAtom*> (atm);
                //check if pitch is in bounds...
                if (ctrl->value <= sel_value_max && ctrl->value >= sel_value_min && ctrl->time >= sel_time_min && ctrl->time <= sel_time_max) {
                    drag_temporary_selection.insert(ctrl);
                }
            }//check next note.
        }

        
    } else if (drag_mode == DRAG_MODE_RESIZE && seq_type == SEQ_TYPE_NOTE) { //resizing do not exist within control sequencers

        NoteAtom* dragged_note = dynamic_cast<NoteAtom*> (drag_note_dragged);
        if (snap) { //if snap
            time = Snap(time);
        }
        double added_length = time - dragged_note->time - dragged_note->drag_beggining_length;

        double note_min_len = 0.01;
        if (snap) { //if snap
            note_min_len = (double) 1.0 / container->owner->resolution;
        }
        double note_max_len = 1.0;

        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        for (; it != selection.end(); it++) {
            NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
            double temp_length = note->drag_beggining_length + added_length;
            if (temp_length < note_min_len) temp_length = note_min_len;
            else if (temp_length > note_max_len) temp_length = note_max_len;
            note->length = temp_length;
        }
        Files::FileModified(); //Mark file as modified
    } else if (drag_mode == DRAG_MODE_CHANGE_VELOCITY && seq_type == SEQ_TYPE_NOTE) { //velocity change do not exist within control sequencers
        double vel_added = (drag_beggining_y - y);
        
        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        for (; it != selection.end(); it++) {
            NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
            int temp_vel = note->drag_beggining_velocity + vel_added;
            if (temp_vel < 0) temp_vel = 0;
            else if (temp_vel > 127) temp_vel = 127;
            note->velocity = temp_vel;
        }
        Files::FileModified(); //Mark file as modified
    }
    
    RedrawAtoms();

}

bool PatternWidget::on_button_press_event(GdkEventButton* event){
    grab_focus();//required to receive key_presses
    
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
        //const int height = allocation.get_height();
    int line = 6 - event->y / (internal_height / 6);
    double time = (double) event->x / (double) width;
    int value = 127 - event->y *(double)127.0/ (internal_height);
    double timeS  = ((double) event->x -(double)crtl_Tsurrounding)/ (double) width;
    double timeE  = ((double) event->x +(double)crtl_Tsurrounding)/ (double) width;
    
    drag_beggining_x = event->x;
    drag_beggining_y = event->y;
    
    //Checking whether clicked place was note or an empty space.
    Atom* found = NULL;
    int size = container->GetSize();
    if (seq_type == SEQ_TYPE_NOTE) {
        //Looking for notes...
        for (int x = 0; x < size; x++) {
            NoteAtom* note = dynamic_cast<NoteAtom*> ((*container)[x]);
            if (note->pitch == line && note->time < time && time < note->time + note->length) {
                found = note;
                break;
            }
        }
    } else if (seq_type == SEQ_TYPE_CONTROL) {
        //Looking for control atoms...
        for (int x = 0; x < size; x++) {
            ControllerAtom* ctrl = dynamic_cast<ControllerAtom*> ((*container)[x]);
            if (ctrl->value + ctrl_Vsurrounding > value && ctrl->value - ctrl_Vsurrounding < value && ctrl->time < timeE && ctrl->time > timeS) {
                found = ctrl;
                break;
            }
        }
    }
    
    drag_note_dragged = found;
    select_only_this_atom_on_LMB_release = NULL;
    
    if(button_pressed == NONE){
        last_clicked_note = found;
        if(event->button == 1 || event->button == 2){
            if(!(event->state & (1 << 0) || event->button == 2)){
                button_pressed = LMB;
                if(found != NULL){
                    //LMB on note
                    //'clear selection and select only the clicked note'
                    std::set<Atom*,AtomComparingClass>::iterator it = selection.find(found);
                    if (it != selection.end()){
                        //this note was already selected: do not unselect it yet
                        select_only_this_atom_on_LMB_release = found;
                    }else{
                        //this note wasn't selected, select only it (immidiatelly)
                        selection.clear();
                        selection.insert(found);
                        on_selection_changed.emit(selection.size());
                        RedrawAtoms();
                    }
                }else{
                    //LMB on empty space
                    //'create a new note'
                    double temp_time = time;
                    if (seq_type == SEQ_TYPE_NOTE) {
                        if (snap) {
                            temp_time = SnapDown(temp_time);
                        }
                        double len = (double) 1.0 / container->owner->resolution;
                        NoteAtom* note = new NoteAtom(temp_time, len, line);
                        if (event->state & (1 << 2)) note->velocity = 0; // If ctrl key was hold, add a quiet note
                        container->Add(note);
                        drag_in_progress = 1;
                        drag_mode = DRAG_MODE_RESIZE;
                        drag_beggining_line = line;
                        drag_beggining_time = time;
                        drag_note_dragged = note;
                        selection.clear();
                        selection.insert(note);
                        on_selection_changed.emit(selection.size());
                    } else if (seq_type == SEQ_TYPE_CONTROL) {
                        if (snap) {
                            temp_time = Snap(temp_time); //fair snapping for control atoms!
                        }
                        ControllerAtom* ctrl = new ControllerAtom(temp_time, value);
                        ctrl->slope_type = add_slope_type;
                        container->Add(ctrl);
                        drag_in_progress = 1;
                        drag_mode = DRAG_MODE_MOVE_SELECTION;
                        drag_beggining_value = value;
                        drag_beggining_time = time;
                        drag_note_dragged = ctrl;
                        selection.clear();
                        selection.insert(ctrl);
                        on_selection_changed.emit(selection.size());
                    }
                    Files::FileModified(); //Mark file as modified
                    RedrawAtoms();
                }
            }else{
                if (event->button == 1) button_pressed = LMBs;
                else if (event->button == 2) button_pressed = MMB;
                if(found != NULL){
                    //LMB + S  /  MMB on note
                    //'select the clicked note, or unselect it, if it was alreasy selected (need to remember the note is it was just selected in order to not unselect it on button_release event)'
                    if(selection.find(found) != selection.end()){
                        //note was selected, will unselect on button_release
                        note_that_was_just_added_to_selection = NULL;
                    }else{
                        //wasn't selected, select it
                        selection.insert(found);
                        note_that_was_just_added_to_selection = found;
                        on_selection_changed.emit(selection.size());
                        RedrawAtoms();
                    }
                    cancel_unselecting = 0;
                }else{
                    //LMB + S  / MMB on empty space
                    //'-'
                }
            }

        } else if (event->button == 3) {
            button_pressed = RMB;
            if (found != NULL) {
                //RMB on note
                //'delete the clicked note'
                container->Remove(found);
                std::set<Atom*,AtomComparingClass>::iterator it = selection.find(found);
                if (it != selection.end()){
                    selection.erase(it);
                    on_selection_changed.emit(selection.size());
                }
                RedrawAtoms();
            } else {
                //RMB on empty space
                //'clear selection'
                selection.clear();
                on_selection_changed.emit(selection.size());
                RedrawAtoms();
            }
            
        } 
    }
    return false;
}

bool PatternWidget::on_button_release_event(GdkEventButton* event){
    
    if(button_pressed == NONE) return false;
    
    //Finish drag-select, by moving temporarily selected notes to the real selection.
    if(drag_in_progress == 1 && drag_mode == DRAG_MODE_SELECT_AREA){
        //Finished selection by dragging.
        std::set<Atom *, AtomComparingClass>::iterator it = drag_temporary_selection.begin();
        for(;it!=drag_temporary_selection.end();it++){
            selection.insert(*it);
        }
        drag_temporary_selection.clear();

        on_selection_changed.emit(selection.size());
    }
    
    //Determine which button click was ended
    if (event->button == 1 || event->button == 2){
        if ((button_pressed == LMB) && event->button == 1){
            //LMB ended
            button_pressed = NONE;
            if(select_only_this_atom_on_LMB_release != NULL){
                selection.clear();
                selection.insert(select_only_this_atom_on_LMB_release);
                on_selection_changed.emit(selection.size());
            }
            
        }else if((button_pressed == LMBs && event->button == 1) || (button_pressed == MMB && event->button == 2)){
            //LMBs/MMB ended
            button_pressed = NONE;
            if(last_clicked_note != NULL){
                std::set<Atom*,AtomComparingClass>::iterator it = selection.find(last_clicked_note);
                if (it != selection.end()&&!cancel_unselecting){
                    //was selected, unselecting
                    selection.erase(it);
                    if (note_that_was_just_added_to_selection != NULL) selection.insert(note_that_was_just_added_to_selection);
                    on_selection_changed.emit(selection.size());
                }
            }
            
        }
    }
    else if(button_pressed == RMB && event->button == 3){
        //RMB ended
        button_pressed = NONE;
        if(drag_in_progress == 1){
                DeleteSelected();
                selection.clear();
                on_selection_changed.emit(selection.size());
        }
    }
    
    //Reset drag flag.
   drag_in_progress = 0;

    RedrawAtoms();
    
    return false;
}

bool PatternWidget::on_leave_notify_event(GdkEventCrossing* event){
    //mouse_button_is_down = 0;
    //drag_in_progress = 0;
    return false;
}

bool PatternWidget::on_motion_notify_event(GdkEventMotion* event){
    if(!drag_in_progress){//there is no drag in progress, maybe we need to initiate one?
        if (button_pressed!=NONE) {
            //if moved some distance, mark drag as in progress. 
            const int distance = 3;
            if (event->x > drag_beggining_x + distance || event->y > drag_beggining_y + distance || event->x < drag_beggining_x - distance || event->y < drag_beggining_y - distance) {
                InitDrag();
                ProcessDrag(event->x, event->y);
            }
        }
    }else{ //drag in process
        ProcessDrag(event->x,event->y);
    }

    return false;
}

bool PatternWidget::on_key_press_event(GdkEventKey* event){
    switch(event->keyval){
        //Note I can't use GDK_KEY_xyz macros, for they are unsupported in older versions of GTK, and harmonySEQ would fail to compile f.e. on Ubuntu Lucid Lynx.
        case 0xff08: //backspace
        case 0xffff: //delete
        case 0xff9f: //keypad delete
            DeleteSelected();
            return true;
            break;
        case 0xff52: //up
            if(event->state & (1 << 0)){ //shift key down
                if (seq_type == SEQ_TYPE_NOTE) IncreaseSelectionVelocity(10);
                else if (seq_type == SEQ_TYPE_CONTROL) IncreaseSelectionValue(8);
            }else
                MoveSelectionUp();
            
            return true;
            break;
        case 0xff54: //down
            if(event->state & (1 << 0)){ //shift key down
                if (seq_type == SEQ_TYPE_NOTE) DecreaseSelectionVelocity(10);
                else if (seq_type == SEQ_TYPE_CONTROL) DecreaseSelectionValue(8);
            }else
                MoveSelectionDown();
            
            return true;
            break;
        case 0xff53: //right
            MoveSelectionRight();
            
            return true;
            break;
        case 0xff51: //left
            MoveSelectionLeft();
            
            return true;
            break;
        default:
            FindAndProcessEventsKeyPress(event);
            return false;
            break;
    }
    return false;
}

bool PatternWidget::on_scroll_event(GdkEventScroll* e){

    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    //const int height = allocation.get_height();
    int line = 6 - e->y / (internal_height / 6);
    double time = (double) e->x / (double) width;
    
    //Checking whether scrolled place was note or an empty space.
    Atom* found = NULL;
    int size = container->GetSize();
    if (seq_type == SEQ_TYPE_NOTE) {
        //Looking for notes...
        for (int x = 0; x < size; x++) {
            NoteAtom* note = dynamic_cast<NoteAtom*> ((*container)[x]);
            if (note->pitch == line && note->time < time && time < note->time + note->length) {
                found = note;
                break;
            }
        }
    } // don'/t look for ctrl atoms, if that's a ctrl seq we'll never adjust velocity.
    
    if (e->state & (1 << 2)) //if ctrl key is pressed...
    {
                if(e->direction == GDK_SCROLL_UP){
                    ZoomIn();
                }else if (e->direction == GDK_SCROLL_DOWN){
                    ZoomOut();
                }
    }else{ //ctrl key not pressed            
        if (found != NULL) {//scrolled a note
            NoteAtom* note = dynamic_cast<NoteAtom*> (found);
            if (e->direction == GDK_SCROLL_DOWN) {
                if (selection.empty() || selection.find(found) == selection.end()) {
                    note->velocity -= 8;
                    if (note->velocity < 0) note->velocity = 0;
                    on_selection_changed.emit(selection.size());
                } else {
                    DecreaseSelectionVelocity(8);
                }
                RedrawAtoms();
            } else if (e->direction == GDK_SCROLL_UP) {
                if (selection.empty()|| selection.find(found) == selection.end()) {
                    note->velocity += 8;
                    if (note->velocity > 127) note->velocity = 127;
                    on_selection_changed.emit(selection.size());
                } else {
                    IncreaseSelectionVelocity(8);
                }
                RedrawAtoms();
            }
        } else { //empty space scrolled, we'll scroll view
            if (e->direction == GDK_SCROLL_UP) {
                on_scroll_left.emit();
            } else if (e->direction == GDK_SCROLL_DOWN) {
                on_scroll_right.emit();
            }
        }
    }
    
    return false;
}

  bool PatternWidget::on_expose_event(GdkEventExpose* event){
    Gtk::Allocation allocation = get_allocation();
    const double width = allocation.get_width();
    const int height = allocation.get_height();
    
    //check if everything wasn't resized, this would mean we have to redraw things.... 
    // an important case is when the widget is show()n, for it changes it's size from
    // something like 1,1 to something like 500,120, and we cannot leave it blank
    if(last_drawn_height != height || last_drawn_width != width) RedrawEverything();  
    last_drawn_height = height; last_drawn_width = width;
    
   cairo_t * c_t = gdk_cairo_create(event->window);
   Cairo::Context ct(c_t);
   
   ct.set_source(cr_buffer_surface,0,0);
   ct.rectangle(0.0,0.0,width,internal_height);
   ct.fill();
   
   cairo_destroy(c_t);
  
  return true;
      
  }
  
  bool PatternWidget::TimeLockAtomsCompleted(){
      atoms_lock =0;
      if(man_i_wanted_to_redraw_atoms_but_it_was_locked_could_you_please_do_it_later_for_me)
          RedrawAtoms();
      man_i_wanted_to_redraw_atoms_but_it_was_locked_could_you_please_do_it_later_for_me = 0;
      return false; //do not repeat the timeout
  }
  
  bool PatternWidget::TimeLockGridCompleted(){
      grid_lock = 0;
      if(man_i_wanted_to_redraw_grid_but_it_was_locked_could_you_please_do_it_later_for_me)
          RedrawGrid();
      man_i_wanted_to_redraw_grid_but_it_was_locked_could_you_please_do_it_later_for_me = 0;
      return false; //do not repeat the timeout
  }
  
  void PatternWidget::RedrawGrid(){
      
      if(grid_lock){
          man_i_wanted_to_redraw_grid_but_it_was_locked_could_you_please_do_it_later_for_me = 1;
          return; //do not draw to often!
      }
      
    if (!container) return; //just in case it's NULL...
    
    
    Gtk::Allocation allocation = get_allocation();
    const double width = allocation.get_width();
    //const int height = allocation.get_height();
    
    int resolution = container->owner->resolution;
    
      //clearing...
      cr_grid_context->save();
      cr_grid_context->set_source_rgb(1.0,1.0,1.0);
      //cr_grid_context->set_operator(Cairo::OPERATOR_SOURCE);
      cr_grid_context->paint();
      cr_grid_context->restore();

    //The +0.5 that often appears below in coordinates it to prevent cairo from antyaliasing lines.


    //horizontal grid
    if (seq_type == SEQ_TYPE_NOTE) {
        cr_grid_context->set_line_width(1);
        cr_grid_context->set_source_rgb(0.0, 0.0, 0.0);
        for (int x = 0; x <= 6; x++) {
            //the ' - (x==6)' does the trick, so that the last line is drawn not outside the widget.
            cr_grid_context->move_to(0, x * internal_height / 6 + 0.5 - (x==6));
            cr_grid_context->line_to(width, x * internal_height / 6 + 0.5 - (x==6));
            cr_grid_context->stroke();
        }
    } else if (seq_type == SEQ_TYPE_CONTROL) {
        cr_grid_context->set_line_width(1);
        for (int x = 0; x <= 4; x++) {

            if (x == 2)
                cr_grid_context->set_line_width(2.0);
            else
                cr_grid_context->set_line_width(1.0);

            if (x == 2)
                cr_grid_context->set_source_rgb(7.0, 0.0, 0.0);
            else
                cr_grid_context->set_source_rgb(0.0, 0.0, 0.0);

            //the ' - (x==4)' does the trick, so that the last line is drawn not outside the widget.
            cr_grid_context->move_to(0, x * internal_height / 4 + 0.5 - (x==4));
            cr_grid_context->line_to(width, x * internal_height / 4 + 0.5 - (x==4));
            cr_grid_context->stroke();
        }
    }

    //vertical grid
    int hints = resolution_hints[resolution];
    for (int x = 0; x <= resolution; x++) {
        if (x % hints == 0) {
            cr_grid_context->set_line_width(1.5);
            cr_grid_context->set_source_rgb(0.0, 0.5, 0.0); //hint colour
        } else {
            cr_grid_context->set_line_width(1.0);
            cr_grid_context->set_source_rgb(0.5, 0.5, 0.4); //normal grid colour
        }
        if (x != resolution) {
            cr_grid_context->move_to((int) ((double) x * (double) width / resolution) + 0.5, 0);
            cr_grid_context->line_to((int) ((double) x * (double) width / resolution) + 0.5, internal_height);
        } else {
            cr_grid_context->move_to((int) ((double) x * (double) width / resolution) - 0.5, 0);
            cr_grid_context->line_to((int) ((double) x * (double) width / resolution) - 0.5, internal_height); //the last one must be in drawing area, so let's put it a 1 px before
        }
        cr_grid_context->stroke();
    }
    
    grid_lock = 1;
    man_i_wanted_to_redraw_grid_but_it_was_locked_could_you_please_do_it_later_for_me = 0;
    Glib::signal_timeout().connect(sigc::mem_fun(*this,&PatternWidget::TimeLockGridCompleted),Config::Interaction::PatternRefreshMS);
    
    Redraw();
  }
  
  void PatternWidget::RedrawAtoms(){

    if (atoms_lock) {
        man_i_wanted_to_redraw_atoms_but_it_was_locked_could_you_please_do_it_later_for_me = 1;
        return; //do not draw to often!
    }
      
    if (!container) return; //just in case it's NULL...
    
            //clearing...
      cr_atoms_context->save();
      cr_atoms_context->set_source_rgba(0,0,0,0);
      cr_atoms_context->set_operator(Cairo::OPERATOR_SOURCE);
      cr_atoms_context->paint();
      cr_atoms_context->restore();
  
    Gtk::Allocation allocation = get_allocation();
    const double width = allocation.get_width();
    //const int height = allocation.get_height();


    int size = container->GetSize();
  
  //notes
  if(seq_type == SEQ_TYPE_NOTE){
                        for (int x = size-1; x >= 0; x--){ //iterating backwards, to draw shades below notes
                              Atom* atm = (*container)[x];
                              NoteAtom* note = dynamic_cast<NoteAtom*>(atm);
                              if(note == NULL) {*err << _("While drawing pattern: note = null. This should never happen! Please report this bug to harmonySEQ developers.\n"); continue;}

                              //calculate coordinates
                              double y1 = (5-note->pitch)*internal_height/6;
                              double h = internal_height/6;
                              double x1 = note->time*width;
                              double w = note->length*width;
                              y1++; // This is because the very first 1px line is the upper border.
                              //Check if note is in selection.
                              bool selected = false;
                              //TODO: searching should be faster! Might be better to mark an atom's flag stating whether the note is in selection?
                              if(selection.find(note) != selection.end()) selected = true;
                              //check if in temp. selection
                              if(drag_temporary_selection.find(note) != drag_temporary_selection.end()) selected = true;

                              //draw note
                              cr_atoms_context->set_line_width(3.0);
                              cr_atoms_context->set_line_join(Cairo::LINE_JOIN_ROUND);
                              cr_atoms_context->rectangle(x1+1.5,y1+1.5,w-3,h-3);
                              double af = (double)note->velocity/127; //may be changed to 128, this will not affect the graphics visibly, but may work slightly faster.
                              if(selected) cr_atoms_context->set_source_rgba(0.8,0.0,0.0,af);
                              else cr_atoms_context->set_source_rgba(0.,0.0,0.8,af);
                              cr_atoms_context->fill_preserve();
                              if(selected) cr_atoms_context->set_source_rgb(0.4,0.0,0.0);
                              else cr_atoms_context->set_source_rgb(0.0,0.0,0.4);
                              cr_atoms_context->stroke();
                                  
                              //cr_atoms_context->save();
                              {
                                  cr_atoms_context->rectangle(x1+1.5,y1+1.5,w-3,h-3);
                                  cr_atoms_context->clip();
                                  
                                  //draw velocity bar
                                 double velbar_up = y1+(127.0-(double)note->velocity)*h/127.0;
                                 double velbar_h = h*(double)note->velocity/127.0;
                                 cr_atoms_context->rectangle(x1+3,velbar_up,7,velbar_h);
                                 if(selected) cr_atoms_context->set_source_rgb(0.8,0.0,0.0);
                                 else cr_atoms_context->set_source_rgb(0.0,0.0,0.8);
                                 cr_atoms_context->fill();
                                 
                                  //length handle
                                  cr_atoms_context->move_to(x1+w-1.5,y1+h-1.5);
                                  cr_atoms_context->line_to(x1+w-1.5,y1+h-1.5-handle_size);
                                  cr_atoms_context->line_to(x1+w-1.5-handle_size,y1+h-1.5);
                                  cr_atoms_context->line_to(x1+w-1.5,y1+h-1.5);
                                  if(selected) cr_atoms_context->set_source_rgb(0.4,0.0,0.0);
                                  else cr_atoms_context->set_source_rgb(0.0,0.0,0.4);
                                  cr_atoms_context->fill();
                                  
                                  cr_atoms_context->reset_clip();
                              }  
                              //cr_atoms_context->restore();

                             if (note->time + note->length > 1.0) {
                                 //draw shade
                                 x1 -= width;
                                 cr_atoms_context->rectangle(x1 + 1.5, y1 + 1.5, w - 3, h - 3);
                                 cr_atoms_context->set_source_rgba(0.8, 0.8, 0.8, af*0.75); 
                                 cr_atoms_context->fill_preserve();
                                 cr_atoms_context->set_source_rgb(0.7, 0.7, 0.7);
                                 cr_atoms_context->stroke(); 
                             }
                        }
      }else if (seq_type == SEQ_TYPE_CONTROL){
                        if(size!=0) //if there are no atoms, draw nothing
                        for (int n = -1; n < size; n++){ //iterating forwards, to draw lines below atoms
                               Atom* atm;
                               Atom* nextatm;
                               if(size == 1) nextatm = atm = (*container)[0];
                               else{
                                   if(n==-1)
                                            atm = (*container)[size-1];
                                   else
                                            atm = (*container)[n];
                                   if(n== size-1)
                                            nextatm = (*container)[0];
                                   else
                                            nextatm = (*container)[n+1];
                               }
                              ControllerAtom* ctrl = dynamic_cast<ControllerAtom*>(atm);
                              ControllerAtom* nextctrl = dynamic_cast<ControllerAtom*>(nextatm);
                              
                              if(ctrl == NULL) {*err << _("While drawing pattern: ctrl = null. This should never happen! Please report this bug to harmonySEQ developers.\n"); continue;}
                              
                              //these are ints intentionally - to avoid unnecesary cairo anti-aliasing we round everything to 1.0 and add 0.5
                              int y = (double)internal_height*((127.0-ctrl->value)/(127.0));
                              int x = ctrl->time*width ;
                              //Check if note is in selection.
                              bool selected = false;
                              if(selection.find(ctrl) != selection.end()) selected = true;
                              //check if in temp. selection
                              if(drag_temporary_selection.find(ctrl) != drag_temporary_selection.end()) selected = true;
                              
                              //draw line to next point
                              cr_atoms_context->set_line_width(3.0);
                              if(n == -1 || n == size-1)
                                  cr_atoms_context->set_source_rgb(0.65,0.65,0.65);
                              else
                                cr_atoms_context->set_source_rgb(0.2,0.2,0.2);
                              cr_atoms_context->move_to(x-(n==-1)*width+0.5,y+0.5);
                              if(ctrl->slope_type == SLOPE_TYPE_FLAT)
                                  cr_atoms_context->line_to(nextctrl->time*width+(n==size-1)*width+0.5, y+0.5);
                              else if(ctrl->slope_type == SLOPE_TYPE_LINEAR){
                                  int next_y =  (double)internal_height*((127.0-nextctrl->value)/(127.0));
                                  cr_atoms_context->line_to(nextctrl->time*width+(n==size-1)*width+0.5, next_y+0.5);
                              }
                              cr_atoms_context->stroke();
                              
                              if(n == -1) continue; //no need to draw -1th atom
                              
                              //draw atom
                              cr_atoms_context->set_line_width(1.0);
                              if(ctrl->slope_type == SLOPE_TYPE_FLAT)
                                  cr_atoms_context->rectangle(x-4.5,y-4.5,11.0,11.0);
                              else if(ctrl->slope_type == SLOPE_TYPE_LINEAR)
                                  cr_atoms_context->arc(x+0.5,y+0.5,7.0,0,2*M_PI);
                              if(selected) cr_atoms_context->set_source_rgb(0.8,0.0,0.0);
                              else cr_atoms_context->set_source_rgb(0.,0.0,0.8);
                              cr_atoms_context->fill_preserve();
                              if(selected) cr_atoms_context->set_source_rgb(0.4,0.0,0.0);
                              else cr_atoms_context->set_source_rgb(0.0,0.0,0.4);
                              cr_atoms_context->stroke();
                        }
      }
  
    
    atoms_lock = 1;
    man_i_wanted_to_redraw_atoms_but_it_was_locked_could_you_please_do_it_later_for_me = 0;
    Glib::signal_timeout().connect(sigc::mem_fun(*this,&PatternWidget::TimeLockAtomsCompleted),Config::Interaction::PatternRefreshMS);

    Redraw();
      
  }
  
  bool PatternWidget::RedrawEverything(){
      RedrawAtoms();
      RedrawGrid();
      return false;
  }
  
  void PatternWidget::Redraw(){
      //Glib::Timer T;
    //long unsigned int t;
      //T.reset(); T.start();
    
    Gtk::Allocation allocation = get_allocation();
    const double width = allocation.get_width();
    
    //T.elapsed(t);
    //*err << " 1- " <<  (int)t << ENDL;
    cr_buffer_context->set_source(cr_grid_surface,0,0);
    cr_buffer_context->rectangle(0.0,0.0,width,internal_height);
    cr_buffer_context->fill();
    
    //T.elapsed(t);
    //*err << " 2 -" <<  (int)t << ENDL;
    cr_buffer_context->set_source(cr_atoms_surface,0,0);
    cr_buffer_context->rectangle(0.0,0.0,width,internal_height);
    cr_buffer_context->fill();

    //T.elapsed(t);
    //*err << " 3 -" <<  (int)t << ENDL;
      //drag_selection rectangle
  if(drag_in_progress && drag_mode==DRAG_MODE_SELECT_AREA){
      cr_buffer_context->set_line_width(2);
      cr_buffer_context->rectangle(drag_beggining_x,drag_beggining_y,drag_current_x-drag_beggining_x,drag_current_y-drag_beggining_y);
      cr_buffer_context->set_source_rgba(0.9,0.4,0.3,0.2);
      cr_buffer_context->fill_preserve();
      cr_buffer_context->set_source_rgb(0.9,0.4,0.3);
      cr_buffer_context->stroke();
      
  }
    //T.elapsed(t);
    //*err << (int)t << ENDL;
    
   // T.stop();
    queue_draw();
}