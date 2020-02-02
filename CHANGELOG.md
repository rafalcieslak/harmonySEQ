## unreleased:
 - Replaced the build system with CMake.
 - HiDPI display support.
 - MIDI Clock and Start/Stop messages output support.
 - Fixed pattern editor zoom behavior.
 - Disabled i18n support to allow for cleaning up the build system.

## harmonyseq 0.16.1 (2020-02-01):
 - Maintenance only release.
 - Ported user interface code to use GTK3 and gtkmm-3.0 / 3.24.
 - Fixed compatibility with modern C++ compilers.
 - Cleaned up unnecessary autotool files.
 - Fixed a major race condition related to opening files while sequencer diodes were operating.

## harmonyseq 0.16 "ranpiollo" (2012-02-04)
 - Implemented an advanced piano-roll interface for sequencers.
 - Added a new type of sequencer - control sequencers, that are used to output MIDI event messages.
 - Added support for custom lengths and resolutions (this allows to play melodies in any time signature).
 - Added chart-like display for control sequencers, which makes it easy to edit their output.
 - Velocity is now applied to all notes separately.
 - German translation, thanks to Jan Fuchsman.
 - Basic Japanese translation, by Nel Pogorzelska.
 - New system for opening files, which allows to automatically convert older files.
 - Added possibility to hide chord's detailed settings.
 - Added some new chords.
 - Significant bug fixes (including much more efficient output routines, which result in MUCH less lag during playback).

## harmonyseq 0.15 "arising" (2011-05-07)
 - Reorganized sequencer window, significantly improving the way a pattern is presented.
 - Merged the sequencer window and the events window into the main window.
 - Added colourful flashing playback indicators.
 - Added 4th note indicators.
 - Added PlayOnEdit feature.
 - Implemented OSC support.
 - Allowed reordering sequencers (this time seriously).
 - Actions that change chords overwrite octave only optionally (one can change chord without changing octave).
 - Added an action that transposes a sequencer by an octave.
 - New event that allows triggering events by an OSC message.
 - Increased tempo range, as some may need higher tempos.
 - Added a button that clears selected pattern.
 - Significantly improved data refreshing speed.
 - Many optimizations, in some cases harmonySEQ may use now up to 30% less CPU.
 - Improved support for unusual Gtk system themes.
 - Changed sequencer's default resolution to 16.
 - Many bugs fixed, and some minor GUI improvements. 

## harmonyseq 0.14 "enlighted" (2011-01-25)
 - Moved GUI from buttons to an elegant toolbar and menu.
 - Reorganized chord logic and GUI.
 - Added a customizable metronome.
 - Added Preferences window for customizing harmonySEQ's settings - stored in ~/.harmonySEQ/harmonySEQ.conf.
 - Added many tooltips to make it easier to use harmonySEQ.
 - Added a pop-up menu for the sequencer list.
 - Added possibility to hide/show columns in the main window as a part of harmonySEQ's configuration.
 - Some major GUI improvements.
 - Fixed many bugs.
 - Added the README file.
 - Source code heavily cleaned-up.

## harmonyseq 0.13 "polyphony" (2010-12-08)
 - Using checkboxes instead of sliders in seqencer GUI makes it now possible to produce polyponic voices using one sequencer
 - Added menubar to improve GUI
 - Added AboutBox
 - Included harmonySEQ icon and used it used widely
 - Basic shortcuts support
 - Added integration with desktop - menu entry, icon, MIME type support

## harmonyseq 0.12 (2010-11-04)
 - Added ability to bind multiple melodies to one sequencer
 - Added Play/Pause button
 - Added Sync action, that may be useful to synchronize harmonySEQ with an external application
 - Improved interaction with user
 - Recognizing wheter the file was modified since it was last saved
 - Added Play/pause action
 - Added action changing melodies in a specified sequencer
 - Reorganized Seq/on/off/toggle actions (merged into one action type), may cause problems when loading older version files.
 - Minor GUI improvements

## harmonyseq 0.11 (2010-10-08)
 - Advanced chord selection
 - Added action setting a sequencer's chord
 - Added action that plays a sequence once
 - Coloring rows in main window depending on sequencer state
 - Added guitar demo and self control demo
 - Minor GUI updates

## harmonyseq 0.10 (2010-09-27)
 - Events and actions implemented (as for now featuring only few events/actions types).
 - Midi events can be now passed-through (allows playing live on external controller)
 - Variable volume.
 - Ported to GNU Autotools.
 - Updated example files.
 - Opening files from command line.
 - Improved file opening/saving.
 - Many minor GUI fixes and improvements.

## harmonyseq 0.9 
 - Sequencers can be now safely dragged.
 - Sorting by several columns implemented (olny several, since it widens the columns).
 - Sequences may have different resolutions
 - User can choose sequence length

## harmonyseq 0.8
 - File access rearanged
 - Sequencers may be now removed
 - ID's hidden in non-debug mode
 - Sequencers cab be now cloned
 - Flashing tempo indicator

## harmonyseq 0.7 (2010-09-05)
 - Saving and loading files

## harmonyseq 0.6
 - Options affecting sequencers can be now set either in the main window or
        in the sequencer window.
 - Sequencer channel is displayed in the main window
 - Muted-unmuted changed to On-off

## harmonyseq 0.5 (2010-09-01)
 - Sequencers may be now added
 - Muting and name changing can be done through main window
 - Minor fixes

## repo initialized (2010-08-18)
