/*
    Copyright (C)  2011 Rafał Cieślak

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

#ifndef CONTROLLERATOM_H
#define	CONTROLLERATOM_H

#include "Atom.h"

enum SlopeType{
    SLOPE_TYPE_FLAT,
    SLOPE_TYPE_LINEAR,
            //maybe once there will be more to choose from?
    SLOPE_TYPE_NONE = 255 //used when the sequencer widget should display it's own setting
};

class ControllerAtom : public Atom {
public:
    ControllerAtom(double _time, int _value);
    ControllerAtom(const ControllerAtom& orig);
    virtual ~ControllerAtom();
    
    /**Creates a copy. Used as virtual constructor.*/
    virtual ControllerAtom* Clone();
    
    /**From 0 to 127*/
    int value;
    
    SlopeType slope_type;
private:

};

#endif	/* CONTROLLERATOM_H */

