#ifndef SEQUENCERMANAGER_HPP
#define SEQUENCERMANAGER_HPP

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

#include <vector>
#include <memory>
#include <mutex>

#include "global.hpp"

class Sequencer;

/* This class may be come instanciable one day */
class SequencerManager {
public:

    /* Atomically copies the list of all sequencers, and retruns
     * it. This is useful for processing all sequencers without
     * holding the mutex for long. */
    static std::vector<std::shared_ptr<Sequencer>> GetAll();

    /* Returns the number of registered sequencers. */
    static unsigned int GetCount();

    /* Adds a sequencer to the list. */
    static void Register(std::shared_ptr<Sequencer>);
    /* Removes a sequencer from the list. */
    static void Unregister(std::shared_ptr<Sequencer>);

    /* Substitutes the internal list of sequencers with the provided
     * list. This is useful e.g. when opening files. */
    static void ReplaceAll(std::vector<std::shared_ptr<Sequencer>>);

    /* Unregisters all sequencers. */
    static void Clear();

    static bs2::signal<void()> on_sequencer_list_changed;

private:
    static std::vector<std::shared_ptr<Sequencer>> sequencers;
    static std::mutex sequencers_mtx;

};

#endif // SEQUENCERMANAGER_HPP
