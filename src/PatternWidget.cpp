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
#include "PatternWidget.h"
#include "cairomm/context.h"
#include "global.h"
#include "messages.h"
#include "AtomContainer.h"
#include "NoteAtom.h"
#include "Sequencer.h"
#include "Event.h"


PatternWidget::PatternWidget(){
    internal_height=50; //random guess. will be reset soon anyway by the SequencerWidget, but better protect from 0-like values.
    vert_size = 450.0; //adjust for better default size
    snap = true;
    add_mode = 0;
    add_events(Gdk::BUTTON_PRESS_MASK);
    add_events(Gdk::BUTTON_RELEASE_MASK);
    add_events(Gdk::BUTTON1_MOTION_MASK);
    add_events(Gdk::BUTTON3_MOTION_MASK);
    add_events(Gdk::KEY_PRESS_MASK);
    set_can_focus(1);//required to receive key_presses
}

PatternWidget::~PatternWidget(){
}

void PatternWidget::SetInternalHeight(int h){
    internal_height = h;
    UpdateSizeRequest();
}

void PatternWidget::Redraw(){
    queue_draw();
}

void PatternWidget::UpdateSizeRequest(){
    //*dbg << "sizerequest " << vert_size << " " << internal_height+20 << ENDL;
    set_size_request(vert_size,internal_height+20);
}

void PatternWidget::ZoomIn(){
    vert_size = vert_size/1.2;
    UpdateSizeRequest();
}

void PatternWidget::ZoomOut(){
    vert_size = vert_size*1.2;
    UpdateSizeRequest();
}

void PatternWidget::EnterAddMode(){
    selection.clear();
    on_selection_changed.emit(0);
    add_mode = 1;
}
void PatternWidget::LeaveAddMode(){
    add_mode = 0;
}

bool PatternWidget::GetAddMode(){
    return add_mode;
}

void PatternWidget::AssignPattern(AtomContainer* cont){
    *dbg << "assigning pattern \n";
    container = cont;
    selection.clear();
    on_selection_changed.emit(0);
    Redraw();
}

void PatternWidget::ClearSelection(){
    selection.clear();
    on_selection_changed.emit(0);
    Redraw();
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
    on_selection_changed.emit(0);
    Redraw();
}

void PatternWidget::SetSnap(bool s){
    snap = s;
}

bool PatternWidget::GetSnap(){
    return snap;
}

void PatternWidget::SetVelocity(int v){
    std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
    for (; it != selection.end(); it++) {
        NoteAtom* note = dynamic_cast<NoteAtom*>(*it);
        note->velocity = v;
    }    
    Redraw();
}

int PatternWidget::Velocity(){
    int sum = 0;
    std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
    for (; it != selection.end(); it++) {
        NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
        sum += note->velocity;
    }    
    if(selection.size()==0) return 0;
    return sum/(selection.size());
}

void PatternWidget::MoveSelectionUp(){
        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        std::set<Atom *, AtomComparingClass> resulting_selection;
        for (; it != selection.end(); it++) {
            NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
            note->pitch = (note->pitch+1)%6;
            resulting_selection.insert(note);
        }
        selection = resulting_selection;
        container->Sort();
        Redraw();
}

void PatternWidget::MoveSelectionDown(){
        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        std::set<Atom *, AtomComparingClass> resulting_selection;
        for (; it != selection.end(); it++) {
            NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
            note->pitch = (note->pitch-1)%6;
            resulting_selection.insert(note);
        }
        selection = resulting_selection;
        container->Sort();
        Redraw();
}


void PatternWidget::MoveSelectionRight(){
        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        std::set<Atom *, AtomComparingClass> resulting_selection;
        for (; it != selection.end(); it++) {
            NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
            note->time = (note->time+1.0/(double)container->owner->resolution);
            if(note->time >= 1.0) note->time -= 1.0;
            if(note->time < 0.0) note->time += 1.0;
            resulting_selection.insert(note);
        }
        selection = resulting_selection;
        container->Sort();
        Redraw();
}

void PatternWidget::MoveSelectionLeft(){
        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        std::set<Atom *, AtomComparingClass> resulting_selection;
        for (; it != selection.end(); it++) {
            NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
            note->time = (note->time-1.0/(double)container->owner->resolution);
            if(note->time >= 1.0) note->time -= 1.0;
            if(note->time < 0.0) note->time += 1.0;
            resulting_selection.insert(note);
        }
        selection = resulting_selection;
        container->Sort();
        Redraw();
}

