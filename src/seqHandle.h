/*
    Copyright (C) 2010, 2011 Rafał Cieślak

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

#ifndef SEQHANDLE_H
#define	SEQHANDLE_H

typedef unsigned int seqHandle;

class Sequencer;
/**Returns a pointer to a sequencer, taking it's handle as an argument.
    *So, as a matter of fact, it converts a handle to a sequencer.
    **/
Sequencer* seqH(seqHandle h);
/**Returns a pointer to a sequencer, taking it's ID in the vector as an argument.*/
Sequencer* seqV(int id);

seqHandle RequestNewSeqHandle(int ID);

int HandleToID(seqHandle h);

void UpdateSeqHandlesAfterDeleting(int ID);

void UpdateSeqHandlesAfterMoving(int from, int to);

/**Totally clears handle maps, and resets handle counter. USE WITH CARE only if all sequencers are removed and no handle is used!*/
void ResetSeqHandles();

void AddCustomSeqHandle(seqHandle h, int ID);
void ManuallySetSeqHandleCounter(int c);
#endif	/* SEQHANDLE_H */

