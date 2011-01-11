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
#include "messages.h"
#include "EventsWindow.h"
#include "MidiDriver.h"
#include "Files.h"

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


#ifdef EVENTS_FLASH
    //Animating the row representing the triggered action.
    eventswindow->ColorizeAction(row_in_event_window);
#endif

    //Reactions depend on action type.
    switch (type){
        case SEQ_ON_OFF_TOGGLE:
            if (sequencers.size()==0 || !sequencers[args[1]]) break;
            switch (args[2]){
                case 0:
                    sequencers[args[1]]->SetOn(0);
                    break;
                case 1:
                    sequencers[args[1]]->SetOn(1);
                    break;
                case 2:
                    sequencers[args[1]]->SetOn(!sequencers[args[1]]->GetOn());
                    break;
            }
            break;

        case TEMPO_SET:
            mainwindow->tempo_button.set_value((double)args[1]);
            tempo = args[1];
            Files::SetFileModified(1);
            break;

        case SEQ_VOLUME_SET:
            if (sequencers.size()==0 || !sequencers[args[1]]) break;
            sequencers[args[1]]->SetVolume(args[2]);
            Files::SetFileModified(1);
            break;

        case SEQ_CHANGE_ONE_NOTE:
            if (sequencers.size()==0 || !sequencers[args[1]]) break;
            sequencers[args[1]]->chord.SetNote(args[2]-1, args[3]);
            sequencers[args[1]]->UpdateGuiChord(); //nessesary //its a temporary wokraround, since UpdateGui seems to crash. Howewer, it is not needed to update anything else than notes.
            Files::SetFileModified(1);
            break;

        case SEQ_CHANGE_CHORD:
            if (sequencers.size()==0 || !sequencers[args[1]]) break;
            sequencers[args[1]]->chord.Set(chord);
            sequencers[args[1]]->UpdateGuiChord(); //nessesary //its a temporary wokraround, since UpdateGui seems to crash. Howewer, it is not needed to update anything else than notes.
            Files::SetFileModified(1);
             break;
        case SEQ_PLAY_ONCE:
            if (sequencers.size()==0 || !sequencers[args[1]]) break;
            sequencers[args[1]]->SetPlayOncePhase(1);
            break;
        case TOGGLE_PASS_MIDI:
            passing_midi = !passing_midi;
            mainwindow->UpdatePassMidiToggle();
            break;
        case NONE:
            *dbg << "empty event triggered\n";
            break;
        case PLAY_PAUSE:
            switch(args[1]){
                case 0: //just pause
                    midi->PauseQueueImmediately();
                    break;
                case 1: //just play
                    if(!midi->GetPaused()) break; //if it is already playing, do not call Sync().
                    midi->ContinueQueue();
                    break;
                case 2: //toggle
                    if (midi->GetPaused()) { midi->ContinueQueue();}
                    else midi->PauseQueueImmediately();
                    break;
            }
            break;
        case SYNC:
            if (midi->GetPaused()) break; //do not sync while in pause!
            midi->Sync();
            break;
        case SEQ_CHANGE_PATTERN:
            if (sequencers.size()==0 || !sequencers[args[1]]) break;
            sequencers[args[1]]->ChangeActivePattern(args[2]);
            break;
        default:

            *err << _("WARNING: Unknown action triggered.");
            break;
    }
}

Glib::ustring Action::GetLabel(){
    char temp[100];
    switch (type){
        case SEQ_ON_OFF_TOGGLE:
            switch (args[2]){
                case 0:
                    sprintf(temp,_("Switch sequencer '%s' OFF"),GetSeqName(args[1]).c_str());
                    break;
                case 1:
                    sprintf(temp,_("Switch sequencer '%s' ON"),GetSeqName(args[1]).c_str());
                    break;
                case 2:
                    sprintf(temp,_("Toggle sequencer '%s'"),GetSeqName(args[1]).c_str());
                    break;
            }
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
        case SEQ_CHANGE_CHORD:
            sprintf(temp,_("Set chord of sequencer '%s' to %s"),GetSeqName(args[1]).c_str(),chord.GetName().c_str());
            break;
        case SEQ_PLAY_ONCE:
            sprintf(temp,_("Play sequence in '%s' once"),GetSeqName(args[1]).c_str());
            break;
        case TOGGLE_PASS_MIDI:
            sprintf(temp,_("Toggle passing MIDI events"));
            break;
        case PLAY_PAUSE:
            switch (args[1]){
                case 0:
                    sprintf(temp,_("Pause"));
                    break;
                case 1:
                    sprintf(temp,_("Play"));
                    break;
                case 2:
                    sprintf(temp,_("Toggle pause/play"));
                    break;
            }
            break;
        case SYNC:
            sprintf(temp,_("Synchronize"));
            break;
        case SEQ_CHANGE_PATTERN:
            sprintf(temp,_("Set active pattern of sequencer '%s' to %d"),GetSeqName(args[1]).c_str(),args[2]);
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
    if (n >= sequencers.size()) {
        if (sequencers.size() > 0) *err << _("Critical ERROR: Trying to get name of sequencer outside of sequencers vector.\n");
        return "(none)";
    }
    if (!sequencers[n])
        sprintf(temp,_("%d (which was removed)"),n);
    else
        sprintf(temp,_("%s"),sequencers[n]->GetName().c_str());
    return temp;
}

void Action::GUIShowWindow(){
    gui_window->show();
    gui_window->raise();
}

void Action::GUIUpdateChordwidget(){
    gui_window->UpdateChordwidget();
}