void PatternWidget::IncreaseSelectionVelocity(){
        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        std::set<Atom *, AtomComparingClass> resulting_selection;
        for (; it != selection.end(); it++) {
            NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
            note->velocity = (note->velocity+10);
            if(note->velocity > 127) note->velocity = 127;
            if(note->velocity < 0) note->velocity = 0;
            resulting_selection.insert(note);
        }
        selection = resulting_selection;
        container->Sort();
        on_selection_changed.emit(selection.size());//this will update the velocity spinbutton
        Redraw();
}

void PatternWidget::DecraseSelecionVelocity(){
        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        std::set<Atom *, AtomComparingClass> resulting_selection;
        for (; it != selection.end(); it++) {
            NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
            note->velocity = (note->velocity-10);
            if(note->velocity > 127) note->velocity = 127;
            if(note->velocity < 0) note->velocity = 0;
            resulting_selection.insert(note);
        }
        selection = resulting_selection;
        container->Sort();
        on_selection_changed.emit(selection.size());//this will update the velocity spinbutton
        Redraw();
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
    //count position
    int line = 5 - drag_beggining_y / (internal_height / 6); //I HAVE NO IDEA WHY THERE SHOULD BE 5 AND NOT 6, DO NOT ASK THAT SEEMS TO BE ****** WEIRD
    double time = (double) drag_beggining_x / (double) width;
    //looking if there is a note where drag was began...
    Atom* note_found = NULL;
    Atom* note_ending_found = NULL;
    const int ending = 10;
    const double ending_size = (double)ending/width;
    int size = container->GetSize();
    for (int x = 0; x < size; x++) {
        NoteAtom* note = dynamic_cast<NoteAtom*> ((*container)[x]);
        if (note->pitch == line && note->time + note->length - ending_size < time && time < note->time + note->length + ending_size) {
            note_ending_found = note;
            break;
        }
        if (note->pitch == line && note->time < time && time < note->time + note->length) {
            note_found = note;
            break;
        }
    }
    //and checking if it's in a selection
    std::set<Atom *, AtomComparingClass>::iterator it;
    if (note_found != NULL) it = selection.find(note_found);
    else it = selection.end(); //pretend the search failed
    
    if(note_ending_found != NULL){
        //user tries to resize the note
        drag_mode = DRAG_MODE_RESIZE;
        drag_in_progress = 1;
        drag_beggining_line = line;
        drag_beggining_time = time;
        drag_note_dragged = note_ending_found;
        //NoteAtom* dragged_note = dynamic_cast<NoteAtom*> ((*container)[note_ending_found]);
        //Store note offsets...
        std::set<Atom*>::iterator it = selection.begin();
        for (; it != selection.end(); it++) {
            NoteAtom* note = dynamic_cast<NoteAtom*>(*it);
            note->drag_beggining_length = note->length;
        }
    }else if (it != selection.end()) {// so it is in selection...
        //beggining drag. 
        drag_mode = DRAG_MODE_MOVE_SELECTION;
        drag_in_progress = 1;
        drag_beggining_line = line;
        drag_beggining_time = time;
        drag_note_dragged = note_found;
        NoteAtom* dragged_note = dynamic_cast<NoteAtom*>(note_found);
        drag_time_offset_to_dragged_note = drag_beggining_time - dragged_note->time;
        //Store note offsets...
        std::set<Atom*>::iterator it2 = selection.begin();
        for (; it2 != selection.end(); it2++) {
            NoteAtom* note = dynamic_cast<NoteAtom*> (*it2);
            //*err << "   " << line << ENDL;
            note->drag_offset_line = note->pitch - line;
            note->drag_offset_time = note->time - time;
            //*err << note->drag_offset_line << " " << note->drag_offset_time << ENDL;
        }
    } else {
        //drag begun in place where was no selection
        drag_mode = DRAG_MODE_SELECT_AREA;
        drag_in_progress = 1;
        drag_beggining_line = line;
        drag_beggining_time = time;
        drag_temporary_selection.clear();
    }
}

