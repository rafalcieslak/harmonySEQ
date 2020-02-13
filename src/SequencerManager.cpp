/*
    Copyright (C) 2020 Rafał Cieślak

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

#include <algorithm>
#include "SequencerManager.hpp"

std::vector<std::shared_ptr<Sequencer>> SequencerManager::sequencers;
std::mutex SequencerManager::sequencers_mtx;

bs2::signal<void()> SequencerManager::on_sequencer_list_changed;


std::vector<std::shared_ptr<Sequencer>> SequencerManager::GetAll() {
    std::lock_guard lock(sequencers_mtx);
    return sequencers;
}

void SequencerManager::Register(std::shared_ptr<Sequencer> seq){
    {
        std::lock_guard lock(sequencers_mtx);
        sequencers.push_back(seq);
    }
    on_sequencer_list_changed();
}

void SequencerManager::Unregister(std::shared_ptr<Sequencer> seq){
    {
        std::lock_guard lock(sequencers_mtx);
        sequencers.erase(
            std::remove(sequencers.begin(), sequencers.end(), seq),
            sequencers.end());
    }
    on_sequencer_list_changed();
}

void SequencerManager::Clear(){
    {
        std::lock_guard lock(sequencers_mtx);
        sequencers.clear();
    }
    on_sequencer_list_changed();
}


void SequencerManager::ReplaceAll(std::vector<std::shared_ptr<Sequencer>> new_set){
    {
        std::lock_guard lock(sequencers_mtx);
        sequencers = new_set;
    }
    on_sequencer_list_changed();
}
