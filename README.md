harmonySEQ
---

A MIDI software sequencer designed for live performances and jams.

harmonySEQ operates a number of synchronized sequencers, each with its
own pattern, repeating looped melodies of different length, duration
and MIDI channel.

![harmonyseq UI](images/screenshot1.png?raw=true)

Main features include:

- support for complex polyrythms
- multi-pattern configuration for each sequencer
- customizable action triggers - exceptionally useful for live
  performances
- chord management system which allows to easily organize melodies
  that sound great together
- support for sequencing MIDI control messages
- an easy-to-use yet powerful interface, with a rich piano-roll
  pattern editor

As for now harmonySEQ works on Linux only. It uses ALSA (asound)
library, and GTKmm for graphical interface.

No official user documentation exists at the moment, but you can refer
to UI tooltips - nearly everything has a detailed explaination.

More information can be found on projects (dated) website: https://harmonyseq.wordpress.com

## Compiling and running

To run harmonySEQ you need to have {gtkmm-3.0,glibmm-2.4,alsa,liblo}
run-time libriries installed on your system (though you can disable
OSC support and liblo dependency by passing --disable-osc to configure
script). To compile it, you will also need the developement headers of
above libraries.

To compile and run w/o installing:
```
mkdir build && cd build
cmake ..
make -j4
./harmonySEQ
```

To compile and install:
```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
make install
```

To build a package for Debian 10:
```
apt install cmake build-essential libgtkmm-3.0-dev libglibmm-2.4-dev liblo-dev libasound2-dev
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
make -j4
cpack
```