void PatternWidget::ProcessDrag(double x, double y,bool shift_key){
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    //const int height = allocation.get_height();
    //count position
    int line = 6 - y / (internal_height / 6);
    double time = (double) x / (double) width;
    
    if (drag_mode == DRAG_MODE_MOVE_SELECTION) {
        
        NoteAtom* dragged_note = dynamic_cast<NoteAtom*>(drag_note_dragged);
        double temp_time = time + dragged_note->drag_offset_time;
        temp_time = temp_time - (int) temp_time; //wrap to 0.0 - 0.9999...
        double snapped_time = temp_time;
        if (snap && !(shift_key)) { //if snap & not shift key...
            snapped_time = Snap(temp_time);
        }
        //Remembers how much was the dragged note moved due to snapping feature, so that we can move other notes by the same distance.
        double snap_offset = snapped_time - temp_time;
        //*dbg << snap_offset << ENDL;

        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        std::set<Atom *, AtomComparingClass> resulting_selection;
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
            //*dbg << " " << note->pitch << " " << note->time <<ENDL;
        }
        selection = resulting_selection;
    } else if (drag_mode == DRAG_MODE_SELECT_AREA) {
        drag_current_x = x;
        drag_current_y = y;
        drag_current_line = line;
        drag_current_time = time;

        //Determining drag selection.
        drag_temporary_selection.clear();
        int sel_pith_min = min(drag_current_line, drag_beggining_line);
        int sel_pith_max = max(drag_current_line, drag_beggining_line);
        double sel_time_min = min(drag_current_time, drag_beggining_time);
        double sel_time_max = max(drag_current_time, drag_beggining_time);
        int size = container->GetSize();
        for (int x = 0; x < size; x++) {
            NoteAtom* note = dynamic_cast<NoteAtom*> ((*container)[x]);
            //check if pitch is in bounds...
            if (note->pitch <= sel_pith_max && note->pitch >= sel_pith_min) {
                //*dbg << " note " << x << " pith ("<<note->pitch <<")  in bounds.\n";
                //check time...
                double start = note->time;
                double end = note->time + note->length;
                if ((start <= sel_time_max && start >= sel_time_min) || (end <= sel_time_max && end >= sel_time_min) || (start <= sel_time_min && end >= sel_time_max)) {
                    //is inside!
                    drag_temporary_selection.insert(note);
                }
            }
        }//check next note.

    } else if (drag_mode == DRAG_MODE_RESIZE) {
        
        NoteAtom* dragged_note = dynamic_cast<NoteAtom*>(drag_note_dragged);
        if (snap && !(shift_key)) { //if snap & not shift key...
            time = Snap(time);
        }
        double added_length = time-dragged_note->time-dragged_note->drag_beggining_length;

        double note_min_len = 0.01;
        if (snap && !(shift_key)) { //if snap & not shift key...
            note_min_len = (double)1.0/container->owner->resolution;
        }
        double note_max_len = 1.0;
        
        std::set<Atom *, AtomComparingClass>::iterator it = selection.begin();
        for (; it != selection.end(); it++) {
            NoteAtom* note = dynamic_cast<NoteAtom*> (*it);
            double temp_length = note->drag_beggining_length+added_length;
            if(temp_length < note_min_len) temp_length = note_min_len;
            else if(temp_length > note_max_len) temp_length = note_max_len;
            note->length = temp_length;
        }
    }
    
    Redraw();
}

bool PatternWidget::on_button_press_event(GdkEventButton* event){
    
    grab_focus();//required to receive key_presses
    
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
        //const int height = allocation.get_height();
    int line = 6 - event->y / (internal_height / 6);
    double time = (double) event->x / (double) width;
    
    if(event->button == 1) //LMB
    {
        drag_beggining_x = event->x;
        drag_beggining_y = event->y;
        if(event->y <= internal_height){
            if(!add_mode){
                        Atom* found = NULL;
                        int size = container->GetSize();
                        for(int x = 0; x <size;x++){
                            NoteAtom* note = dynamic_cast<NoteAtom*>((*container)[x]);
                            if(note->pitch == line &&note->time < time && time < note->time+note->length){
                                found = note;
                                break;
                            }
                        }
                        if(found == NULL){
                            //clicked empty space, clear selection.
                            if (event->state & (1 << 0) || event->state & (1 << 2)) {//shift or ctrl key was pressed...
                                //Do nothing, do not clear selection.
                            } else {
                                //Empty space with no shift... clear selection.
                                selection.clear();
                                on_selection_changed.emit(selection.size());
                            }
                        }else{
                            //clicked a note.
                            if (event->state & (1 << 2)) {//ctrl key was pressed...
                                //we'll add the note to selection, unless it's already selected, then we de-select it.
                                std::set<Atom*>::iterator it= selection.find(found);
                                 if(it != selection.end()) {
                                     //it's already selected, then:
                                     //REMOVING NOTE FROM SELECTION
                                     selection.erase(it);
                                     
                                     on_selection_changed.emit(selection.size());
                                 }else{
                                     //it was not selected, select it.
                                     //ADDING NOTE TO SELECTION
                                     selection.insert(found);

                                     on_selection_changed.emit(selection.size());
                                 }
                            } else {//shift key was not pressed
                                std::set<Atom *, AtomComparingClass>::iterator it= selection.find(found);
                                 if(it != selection.end()) 
                                     //it's already selected, then:
                                     ;
                                 else{
                                     //it was not selected
                                     //SELECTING A NEW NOTE
                                     selection.clear();
                                     selection.insert(found);
                                     
                                     on_selection_changed.emit(selection.size());
                                 }
                            }
                        }
            }else{ //ADD MODE ON
                double temp_time=time;
                if(snap && !(event->state & (1 << 0))){
                    temp_time = SnapDown(temp_time);
                }
                double len = (double)1.0/container->owner->resolution;    
                NoteAtom* note = new NoteAtom(temp_time,len,line);
                container->Add(note);
                drag_in_progress=1;
                drag_mode=DRAG_MODE_RESIZE;
                drag_beggining_line = line;
                drag_beggining_time = time;
                drag_note_dragged = note;
                selection.clear();
                selection.insert(note);
                on_selection_changed.emit(selection.size());
            }
            Redraw();
        } //(event->y <= internal_height)
        
        
    }else if(event->button == 3){ //RMB
        if(add_mode){
            add_mode = 0;
            drag_in_progress = 0;
            on_add_mode_changed.emit();
        }//else{
            on_add_mode_changed.emit();
            double temp_time = time;
            if (snap && !(event->state & (1 << 0))) {
                temp_time = SnapDown(temp_time);
            }
            double len = (double) 1.0 / container->owner->resolution;
            NoteAtom* note = new NoteAtom(temp_time, len, line);
            container->Add(note);
            drag_in_progress = 1;
            drag_mode = DRAG_MODE_RESIZE;
            drag_beggining_line = line;
            drag_beggining_time = time;
            drag_note_dragged = note;
            selection.clear();
            selection.insert(note);
            on_selection_changed.emit(selection.size());
            Redraw();
        //}
    }
    return false;
}

