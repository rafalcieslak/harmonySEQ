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

#include <functional>
#include <deque>
#include <mutex>
#include <glibmm/dispatcher.h>
#include <gtkmm/main.h>
#include <thread>
#include <chrono>

Glib::Dispatcher* dispatcher = nullptr;

std::deque<std::function<void()>> ui_thread_work_queue;
std::mutex ui_thread_work_queue__mtx;

void DeferWorkToUIThread(std::function<void()> f){
    /* We previously used Glib::signal_idle(), but that approach is by
     * no means thread-safe.  In fact, using Glib::Dispatcher - like
     * we do instead - is pretty much the only safe way of notifying a
     * GTK thread of anything from a foreign thread. Quite silly, but
     * we have to live with it. */

    /* GUI thread did not yet create the dispatcher object - wait
     * until it does. This only happens once on startup. */
    while(dispatcher == nullptr) std::this_thread::sleep_for (std::chrono::milliseconds(1));

    ui_thread_work_queue__mtx.lock();
    ui_thread_work_queue.push_back(f);
    ui_thread_work_queue__mtx.unlock();
    dispatcher->emit();
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
    Gtk::Main::run();
}
