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

#include "seqHandle.h"
#include "global.h"
#include "messages.h"

std::map<seqHandle, int> seqHandlesMap;
unsigned int handlecounter = 1;

void PrintHandlesMap(){
    //debug function, prints how does the map look like.
    *dbg << "Now the handles map looks like:" << ENDL;
    int a = 0,b = 0;
  for (std::map<seqHandle,int>::iterator it=seqHandlesMap.begin() ; it != seqHandlesMap.end(); it++ ){
      a = it->first;
      b = it->second;

    *dbg << a << " => " << b << ENDL;
  }
}

Sequencer* seqH(seqHandle h){
    std::map<seqHandle,int>::iterator iter;
    iter = seqHandlesMap.find(h);
    if (iter != seqHandlesMap.end()){
        if (iter->second == -1) return NULL;
        else return seqVector[iter->second];
    } else
        return NULL;
}

Sequencer* seqV(int id){
    if (id < (int)seqVector.size()){
        return seqVector[id];
    }else
        return NULL;
    
}

seqHandle RequestNewSeqHandle(int ID){
    unsigned int h = handlecounter;
    handlecounter++;
    
    seqHandlesMap[h] = ID;
    return h;
}

int HandleToID(seqHandle h){
    std::map<seqHandle,int>::iterator iter;
    iter = seqHandlesMap.find(h);
    if (iter != seqHandlesMap.end()){
        return iter->second;
    } else
        return -1;
}


void UpdateSeqHandlesAfterDeleting(int ID){
    //for each element in a map...
    for (std::map<seqHandle,int>::iterator it = seqHandlesMap.begin(); it != seqHandlesMap.end(); it++){
        if (it->second < ID) continue;
        else if (it->second == ID) it->second = -1; //if used, this WILL case an error soon and grab developer's attetion
        else it->second--;
    }
    PrintHandlesMap();

}


void UpdateSeqHandlesAfterMoving(int from, int to){
    if (from == to) return;
    else if (from < to){//moved downwards
        //for each element in a map...
        *dbg << from << " - " << to <<ENDL;
        for (std::map<seqHandle, int>::iterator it = seqHandlesMap.begin(); it != seqHandlesMap.end(); it++) {
            if (it->second > from && it->second <= to) it->second--;
            else if (it->second == from) it->second = to;
        }
    }else{//moved upwards
        //for each element in a map...
        for (std::map<seqHandle, int>::iterator it = seqHandlesMap.begin(); it != seqHandlesMap.end(); it++) {
            if (it->second < from && it->second >= to) it->second++;
            else if (it->second == from) it->second = to;
        }
        
    }
    PrintHandlesMap();
}

void ResetSeqHandles(){
    seqHandlesMap.clear();
    handlecounter = 1;

}


void AddCustomSeqHandle(seqHandle h, int ID){
    seqHandlesMap[h] = ID;
}