bool PatternWidget::on_button_release_event(GdkEventButton* event){
    if(event->button == 1){
        if(!drag_in_progress){
            
        }else{
            drag_in_progress = 0;
            if(drag_mode == DRAG_MODE_SELECT_AREA){
                //Finished selection by dragging.
                std::set<Atom *, AtomComparingClass>::iterator it = drag_temporary_selection.begin();
                for(;it!=drag_temporary_selection.end();it++){
                    selection.insert(*it);
                }
                drag_temporary_selection.clear();
                
                on_selection_changed.emit(selection.size());
                Redraw();
            }else if (drag_mode == DRAG_MODE_MOVE_SELECTION) {
                container->Sort();
            }
        }
    }else if(event->button == 3){
        if(drag_in_progress) drag_in_progress = 0;
    }
    return false;
}

bool PatternWidget::on_leave_notify_event(GdkEventCrossing* event){
    //mouse_button_is_down = 0;
    //drag_in_progress = 0;
    return false;
}

bool PatternWidget::on_motion_notify_event(GdkEventMotion* event){
        if(!drag_in_progress){//there is no drag in progress, maybe we need to initiate one?
            if (event->state & (1 << 8)) { //LMB down 
                //if moved some distance, mark drag as in progress. 
                //if we use SHIFT to precise moving, do not apply this distance, and mark as drag regardless of it.
                const int distance = 3;
                if (event->x > drag_beggining_x + distance || event->y > drag_beggining_y + distance || event->x < drag_beggining_x - distance || event->y < drag_beggining_y - distance) {
                    InitDrag();
                    ProcessDrag(event->x, event->y, (event->state & (1 << 0)));
                }
            }
        }else{ //drag in process
            ProcessDrag(event->x,event->y,(event->state & (1 << 0)));
        }
        
    return false;
}

