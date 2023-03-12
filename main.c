#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>

#include "main.h"
#include "codes.h"
#include "arg.h"

long get_time_ms()
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC_RAW, &now);
	return now.tv_sec * 1000 + now.tv_nsec / 1000000;
}

void print_str_center(char *str, int col_offset)
{
	int max_y, max_x;
	getmaxyx(stdscr, max_y, max_x);

	mvprintw(max_y / 2 + col_offset, max_x / 2 - strlen(str) / 2, str);
}

/*
 * run_timer runs until no more time is left in the pomo.
 */
void run_timer(long pomo_n,
		long pomo_len_ms,
		long break_len_ms)
{
	// ncurses initialization
	initscr();
	keypad(stdscr, TRUE);
	timeout(-1);
	noecho();

	long start_ms = get_time_ms(); 
	long pomo_i = 1;
	bool is_break = false;

	// When starting a timer (break or work):
	//  - Store ms of start
	//  - Elapse 100ms each tick
	//  - If now > start + timer_length then flip timer -> prompt to continue
	//  - Update display (timer type, and time remaining mm:ss)
	while (pomo_i <= pomo_n)
	{
		long timer_len_ms = is_break ? break_len_ms : pomo_len_ms;
		long remaining_ms = start_ms + timer_len_ms - get_time_ms();

		// 100ms
		usleep(100000);

		move(0, 0);
		clrtobot();

		char *timer_name = is_break ? BREAK_TXT : POMO_TXT;

		if (remaining_ms <= 0)
		{
			// Prompt to start next timer.
			char restart_txt[64];
			sprintf(restart_txt, "%s timer's up! Start next timer? [Y/n]", timer_name);
			print_str_center(restart_txt, 0);
			
			char prompt_answer = getch();
			if (prompt_answer == 'n' || prompt_answer == 'N')
			{
				goto finish;
			}

			if (is_break)
			{
				pomo_i++;
			}
			is_break = !is_break;
			start_ms = get_time_ms();
			continue;
		}

		// Show timer status
		char title[32];
		sprintf(title, "%s %lu/%lu", timer_name, pomo_i, pomo_n);
		print_str_center(title, 0);

		long remaining_mn = remaining_ms / 60 / 1000 % 60;
		long remaining_s = remaining_ms / 1000 % 60;

		char remaining_txt[6];
		sprintf(remaining_txt, "%02lu:%02lu", remaining_mn, remaining_s);	
		print_str_center(remaining_txt, 1);
		refresh();
	}

finish:
	endwin();
}

int main(int argc, char **argv)
{
	// First argument: Pomo count (default: 4)
	// Second argument: Pomo length (default: 25 minutes)
	// Third argument: Break length	(default: 5 minutes)
	long pomo_n = argc > 1 ? argtol(argv[1]) : POMO_N;
	long pomo_len_ms = argc > 2 ? argtol(argv[2]) * 60 * 1000 : POMO_LEN_MS;
	long break_len_ms = argc > 3 ? argtol(argv[3]) * 60 * 1000 : BREAK_LEN_MS;

	run_timer(pomo_n, pomo_len_ms, break_len_ms);

	return SUCCESS;
}
