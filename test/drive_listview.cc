/**
 * Copyright (c) 2007-2012, Timothy Stack
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * * Neither the name of Timothy Stack nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "listview_curses.hh"

using namespace std;

static listview_curses lv;

class my_source : public list_data_source {
public:
    my_source() : ms_rows(2){};

    size_t listview_rows(const listview_curses& lv)
    {
        return this->ms_rows;
    };

    void listview_value_for_rows(const listview_curses& lv,
                                 vis_line_t row,
                                 vector<attr_line_t>& rows)
    {
        for (auto& value_out : rows) {
            if (row == 0) {
                value_out = "Hello";
            } else if (row == 1) {
                value_out = "World!";
            } else if (row < this->ms_rows) {
                char buffer[32];

                snprintf(buffer, sizeof(buffer), "%d", (int) row);
                value_out = string(buffer);
            } else {
                assert(0);
            }
            ++row;
        }
    };

    size_t listview_size_for_row(const listview_curses& lv, vis_line_t row)
    {
        return 100;
    };

    bool attrline_next_token(const view_curses& vc,
                             int line,
                             struct line_range& lr,
                             int& attrs_out)
    {
        return false;
    };

    int ms_rows;
};

int
main(int argc, char* argv[])
{
    int c, retval = EXIT_SUCCESS;
    bool wait_for_input = false, set_height = false;
    my_source ms;
    WINDOW* win;

    win = initscr();
    lv.set_data_source(&ms);
    lv.set_window(win);
    noecho();

    while ((c = getopt(argc, argv, "y:t:l:r:h:w")) != -1) {
        switch (c) {
            case 'y':
                lv.set_y(atoi(optarg));
                break;
            case 'h':
                lv.set_height(vis_line_t(atoi(optarg)));
                set_height = true;
                break;
            case 't':
                lv.set_top(vis_line_t(atoi(optarg)));
                break;
            case 'l':
                lv.set_left(atoi(optarg));
                break;
            case 'w':
                wait_for_input = true;
                break;
            case 'r':
                ms.ms_rows = atoi(optarg);
                break;
        }
    }

    if (!set_height) {
        unsigned long height, width;
        getmaxyx(win, height, width);
        lv.set_height(vis_line_t(height - lv.get_y()));
    }

    lv.do_update();
    refresh();
    if (wait_for_input) {
        getch();
    }
    endwin();

    return retval;
}
