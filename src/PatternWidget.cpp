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
PatternWidget::PatternWidget(){
    internal_height=50; //random guess.
}

PatternWidget::~PatternWidget(){
}

void PatternWidget::SetInternalHeight(int h){
    internal_height = h;
    set_size_request(600,h+20);
}

void PatternWidget::SetResolution(int r){
    resolution = r;
    Redraw();
}

int PatternWidget::GetResoution(){
    return resolution;
}

void PatternWidget::Redraw(){
    queue_draw();
}

void PatternWidget::AssignPattern(AtomContainer* cont){
    *err << "assigning pattern \n";
    container = cont;
}

  bool PatternWidget::on_expose_event(GdkEventExpose* event){
   cairo_t * c_t = gdk_cairo_create(event->window);
   Cairo::Context ct(c_t);
   //Cairo::RefPtr<Cairo::Context> cr(&ct);
    
   Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();
  
  
  //The +0.5 that often appears below in coordinates it to prevent cairo from antyaliasing lines.
    
  if (container) //just in case it's NULL...
  for (int x = 0; x < container->GetSize(); x++){
      Atom* atm = (*container)[x];
      NoteAtom* note = dynamic_cast<NoteAtom*>(atm);
      double y1 = (5-note->pitch)*internal_height/6;
      double h = internal_height/6;
      double x1 = note->time*width;
      double w = note->length*width;
      ct.set_source_rgb(0.5,0.0,0.0);
      x1++;y1++; // This is because the very first 1px line is the upper border.
    *err << "drawing note... "<< x1 << " " << y1 << " " << w << " " <<  h << "\n";
      ct.rectangle(x1,y1,w,h);
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
        ct.move_to((int)((double)x*(double)width/resolution) + 0.5,0);
        ct.line_to((int)((double)x*(double)width/resolution) + 0.5,internal_height);
        ct.stroke();
  }
  
  return true;
      
  }