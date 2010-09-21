/*
    Copyright (C) 2010 Rafał Cieślak

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

#include "Action.h"
#include "Sequencer.h"


Action::Action(ActionTypes t, int a1, int a2){
    type = t;
    arg1 = a1;
    arg2 = a2;
}


Action::Action(const Action& orig){
}


Action::~Action(){
}

void Action::Trigger(int data){
    *dbg << "-- Action triggered '" << GetLabel() << "'.\n";
    switch (type){
        case SEQ_TOGGLE:
            if (!sequencers[arg1]) break;
            sequencers[arg1]->SetOn(!sequencers[arg1]->GetOn());
            break;
        case SEQ_OFF:
            if (!sequencers[arg1]) break;
            sequencers[arg1]->SetOn(1);
            break;
        case SEQ_ON:
            if (!sequencers[arg1]) break;
            sequencers[arg1]->SetOn(0);
            break;

        case MAINOTE_SET:
            mainnote = arg1;
            break;

        case TEMPO_SET:
            tempo = arg2;
            break;

        case SEQ_VOLUME_SET:
            if (!sequencers[arg1]) break;
            sequencers[arg1]->SetVolume(arg2);

        case NONE:
            *dbg << "empty event triggered\n";
            break;
    }
}

Glib::ustring Action::GetLabel(){
    return "An action";

}