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

#ifndef COLOR_HPP
#define COLOR_HPP


/* Gdk::RGBA color representation lacks any flexibility and Cairo
   comparibility, so we implement our own wrapper that has some
   convenience functions. */

#include <gdkmm/rgba.h>
#include <cairomm/context.h>

struct Color {
    Color() {}

    Color(double r_, double g_, double b_, double a_ = 1.0) {
        r = r_, g = g_, b = b_, a = a_;
    }

    Color(const char *text){
        Gdk::RGBA c(text);
        r = c.get_red();
        g = c.get_green();
        b = c.get_blue();
        a = c.get_alpha();
    }

    Color(const Gdk::RGBA& c){
        r = c.get_red();
        g = c.get_green();
        b = c.get_blue();
        a = c.get_alpha();
    }

    /* Intentionally public - this is a data class. */
    double r, g, b, a;

    operator Gdk::RGBA() const {
        Gdk::RGBA c;
        c.set_rgba(r, g, b, a);
        return c;
    }

    operator GdkRGBA() const{
        GdkRGBA c;
        c.red = r;
        c.green = g;
        c.blue = b;
        c.alpha = a;
        return c;
    }

    void to_context(Cairo::RefPtr<Cairo::Context> ct) const{
        ct->set_source_rgba(r, g, b, a);
    }

    Color with_a(double a_) {
        Color c(*this);
        c.a = a_;
        return c;
    }
};

#endif // COLOR_HPP
