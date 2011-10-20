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

#ifndef DIODEMIDIEVENT_H
#define	DIODEMIDIEVENT_H

enum DiodeType{
    DIODE_TYPE_NOTE,
    DIODE_TYPE_CTRL,
    DIODE_TYPE_GRID
};

class DiodeMidiEvent {
public:
    DiodeMidiEvent(DiodeType type_, double time_, int value_, int color_, int max_res_ = 0);
    DiodeMidiEvent(const DiodeMidiEvent &orig);
    virtual ~DiodeMidiEvent();
    
    DiodeMidiEvent & operator= (const DiodeMidiEvent & other);
    
    DiodeType type;
    double time;
    /**0-127 for crtls, 0-5 for notes, 0-res grid*/
    int value;
    /**Used for grid diodes to indicate maximum resolution. PatternWidget would use that to calculate distance between grid diodes.*/
    int max_res;
    /** 0 - green, 1 - yellow*/
    int color;
private:

};

#endif	/* DIODEMIDIEVENT_H */

