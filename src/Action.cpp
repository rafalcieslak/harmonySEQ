/*
    Copyright (C) 2010-2012, 2020 Rafał Cieślak

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

#include "Action.hpp"

#include <functional>
#include <memory>

#include "ControlSequencer.hpp"
#include "Files.hpp"
#include "Engine.hpp"
#include "NoteSequencer.hpp"
#include "SequencerManager.hpp"
#include "Sequencer.hpp"

extern Engine* engine;


Action::Action(ActionTypes t, int a1, int a2){
    args.resize(ACTION_ARGS_NUM);
    type = t;
    args[1] = a1;
    args[2] = a2;

    seq_list_changed_conn = SequencerManager::on_sequencer_list_changed.connect(std::bind(&Action::OnSeqListChanged, this));
}

Action::~Action(){
    seq_list_changed_conn.disconnect();
}

void Action::CopyInto(Action& a) const{
    a.type = type;
    a.args = args;
    chord.CopyInto(a.chord);
    a.target_seq = target_seq;
}

void Action::Trigger(int data){

    //Reactions depend on action type.
    if(type == SEQ_ON_OFF_TOGGLE) {
        auto seq = target_seq.lock();
        if(!seq) return;
        switch (args[2]){
        case 0:
            seq->SetOn(0);
            break;
        case 1:
            seq->SetOn(1);
            break;
        case 2:
            seq->SetOn(!seq->GetOn());
            break;
        }
    }else if(type == TEMPO_SET){
        engine->SetTempo(args[1]);
        Files::SetFileModified(1);
    }else if(type == SEQ_CHANGE_ONE_NOTE){
        auto noteseq = std::dynamic_pointer_cast<NoteSequencer>(target_seq.lock());
        if(!noteseq) return;
        noteseq->chord.SetNote(args[2]-1, args[3]);
        Files::SetFileModified(1);
    }else if(type == SEQ_CHANGE_CHORD){
        auto noteseq = std::dynamic_pointer_cast<NoteSequencer>(target_seq.lock());
        if(!noteseq) return;
        noteseq->chord.Set(chord,!args[3]);
        Files::SetFileModified(1);
    }else if(type == SEQ_PLAY_ONCE){
        auto seq = target_seq.lock();
        if(!seq) return;
        seq->SetPlayOncePhase(1);
    }else if(type == NONE){

    }else if(type == PLAY_PAUSE){
        switch(args[1]){
        case 0: //just pause
            engine->PauseImmediately();
            break;
        case 1: //just play
            if(!engine->GetPaused()) break; //if it is already playing, do not call Sync().
            engine->Unpause();
            break;
        case 2: //toggle
            if (engine->GetPaused()) { engine->Unpause();}
            else engine->PauseImmediately();
            break;
        }
    }else if(type == SYNC){
        engine->Sync();
    }else if(type == SEQ_CHANGE_PATTERN){
        auto seq = target_seq.lock();
        if(!seq) return;
        seq->SetActivePatternNumber(args[2]);
    }else if(type == SEQ_TRANSPOSE_OCTAVE){
        auto noteseq = std::dynamic_pointer_cast<NoteSequencer>(target_seq.lock());
        if(!noteseq) return;
        noteseq->chord.SetBaseOctave(noteseq->chord.GetBaseOctave()+args[2]);
        Files::SetFileModified(1);
    }else if(type == TOGGLE_PASS_MIDI){
        // No longer supported.
    }else{
        fprintf(stderr, "WARNING: Unknown action triggered.\n");
    }

    on_trigger();
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

        case SEQ_CHANGE_ONE_NOTE:
            sprintf(temp,_("Set note %d of sequencer '%s' to %d"),args[2],GetSeqName(args[1]).c_str(),args[3]);
            break;
        case SEQ_CHANGE_CHORD:
            sprintf(temp,_("Set chord of sequencer '%s' to %s"),GetSeqName(args[1]).c_str(),chord.GetName(args[3]).c_str());
            break;
        case SEQ_PLAY_ONCE:
            sprintf(temp,_("Play sequence in '%s' once"),GetSeqName(args[1]).c_str());
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
        case SEQ_TRANSPOSE_OCTAVE:
            if (args[2] < 0){
                sprintf(temp,_("Decrease sequencer '%s' chord's  octave by %d"),GetSeqName(args[1]).c_str(),-1*args[2]);
            }else{
                sprintf(temp,_("Increase sequencer '%s's chord's octave by %d"),GetSeqName(args[1]).c_str(),args[2]);
            }
            break;
        case NONE:
            sprintf(temp,_("(empty action)"));
            break;
        case TOGGLE_PASS_MIDI:
            sprintf(temp,_("(unsupported)"));
            break;
        default:
            sprintf(temp,_("(unknown action)"));
    }
    return temp;
}

Glib::ustring Action::GetSeqName(int h){
    char temp[100];
    auto seq = target_seq.lock();
    if (!seq)
        sprintf(temp,_("(none)"));
    else
        sprintf(temp,_("%s"),seq->GetName().c_str());
    return temp;
}

void Action::OnSeqListChanged(){
    /* TODO: We would prefer a "on_sequencer_unregistered" signal. */

    auto seq = target_seq.lock();
    if (!seq or !SequencerManager::IsRegistered(seq)){
        /* Looks like this seq was just removed. Emit a on_changed
         * signal since our label has just changed, even though action
         * values did not. */
        on_changed();
    }
}
