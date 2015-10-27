/*
 *  This file is part of seq24/sequencer64.
 *
 *  seq24 is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  seq24 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with seq24; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * \file          font.cpp
 *
 *  This module declares/defines the base class for font handling.
 *
 * \library       sequencer64 application
 * \author        Seq24 team; modifications by Chris Ahlstrom
 * \date          2015-07-24
 * \updates       2015-10-26
 * \license       GNU GPLv2 or above
 *
 *  The interesting thing about this font class is that font files are not
 *  used.  Instead, the fonts are provided by pixmaps in the <tt>
 *  resources/pixmaps </tt> directory: Fonts are implemented as a relatively
 *  large bitmap that holds each of the font characters in a grid.  The grid
 *  is w = 16 cells wide by h = 16 cells high, to represent 256 characters.
 *  The coordinate of character c is (c % w, c / h).
 *
 *  Each character cell, including the unused border, is 10 pixels wide and
 *  14 pixels high.  Inside this cell is a 1-pixel blank border, or "inner
 *  padding".  The actual character size is 6 x 10 pixels.
 *
 *  We've also added some pixmaps for black lettering on a yellow background,
 *  and for yellow lettering on a black background, to handle "empty"
 *  patterns, which have no events, just some meta information.
 *
 *  Finally, we created another whole set of font pixmaps for an anti-aliased
 *  font.  This new font is selected by defining SEQ64_USE_NEW_FONT.  The new
 *  pixmaps aren't quite the same size, and the lettering is marginally
 *  larger, so alternative sizing variables are defined if the new font is in
 *  force.
 *
 *  The new font was created by using a short C program to create a 16x16 text
 *  file, importing it into a LibreOffice spreadsheet, setting the text to the
 *  WenQuanYi Zen Hei Mono bold font, adding some of the extended ASCII
 *  characters used in Seq24's old PC font, sizing the grid as closely as
 *  possible to the original, and using the GIMP to generate the XPM files.
 *  This process was actually easier than trying to figure out how to write
 *  text using gtkmm.  See the contrib/ascii-matrix.* files for the source
 *  material.
 *
 *  Also note we had to reduce the actual size of the characters slightly to
 *  render them properly in the cramped spaces of the GUI.
 *
 * \warning
 *      Some global sizes, such as c_names_x, may depend on aspects of the
 *      character size!
 */

const int cf_grid_w = 16;               /* number of horizontal font cells  */
const int cf_grid_h = 16;               /* number of vertical font cells    */

#define SEQ64_USE_NEW_FONT              /* currently for testing only       */

#ifdef SEQ64_USE_NEW_FONT
const int cf_cell_w = 11;               /* full width of character cell     */
const int cf_cell_h = 15;               /* full height of character cell    */
const int cf_offset =  3;               /* x, y offsets of top left pixel   */
const int cf_text_w =  6; // actual:  8 /* doesn't include inner padding    */
const int cf_text_h = 11; // actual: 12 /* ditto                            */
#else
const int cf_cell_w =  9;               /* full width of character cell     */
const int cf_cell_h = 13;               /* full height of character cell    */
const int cf_offset =  2;               /* x, y offsets of top left pixel   */
const int cf_text_w =  6;               /* doesn't include inner padding    */
const int cf_text_h = 10;               /* ditto                            */
#endif

#include "easy_macros.h"
#include "font.hpp"

#ifdef SEQ64_USE_NEW_FONT
#include "pixmaps/wenfont_w.xpm"        /* white on black (inverse video)   */
#include "pixmaps/wenfont_b.xpm"        /* black on white                   */
#include "pixmaps/wenfont_yb.xpm"       /* yellow on black (inverse video)  */
#include "pixmaps/wenfont_y.xpm"        /* black on yellow                  */
#else
#include "pixmaps/font_w.xpm"           /* white on black (inverse video)   */
#include "pixmaps/font_b.xpm"           /* black on white                   */
#include "pixmaps/font_yb.xpm"          /* yellow on black (inverse video)  */
#include "pixmaps/font_y.xpm"           /* black on yellow                  */
#endif