bool PatternWidget::on_key_press_event(GdkEventKey* event){
    switch(event->keyval){
        case GDK_KEY_BackSpace:
        case GDK_KEY_Delete:
        case GDK_KEY_KP_Delete:
            DeleteSelected();
           if(add_mode){ add_mode = 0; on_add_mode_changed.emit();}
            return true;
            break;
        case GDK_KEY_Up:
            if(event->state & (1 << 0)) //shift key down
                IncreaseSelectionVelocity();
            else
                MoveSelectionUp();
            
           if(add_mode){ add_mode = 0; on_add_mode_changed.emit();}
            return true;
            break;
        case GDK_KEY_Down:
            if(event->state & (1 << 0)) //shift key down
                DecraseSelecionVelocity();
            else
                MoveSelectionDown();
            
           if(add_mode){ add_mode = 0; on_add_mode_changed.emit();}
            return true;
            break;
        case GDK_KEY_Right:
            MoveSelectionRight();
            
           if(add_mode){ add_mode = 0; on_add_mode_changed.emit();}
            return true;
            break;
        case GDK_KEY_Left:
            MoveSelectionLeft();
            
           if(add_mode){ add_mode = 0; on_add_mode_changed.emit();}
            return true;
            break;
        default:
            FindAndProcessEventsKeyPress(event);
            return false;
            break;
    }
    return false;
}

  bool PatternWidget::on_expose_event(GdkEventExpose* event){
   cairo_t * c_t = gdk_cairo_create(event->window);
   Cairo::Context ct(c_t);
   //Cairo::RefPtr<Cairo::Context> cr(&ct);
    
   Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  //const int height = allocation.get_height();
  
  int resolution = container->owner->resolution;
  
  //The +0.5 that often appears below in coordinates it to prevent cairo from antyaliasing lines.
    
  //notes
  ct.set_line_width(3.0);
  ct.set_line_join(Cairo::LINE_JOIN_ROUND);
  if (container) //just in case it's NULL...
  for (int x = container->GetSize()-1; x >= 0; x--){ //iterating backwards, to draw shades below notes
      Atom* atm = (*container)[x];
      NoteAtom* note = dynamic_cast<NoteAtom*>(atm);
      if(note == NULL) {*err << "While drawing pattern: note = null. This should never happen! Please report this bug to harmonySEQ developers.\n"; continue;}
      double y1 = (5-note->pitch)*internal_height/6;
      double h = internal_height/6;
      double x1 = note->time*width;
      double w = note->length*width;
      y1++; // This is because the very first 1px line is the upper border.
    //*dbg << "drawing note... "<< x1 << " " << y1 << " " << w << " " <<  h << "\n";
      //Check if note is in selection.
      bool selected = false;
      if(selection.find(note) != selection.end()) selected = true;
      //check if in temp. selection
      if(drag_temporary_selection.find(note) != drag_temporary_selection.end()) selected = true;
      
      ct.rectangle(x1+1.5,y1+1.5,w-3,h-3);
      double af = (double)note->velocity/127; //may be changed to 128, this will not affect the graphics visibly, but may work slightly faster.
      if(selected) ct.set_source_rgba(0.8,0.0,0.0,af);
      else ct.set_source_rgba(0.,0.0,0.8,af);
      ct.fill_preserve();
      if(selected) ct.set_source_rgb(0.4,0.0,0.0);
      else ct.set_source_rgb(0.0,0.0,0.4);
      ct.stroke();
      
      if(note->time + note->length > 1.0){
          //draw shade
          x1 -= width;
         ct.rectangle(x1+1.5,y1+1.5,w-3,h-3);
         ct.set_source_rgba(0.8,0.8,0.8,af/0.75);
         ct.fill_preserve();
         ct.set_source_rgb(0.7,0.7,0.7);
         ct.stroke();
      }
      
     double velbar_up = y1+(127.0-(double)note->velocity)*h/127.0;
     double velbar_h = h*(double)note->velocity/127.0;
     ct.rectangle(x1+3,velbar_up,4,velbar_h);
     if(selected) ct.set_source_rgb(0.8,0.0,0.0);
     else ct.set_source_rgb(0.0,0.0,0.8);
     ct.fill();
  }
  
  //horizontal grid
  ct.set_line_width(1);
  ct.set_source_rgb(0.0,0.0,0.0);
  for(int x = 0; x <= 6; x++){
        ct.move_to(0,x*internal_height/6+0.5);
        ct.line_to(width,x*internal_height/6+0.5);
        ct.stroke();
  }
  
  //vertical grid
  ct.set_line_width(1);
  ct.set_source_rgb(0.3,0.3,0.2);
  for(int x = 0; x <= resolution; x++){
        if (x!=resolution){
                ct.move_to((int)((double)x*(double)width/resolution) + 0.5,0);
                ct.line_to((int)((double)x*(double)width/resolution) + 0.5,internal_height);
        }else{
                ct.move_to((int)((double)x*(double)width/resolution) - 0.5,0);
                ct.line_to((int)((double)x*(double)width/resolution) - 0.5,internal_height); //the last one must be in drawing area, so let's put it a 1 px before
        }  
        ct.stroke();
  }
  
  //drag_selection rectangle
  if(drag_in_progress && drag_mode==DRAG_MODE_SELECT_AREA){
      ct.set_line_width(2);
      ct.rectangle(drag_beggining_x,drag_beggining_y,drag_current_x-drag_beggining_x,drag_current_y-drag_beggining_y);
      ct.set_source_rgba(0.9,0.4,0.3,0.2);
      ct.fill_preserve();
      ct.set_source_rgb(0.9,0.4,0.3);
      ct.stroke();
      
  }
  
  return true;
      
  }
