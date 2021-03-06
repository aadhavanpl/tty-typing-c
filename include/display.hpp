#pragma once

#include "word_generator.hpp"

#include <curses.h>
#include <iostream>
#include <chrono>

class display {
private:
    int m_total_time;
    int m_remaining_time;
    chrono::time_point<chrono::steady_clock> m_start_time;
    vector<string> m_rand_words;
    uint m_num_words;
    WINDOW* m_main_window;

    void set_initial_time(int total_time_sec) {
        this -> m_total_time = this -> m_remaining_time = total_time_sec;
        this -> m_start_time = chrono::steady_clock::now();
    }

    bool has_timer_ended() {
        return (this -> m_remaining_time) <= 0;
    }

    void calculate_remaining_time() {
        chrono::time_point<chrono::steady_clock> current_time = chrono::steady_clock::now();
        chrono::duration<float> elapsed_time = this -> m_start_time - current_time;
        int elapsed_seconds = abs((int)elapsed_time.count());
        this -> m_remaining_time = m_total_time - elapsed_seconds;
    }

    void show_time_remaining(WINDOW* window) {
        calculate_remaining_time();
        int ymax, xmax;
        getmaxyx(window, ymax, xmax);
        int remaining_mins = (this -> m_remaining_time) / 60;
        int remaining_secs = (this -> m_remaining_time) % 60;
        mvwprintw(window, 0, xmax-11, "Time: %02i:%02i", remaining_mins, remaining_secs);
    }

    void show_all_words(WINDOW* window, int y, int x) {
        mvprintw(y,x,"");
		uint max_words_per_line = 6;
        for(int i = 0; i < this -> m_num_words / max_words_per_line; ++i) {
            for(int j = 0; j < max_words_per_line; ++j) {
                printw("%s ", (this -> m_rand_words[max_words_per_line*i + j]).c_str());
            } printw("\n");
        }
    }

public:
    void start(int argc, char** argv) {
        word_generator wg;
        wg.initialize(argc, argv);
        set_initial_time(60);
        this -> m_rand_words = wg.generate_random_words();
        this -> m_num_words = wg.get_num_words();
        this -> m_main_window = initscr();
        noecho();
        curs_set(0);
        char user_inp;
        bool has_printed_all_words = false;
        do {
            if(!has_printed_all_words) {
                show_all_words(this -> m_main_window, 0,0);
                has_printed_all_words = true;
            }
            show_time_remaining(this -> m_main_window);
            refresh();
        } while(!has_timer_ended());
        endwin();
    }
};
