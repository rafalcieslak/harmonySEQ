/*
    Copyright (C) 2011, 2020 Rafał Cieślak

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

#include "ControlSequencer.hpp"


ControlSequencer::ControlSequencer()
                                                                : Sequencer(){
    controller_number = 1;
    resolution = 32;
}

ControlSequencer::ControlSequencer(Glib::ustring _name0)
                                                                : Sequencer(_name0){
    controller_number = 1;
    resolution = 32;
}

ControlSequencer::ControlSequencer(const ControlSequencer& orig)
                                                                : Sequencer(orig){
    controller_number = orig.controller_number;
}


ControlSequencer::~ControlSequencer(){
}

SeqType_t ControlSequencer::GetType(){
    return SEQ_TYPE_CONTROL;
}

std::shared_ptr<Sequencer> ControlSequencer::Clone(){
    return std::shared_ptr<ControlSequencer>(new ControlSequencer(*this));
}

void ControlSequencer::SetControllerNumber(int number){
    controller_number = number;
    on_parameter_change();
}
int ControlSequencer::GetControllerNumber(){
    return controller_number;
}
