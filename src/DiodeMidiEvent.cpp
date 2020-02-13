/*
    Copyright (C) 2011 Rafał Cieślak

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

#include "DiodeMidiEvent.h"


DiodeMidiEvent::DiodeMidiEvent(std::weak_ptr<Sequencer> target_, DiodeType type_, double time_, int value_, int color_, int max_res_){
    target = target_;
    type = type_;
    time = time_;
    value = value_;
    max_res = max_res_;
    color = color_;
}

DiodeMidiEvent::DiodeMidiEvent(const DiodeMidiEvent& orig){
    target = orig.target;
    type = orig.type;
    time = orig.time;
    value = orig.value;
    max_res = orig.max_res;
    color = orig.color;
}

DiodeMidiEvent::~DiodeMidiEvent(){
}

DiodeMidiEvent & DiodeMidiEvent::operator =(const DiodeMidiEvent& other){
    if(this != &other){
        target = other.target;
        type = other.type;
        time = other.time;
        value = other.value;
        max_res = other.max_res;
        color = other.color;

    }
    return *this;
}
