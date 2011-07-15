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

PatternWidget::PatternWidget(){
    internal_height=50; //random guess. will be reset soon anyway by the SequencerWidget, but better protect from 0-like values.
    velocity = 0; //as above
    vert_size = 450.0; //adjust for better default size
    add_events(Gdk::BUTTON_PRESS_MASK);
    add_events(Gdk::BUTTON_RELEASE_MASK);
    add_events(Gdk::BUTTON1_MOTION_MASK);
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

void PatternWidget::SetVelocity(int v){
    std::set<int>::iterator it = selection.begin();
    for (; it != selection.end(); it++) {
        NoteAtom* note = dynamic_cast<NoteAtom*> ((*container)[*it]);
        note->velocity = v;
    }    
    Redraw();
}

int PatternWidget::CalculateAverageVelocity(){
    int sum = 0;
    std::set<int>::iterator it = selection.begin();
    for (; it != selection.end(); it++) {
        NoteAtom* note = dynamic_cast<NoteAtom*> ((*container)[*it]);
        sum += note->velocity;
    }    
    if(selection.size()==0) return 0;
    return sum/(selection.size());
}

bool PatternWidget::on_button_press_event(GdkEventButton* event){
    if(event->button == 1) //LMB
    {
        mouse_button_is_down = 1;
        drag_beggining_x = event->x;
        drag_beggining_y = event->y;
        
        Gtk::Allocation allocation = get_allocation();
        const int width = allocation.get_width();
        const int height = allocation.get_height();
        //determine line:
        if(event->y <= internal_height){
            int line = 6-event->y/(internal_height/6);
            //*err << line << ENDL;
            double time = (double)event->x/(double)width;
            //*err << time <<ENDL;
            int found = -1;
            int size = container->GetSize();
            for(int x = 0; x <size;x++){
                NoteAtom* note = dynamic_cast<NoteAtom*>((*container)[x]);
                if(note->pitch == line &&note->time < time && time < note->time+note->length){
                    found = x;
                    break;
                }
            }
            if(found == -1){
                //clicked empty space, clear selection.
                if (event->state & (1 << 0)) {//shift key was pressed...
                    //Do nothing, do not clear selection.
                } else {
                    //Empty space with no shift... clear selection.
                    selection.clear();
                    on_selection_changed.emit(selection.size());
                }
            }else{
                //clicked a note.
                if (event->state & (1 << 0)) {//shift key was pressed...
                    //we'll add the note to selection, unless it's already selected, then we de-select it.
                    std::set<int>::iterator it= selection.find(found);
                     if(it != selection.end()) {
                         //it's already selected, then:
                         //REMOVING NOTE FROM SELECTION
                         selection.erase(it);
                         
                         velocity = CalculateAverageVelocity();
                         on_selection_changed.emit(selection.size());
                     }else{
                         //it was not selected, select it.
                         //ADDING NOTE TO SELECTION
                         selection.insert(found);
                         
                         velocity = CalculateAverageVelocity();
                         on_selection_changed.emit(selection.size());
                     }
                } else {//shift key was not pressed
                    std::set<int>::iterator it= selection.find(found);
                     if(it != selection.end()) 
                         //it's already selected, then:
                         ;
                     else{
                         //it was not selected
                         //SELECTING A NEW NOTE
                         selection.clear();
                         selection.insert(found);
                         
                         velocity = CalculateAverageVelocity();
                         on_selection_changed.emit(selection.size());
                     }
                }
            }
            
            Redraw();
        } //(event->y <= internal_height)
    }
}

bool PatternWidget::on_button_release_event(GdkEventButton* event){
    if(event->button == 1)
        if(!selection_is_being_dragged){
            mouse_button_is_down = 0;
        }else{
            mouse_button_is_down = 0;
            selection_is_being_dragged = 0;
        }
    
}

bool PatternWidget::on_leave_notify_event(GdkEventCrossing* event){
    mouse_button_is_down = 0;
    selection_is_being_dragged = 0;
}

bool PatternWidget::on_motion_notify_event(GdkEventMotion* event){
    if(event->state & (1 << 8)){ //LMB down 
        if(!selection_is_being_dragged){
            if(mouse_button_is_down){
                if(!selection.empty()){
                    //if moved some distance, mark drag as in progress. 
                    //if we use SHIFT to precise moving, do not apply this distance, and mark as drag regardless of it.
                    const int distance = 9;
                    if(event->x > drag_beggining_x+distance || event->y > drag_beggining_y + distance || event->x < drag_beggining_x - distance || event->y < drag_beggining_y - distance) {

                        Gtk::Allocation allocation = get_allocation();
                        const int width = allocation.get_width();
                        const int height = allocation.get_height();
                        //count position
                        int line = 5 - drag_beggining_y / (internal_height / 6); //I HAVE NO IDEA WHY THERE SHOULD BE 5 AND NOT 6, DO NOT ASK THAT SEEMS TO BE ****** WEIRD
                        double time = (double) drag_beggining_x / (double) width;
                        //looking if there is a note where drag was began...
                        int found = -1;
                        int size = container->GetSize();
                        for (int x = 0; x < size; x++) {
                            NoteAtom* note = dynamic_cast<NoteAtom*> ((*container)[x]);
                            if (note->pitch == line && note->time < time && time < note->time + note->length) {
                                found = x;
                                break;
                            }
                        }
                        //and checking if it's in a selection
                        std::set<int>::iterator it= selection.find(found);
                        if(it!=selection.end()){// so it is in selection...
                            //===
                            //beggining drag. 
                            selection_is_being_dragged = 1;
                            drag_beggining_line = line;
                            drag_beggining_time = time;
                            //Store note offsets...
                            std::set<int>::iterator it = selection.begin();
                            for (;it!=selection.end();it++) {
                                NoteAtom* note = dynamic_cast<NoteAtom*> ((*container)[*it]);
                                //*err << "   " << line << ENDL;
                                note->drag_offset_line = note->pitch - line;
                                note->drag_offset_time = note->time-time;
                                //*err << note->drag_offset_line << " " << note->drag_offset_time << ENDL;
                            }
                            //===
                        }
                    }
                }
            }
        }else{ //drag in process
            Gtk::Allocation allocation = get_allocation();
            const int width = allocation.get_width();
            const int height = allocation.get_height();
            //count position
            int line = 6 - event->y / (internal_height / 6);
            double time = (double) event->x / (double) width;
            //*err << line << " " << time <<ENDL;
            
            std::set<int>::iterator it = selection.begin();
            for (; it != selection.end(); it++) {
                NoteAtom* note = dynamic_cast<NoteAtom*> ((*container)[*it]);
                int temp_pitch =  line+note->drag_offset_line;
                double temp_time = time+note->drag_offset_time;
                temp_pitch = temp_pitch%6; //wrap to 0-5;
                temp_time =  temp_time - (int)temp_time; //wrap to 0.0 - 0.9999...
                if(temp_pitch < 0) temp_pitch+= 6;
                if(temp_time < 0) temp_time += 1.0;
                note->pitch = temp_pitch;
                note->time =temp_time;
                //*err << " " << note->pitch << " " << note->time <<ENDL;
            }
            Redraw();
        }
        
    }
    
}

void PatternWidget::on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context){
    
}

  bool PatternWidget::on_expose_event(GdkEventExpose* event){
   cairo_t * c_t = gdk_cairo_create(event->window);
   Cairo::Context ct(c_t);
   //Cairo::RefPtr<Cairo::Context> cr(&ct);
    
   Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();
  
  int resolution = container->owner->resolution;
  
  //The +0.5 that often appears below in coordinates it to prevent cairo from antyaliasing lines.
    
  ct.set_line_width(3.0);
  ct.set_line_join(Cairo::LINE_JOIN_ROUND);
  if (container) //just in case it's NULL...
  for (int x = 0; x < container->GetSize(); x++){
      Atom* atm = (*container)[x];
      NoteAtom* note = dynamic_cast<NoteAtom*>(atm);
      double y1 = (5-note->pitch)*internal_height/6;
      double h = internal_height/6;
      double x1 = note->time*width;
      double w = note->length*width;
      ;y1++; // This is because the very first 1px line is the upper border.
    //*dbg << "drawing note... "<< x1 << " " << y1 << " " << w << " " <<  h << "\n";
      //Check if note is in selection.
      bool selected = false;
      if(selection.find(x) != selection.end()) selected = true;
      
      ct.rectangle(x1+1.5,y1+1.5,w-3,h-3);
      double af = (double)note->velocity/127; //may be changed to 128, this will not affect the graphics visibly, but may work slightly faster.
      if(!selected) ct.set_source_rgba(0.0,0.0,0.8,af);
      else ct.set_source_rgba(0.8,0.0,0.0,af);
      ct.fill_preserve();
      if(!selected) ct.set_source_rgb(0.0,0.0,0.4);
      else ct.set_source_rgb(0.4,0.0,0.0);
      ct.stroke();
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
  
  return true;
      
  }
