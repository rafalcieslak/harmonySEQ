/* 
 * File:   MidiDriver.h
 * Author: cielak
 *
 * Created on 18 sierpień 2010, 17:14
 */
#include <alsa/asoundlib.h>

#ifndef MIDIDRIVER_H
#define	MIDIDRIVER_H
#include "global.h"

class MidiDriver {
public:
    snd_seq_t* seq_handle;
    int output_port,output_port2, input_port;
    int queueid;
    bool working;
    double tempo;
    int tick;
    MidiDriver();
    void SendNoteEvent(int pitch, int volume);
    void Open();
    void InitQueue();
    void SetTempo(double bpm);
    void ClearQueue();
    void DeleteQueue();
    void UpdateQueue();
    void ProcessInput();
    MidiDriver(const MidiDriver& orig);
    virtual ~MidiDriver();
private:

};

#endif	/* MIDIDRIVER_H */

