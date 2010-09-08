#define I_DO_NOT_WANT_EXTERNS_FROM_GLOBAL_H
#include "global.h"
#undef I_DO_NOT_WANT_EXTERNS_FROM_GLOBAL_H
class Sequencer;
class debug;
class error;
class MidiDriver;
class MainWindow;
vector<Sequencer *> sequencers(2);
int mainnote = 60;
double tempo = DEFAULT_TEMPO;
int ports_number;
int running = 1;
debug* dbg;
error* err;
MidiDriver* midi;
MainWindow* mainwindow;