namespace seq64
{

/**
 *    Rote default constructor.
 */

font::font ()
 :
    m_font_w        (cf_text_w),
    m_font_h        (cf_text_h),
    m_pixmap        (nullptr),
    m_black_pixmap  (),
    m_white_pixmap  (),
    m_b_on_y_pixmap (),
    m_y_on_b_pixmap (),
    m_clip_mask     ()
{
   // empty body
}

/**
 *  Initialization function for a window on which fonts will be drawn.  This
 *  function loads four pixmaps that contain the characters to be used to draw
 *  text strings.
 *
 *  One pixmap has white characters on a black background, one has black
 *  characters on a white background, one has yellow characters on a black
 *  background, and one has black characters on a yellow background.
 */

void
font::init (Glib::RefPtr<Gdk::Window> wp)
{
#ifdef SEQ64_USE_NEW_FONT
    m_black_pixmap = Gdk::Pixmap::create_from_xpm
    (
        wp->get_colormap(), m_clip_mask, wenfont_b_xpm
    );
    m_white_pixmap = Gdk::Pixmap::create_from_xpm
    (
        wp->get_colormap(), m_clip_mask, wenfont_w_xpm
    );
    m_b_on_y_pixmap = Gdk::Pixmap::create_from_xpm
    (
        wp->get_colormap(), m_clip_mask, wenfont_y_xpm
    );
    m_y_on_b_pixmap = Gdk::Pixmap::create_from_xpm
    (
        wp->get_colormap(), m_clip_mask, wenfont_yb_xpm
    );
#else
    m_black_pixmap = Gdk::Pixmap::create_from_xpm
    (
        wp->get_colormap(), m_clip_mask, font_b_xpm
    );
    m_white_pixmap = Gdk::Pixmap::create_from_xpm
    (
        wp->get_colormap(), m_clip_mask, font_w_xpm
    );
    m_b_on_y_pixmap = Gdk::Pixmap::create_from_xpm
    (
        wp->get_colormap(), m_clip_mask, font_y_xpm
    );
    m_y_on_b_pixmap = Gdk::Pixmap::create_from_xpm
    (
        wp->get_colormap(), m_clip_mask, font_yb_xpm
    );
#endif
}

/**
 *  Draws a text string. This function grabs the proper font bitmap,
 *  extracts the current character pixmap from it, and slaps it down where
 *  it needs to be to render the character in the string.
 *
 * \param a_gc
 *      Provides the graphics context for drawing the text using GTK+.
 *
 * \param x
 *      The horizontal location of the text.
 *
 * \param y
 *      The vertical location of the text.
 *
 * \param a_draw
 *      The drawable object on which to draw the text.
 *
 * \param str
 *      The string to draw.  Should use a constant string reference
 *      instead.
 *
 * \param col
 *      The font color to use to draw the string.  The supported values are
 *      font::BLACK, font::WHITE, font::BLACK_ON_YELLOW,
 *      font::YELLOW_ON_BLACK.  The actual correct colors are provided by
 *      selecting one of four font pixmaps, as described in the init()
 *      function.
 */

void
font::render_string_on_drawable
(
    Glib::RefPtr<Gdk::GC> a_gc,
    int x, int y,
    Glib::RefPtr<Gdk::Drawable> a_draw,
    const char * str,
    font::Color col
)
{
    int length = 0;
    if (not_nullptr(str))
        length = strlen(str);

    if (col == font::BLACK)
        m_pixmap = &m_black_pixmap;
    else if (col == font::WHITE)
        m_pixmap = &m_white_pixmap;
    else if (col == font::BLACK_ON_YELLOW)
        m_pixmap = &m_b_on_y_pixmap;
    else if (col == font::YELLOW_ON_BLACK)
        m_pixmap = &m_y_on_b_pixmap;
    else
        m_pixmap = &m_black_pixmap;     // user lied, provide a legal pointer

    for (int i = 0; i < length; ++i)
    {
        unsigned char c = (unsigned char)(str[i]);
        int pixbuf_index_x = c % cf_grid_w;
        int pixbuf_index_y = c / cf_grid_h;
        pixbuf_index_x *= cf_cell_w;    // width of grid (letter is 6 pixels)
        pixbuf_index_x += cf_offset;    // add 2 for border?
        pixbuf_index_y *= cf_cell_h;    // height of grid (letter is 10 pixels)
        pixbuf_index_y += cf_offset;    // add 2 for border?
        a_draw->draw_drawable
        (
            a_gc, *m_pixmap, pixbuf_index_x, pixbuf_index_y,
            x + (i * m_font_w), y, m_font_w, m_font_h
        );
    }
}

}           // namespace seq64

/*
 * font.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

