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
 * \file          qseqtime.cpp
 *
 *  This module declares/defines the base class for drawing the
 *  time/measures bar at the top of the patterns/sequence editor.
 *
 * \library       sequencer64 application
 * \author        Seq24 team; modifications by Chris Ahlstrom
 * \date          2018-01-01
 * \updates       2018-03-11
 * \license       GNU GPLv2 or above
 *
 */

#include "Globals.hpp"
#include "qseqtime.hpp"
#include "sequence.hpp"

/*
 *  Do not document a namespace; it breaks Doxygen.
 */

namespace seq64
{

/**
 *
 */

qseqtime::qseqtime (sequence & seq, QWidget * parent)
 :
    QWidget     (parent),
    m_seq       (seq),
    m_timer     (new QTimer(this)),  // refresh timer to queue regular redraws
    m_font      (),
    m_zoom      (1)
{
    QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer->setInterval(50);
    m_timer->start();
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

/**
 *
 */

void
qseqtime::paintEvent (QPaintEvent *)
{
    QPainter painter(this);
    QPen pen(Qt::black);
    QBrush brush(Qt::lightGray, Qt::SolidPattern);
    m_font.setPointSize(6);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setFont(m_font);
    painter.drawRect             // draw time bar border
    (
        c_keyboard_padding_x, 0, size().width(), size().height() - 1
    );

    int measure_length_32nds = m_seq.get_beats_per_bar() * 32 /
        m_seq.get_beat_width();

    int measures_per_line = (128 / measure_length_32nds) / (32 / m_zoom);
    if (measures_per_line <= 0)
        measures_per_line = 1;

    int ticks_per_measure = m_seq.get_beats_per_bar() *
        (4 * c_ppqn) / m_seq.get_beat_width();

    int ticks_per_beat = ticks_per_measure * measures_per_line;
    int start_tick = 0;
    int end_tick = (m_seq.get_length());

    pen.setColor(Qt::black);
    painter.setPen(pen);
    for (int i = start_tick; i <= end_tick; i += ticks_per_beat)
    {
        int zoomedX = i / m_zoom + c_keyboard_padding_x;

        // vertical line at each beat

        painter.drawLine(zoomedX, 0, zoomedX, size().height());

        char bar[8];
        snprintf(bar, sizeof(bar), "%d", (i / ticks_per_measure) + 1);

        // number each beat

        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.drawText(zoomedX + 3, 10, bar);
    }

    long end_x = m_seq.get_length() / m_zoom + c_keyboard_padding_x;

    // draw end of seq label, label background

    pen.setColor(Qt::white);
    brush.setColor(Qt::white);
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    painter.setPen(pen);
    painter.drawRect(end_x + 1, 13, 15, 8);
    pen.setColor(Qt::black);                     // label text
    painter.setPen(pen);
    painter.drawText(end_x + 1, 21, tr("END"));
}

/**
 *
 */

void
qseqtime::mousePressEvent (QMouseEvent *)
{
    // no code
}

/**
 *
 */

void
qseqtime::mouseReleaseEvent (QMouseEvent *)
{
    // no code
}

/**
 *
 */

void
qseqtime::mouseMoveEvent(QMouseEvent *)
{
    // no code
}

/**
 *
 */

QSize
qseqtime::sizeHint() const
{
    return QSize(m_seq.get_length() / m_zoom + 100 + c_keyboard_padding_x, 22);
}

/**
 *
 */

void
qseqtime::zoom_in ()
{
    if (m_zoom > 1)
        m_zoom *= 0.5;
}

/**
 *
 */

void
qseqtime::zoom_out ()
{
    if (m_zoom < 32)
        m_zoom *= 2;
}

}           // namespace seq64

/*
 * qseqtime.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

