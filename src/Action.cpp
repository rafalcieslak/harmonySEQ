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
#include "MainWindow.h"


Action::Action(ActionTypes t, int a1, int a2){
    args.resize(ACTION_ARGS_NUM);
    type = t;
    args[1] = a1;
    args[2] = a2;
    gui_window = new ActionGUI(this);
}


Action::Action(const Action& orig){
}


Action::~Action(){
    delete gui_window;
}

void Action::Trigger(int data){
    *dbg << "-- Action triggered '" << GetLabel() << "'.\n";
    eventswindow->ColorizeAction(row_in_event_window);

    switch (type){
        case SEQ_TOGGLE:
            if (!sequencers[args[1]]) break;
            sequencers[args[1]]->SetOn(!sequencers[args[1]]->GetOn());
            break;
        case SEQ_OFF:
            if (!sequencers[args[1]]) break;
            sequencers[args[1]]->SetOn(0);
            break;
        case SEQ_ON:
            if (!sequencers[args[1]]) break;
            sequencers[args[1]]->SetOn(1);
            break;

        case MAINOTE_SET:
            mainwindow->main_note.set_value((double)args[1]);
            mainnote = args[1];
            break;

        case TEMPO_SET:
            mainwindow->tempo_button.set_value((double)args[1]);
            tempo = args[1];
            break;

        case SEQ_VOLUME_SET:
            if (!sequencers[args[1]]) break;
            sequencers[args[1]]->SetVolume(args[2]);
            break;

        case SEQ_CHANGE_ONE_NOTE:
            if (!sequencers[args[1]]) break;
            sequencers[args[1]]->notes[args[2]-1] = args[3];
            sequencers[args[1]]->UpdateGuiNotes(); //nessesary //its a temporary wokraround, since UpdateGui seems to crash. Howewer, it is not needed to

        case NONE:
            *dbg << "empty event triggered\n";
            break;

        default:

            *err << _("WARNING: Unknown action triggered.");
    }
}

Glib::ustring Action::GetLabel(){

    char temp[100];
    switch (type){
        case SEQ_TOGGLE:
            sprintf(temp,_("Toggle sequencer '%s'"),GetSeqName(args[1]).c_str());
            break;
        case SEQ_OFF:
            sprintf(temp,_("Switch sequencer '%s' OFF"),GetSeqName(args[1]).c_str());
            break;
        case SEQ_ON:
            sprintf(temp,_("Switch sequencer '%s' ON"),GetSeqName(args[1]).c_str());
            break;

        case MAINOTE_SET:
            sprintf(temp,_("Set main note to %d"),args[1]);
            break;

        case TEMPO_SET:
            sprintf(temp,_("Set tempo to %d BPM"),args[1]);
            break;

        case SEQ_VOLUME_SET:
            sprintf(temp,_("Set volume of sequencer '%s' to %d"),GetSeqName(args[1]).c_str(),args[2]);
            break;
        case SEQ_CHANGE_ONE_NOTE:
            sprintf(temp,_("Set note %d of sequencer '%s' to %d"),args[2],GetSeqName(args[1]).c_str(),args[3]);
            break;
        case NONE:
            sprintf(temp,_("(empty action)"));
            break;
        default:
            sprintf(temp,_("(unknown action)"));
    }
    return temp;
}

Glib::ustring Action::GetSeqName(int n){
    char temp[100];
    if (!sequencers[n])
        sprintf(temp,_("%d (which was removed)"),n);
    else
        sprintf(temp,_("%s"),sequencers[n]->GetName().c_str());
    return temp;
}

void Action::ShowWindow(){
    gui_window->show();

}