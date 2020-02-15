#ifndef SHARED_HPP
#define SHARED_HPP

#include <map>
#include <string>
#include <functional>

#include <glibmm/datetime.h>


// Macro for easier internalisation.
// Gettext was disabled during source code cleanup, as it's difficult
// to maintain, and we'd rather focus on other features.
#define _(STRING) (STRING)


#define DEFAULT_TEMPO 120.0


void DeferWorkToUIThread(std::function<void()> f);


namespace Gtk {class Window;}

/**Ask user a YES-NO question.*/
bool Ask(Gtk::Window* parent, std::string message, std::string secondary_message = "");
/**Shows user a message.*/
void Info(Gtk::Window* parent, std::string message, std::string secondary_message = "");

/** Return current wallclock timestamp as a double **/
inline double GetRealTime(){
    Glib::DateTime dt = Glib::DateTime::create_now_utc();
    return dt.to_unix() + dt.get_microsecond()/1000000.0;
}

void WaitForDispatcher();
void UIMain();

/* Maps used for key-name convesions. Should be abstracted away to
 * some separate file. */
extern std::map<std::string, int> keymap_stoi;
extern std::map<int, std::string> keymap_itos;


#endif
