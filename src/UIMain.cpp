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

#include <deque>
#include <functional>
#include <mutex>
#include <string>
#include <thread>

#include <gtkmm.h>

#include "shared.hpp"


enum DispatcherState{
    /* The receiver thread did not prepare the dispatcher yet. */
    DISPATCHER_STATE_NOT_READY = 0,
    /* The dispatcher is ready to receive events. */
    DISPATCHER_STATE_READY = 1,
    /* The main thread has already stopped, it will no longer process events, and the dispatcher ends up in an invalid state. */
    DISPATCHER_STATE_CLOSED = 2,
};

Glib::Dispatcher* dispatcher = nullptr;
DispatcherState dispatcher_state = DISPATCHER_STATE_NOT_READY;

std::deque<std::function<void()>> ui_thread_work_queue;
std::mutex ui_thread_work_queue__mtx;

void DeferWorkToUIThread(std::function<void()> f){
    /* We previously used Glib::signal_idle(), but that approach is by
     * no means thread-safe.  In fact, using Glib::Dispatcher - like
     * we do instead - is pretty much the only safe way of notifying a
     * GTK thread of anything from a foreign thread. Quite silly, but
     * we have to live with it. */

    ui_thread_work_queue__mtx.lock();
    ui_thread_work_queue.push_back(f);
    ui_thread_work_queue__mtx.unlock();

    /* GUI thread did not yet create the dispatcher object. This only
     * happens during initialization. */
    if(dispatcher_state == DISPATCHER_STATE_NOT_READY) return;
    if(dispatcher_state == DISPATCHER_STATE_CLOSED) return;
    dispatcher->emit();
}

void WaitForDispatcher(){
    while(dispatcher_state == DISPATCHER_STATE_NOT_READY)
        std::this_thread::sleep_for (std::chrono::milliseconds(1));
}

void ProcessUIWorkQueue(){
    ui_thread_work_queue__mtx.lock();
    while(!ui_thread_work_queue.empty()){
        std::function<void()> f = ui_thread_work_queue.front();
        ui_thread_work_queue.pop_front();

        /* Unlock the queue mutex while running f() so that whatever
         * function was scheduled, it is able to schedule more work to
         * the queue with no deadlocks. */
        ui_thread_work_queue__mtx.unlock();
        f();
        ui_thread_work_queue__mtx.lock();
    }
    ui_thread_work_queue__mtx.unlock();
}

void UIMain(){
    dispatcher = new Glib::Dispatcher;
    dispatcher->connect(std::function<void()>(ProcessUIWorkQueue));
    dispatcher_state = DISPATCHER_STATE_READY;

    /* In case some events are waiting even before we got here. */
    ProcessUIWorkQueue();

    Gtk::Main::run();

    dispatcher_state = DISPATCHER_STATE_CLOSED;
    delete dispatcher;
    dispatcher = nullptr;
}


/* Some generic UI util functions. Need to find a better place to implement them. */

bool Ask(Gtk::Window* parent, std::string message, std::string secondary_message){
    Gtk::MessageDialog dialog(message, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
    if(parent)
        dialog.set_transient_for(*parent);
    dialog.set_secondary_text(secondary_message);
    int result = dialog.run();
    switch (result){
        case Gtk::RESPONSE_YES:
            return 1;
        case Gtk::RESPONSE_NO:
            return 0;
    }
    return 0;
}

void Info(Gtk::Window* parent, std::string message, std::string secondary_message){
    Gtk::MessageDialog dialog(message, false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
    if(parent)
        dialog.set_transient_for(*parent);
    dialog.set_secondary_text(secondary_message);
    dialog.run();
}
