/**************************************************************************
 *   move.c                                                               *
 *                                                                        *
 *   Copyright (C) 1999 Chris Allegretta                                  *
 *   This program is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation; either version 1, or (at your option)  *
 *   any later version.                                                   *
 *                                                                        *
 *   This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *   GNU General Public License for more details.                         *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program; if not, write to the Free Software          *
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.            *
 *                                                                        *
 **************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "config.h"
#include "proto.h"
#include "nano.h"

#ifndef NANO_SMALL
#include <libintl.h>
#define _(string) gettext(string)
#else
#define _(string) (string)
#endif

void page_down_center(void)
{
    if (editbot->next != NULL && editbot->next != filebot) {
	edit_update(editbot->next);
	center_cursor();
    } else if (editbot != filebot) {
	edit_update(editbot);
	center_cursor();
    } else {
	while (current != filebot)
	    current = current->next;
	edit_update(current);
    }
    update_cursor();

}

int page_down(void)
{
    wrap_reset();
    current_x = 0;
    placewewant = 0;

    if (current == filebot)
	return 0;

    if (editbot != filebot) {
	current_y = 0;
	current = editbot;
    } else
	while (current != filebot) {
	    current = current->next;
	    current_y++;
	}

    edit_update_top(current);
    update_cursor();
    UNSET(KEEP_CUTBUFFER);
    check_statblank();
    return 1;
}

int do_home(void)
{
    current_x = 0;
    placewewant = 0;
    update_line(current, current_x);
    return 1;
}

int do_end(void)
{
    current_x = strlen(current->data);
    placewewant = xplustabs();
    update_line(current, current_x);

    return 1;
}

/* What happens when we want to go past the bottom of the buffer */
int do_down(void)
{
    wrap_reset();
    if (current->next != NULL) {
	update_line(current->prev, 0);

	if (placewewant > 0)
	    current_x = actual_x(current->next, placewewant);

	if (current_x > strlen(current->next->data))
	    current_x = strlen(current->next->data);
    } else {
	UNSET(KEEP_CUTBUFFER);
	check_statblank();
	return 0;
    }

    if (current_y < editwinrows - 1 && current != editbot)
	current_y++;
    else
	page_down_center();

    update_cursor();
    update_line(current->prev, 0);
    update_line(current, current_x);
    UNSET(KEEP_CUTBUFFER);
    check_statblank();
    return 1;
}

void page_up_center(void)
{
    if (edittop != fileage) {
	edit_update(edittop);
	center_cursor();
    } else
	current_y = 0;

    update_cursor();

}

int do_up(void)
{
    wrap_reset();
    if (current->prev != NULL) {
	update_line(current, 0);

	if (placewewant > 0)
	    current_x = actual_x(current->prev, placewewant);

	if (current_x > strlen(current->prev->data))
	    current_x = strlen(current->prev->data);
    }
    if (current_y > 0)
	current_y--;
    else
	page_up_center();

    update_cursor();
    update_line(current->next, 0);
    update_line(current, current_x);
    UNSET(KEEP_CUTBUFFER);
    check_statblank();
    return 1;
}

int do_right(void)
{
    if (current_x < strlen(current->data)) {
	current_x++;
    } else {
	if (do_down())
	    current_x = 0;
    }

    placewewant = xplustabs();
    update_cursor();
    update_line(current, current_x);
    UNSET(KEEP_CUTBUFFER);
    check_statblank();
    return 1;
}

int do_left(void)
{
    if (current_x > 0)
	current_x--;
    else if (current != fileage) {
	placewewant = 0;
	current_x = strlen(current->prev->data);
	do_up();
    }
    placewewant = xplustabs();

    update_cursor();
    update_line(current, current_x);
    UNSET(KEEP_CUTBUFFER);
    check_statblank();
    return 1;
}
