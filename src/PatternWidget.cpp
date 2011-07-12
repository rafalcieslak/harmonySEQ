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

#include <gtk-2.0/gdk/gdkcairo.h>

#include "PatternWidget.h"
#include "cairomm/context.h"
#include "global.h"
#include "messages.h"
PatternWidget::PatternWidget(){
}



PatternWidget::~PatternWidget(){
}


  bool PatternWidget::on_expose_event(GdkEventExpose* event){
   cairo_t * c_t = gdk_cairo_create(event->window);
   Cairo::Context ct(c_t);
   //Cairo::RefPtr<Cairo::Context> cr(&ct);
    
   Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();
  
  ct.set_line_width(2);
  ct.set_source_rgb(1.0,0.0,1.0);
  
  ct.move_to(-width/2,-height/2);
  ct.line_to(width/2,height/2);
  
  ct.stroke();
  
  return true;
      
  }