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
	noecho();

	long start_ms = get_time_ms(); 
	long pomo_i = 0;
	bool is_break = false;

	// When starting a timer (break or work):
	//  - Store ms of start
	//  - Elapse 100ms each tick
	//  - If now > start + timer_length then flip timer -> prompt to continue
	//  - Update display (timer type, and time remaining mm:ss)
	while (pomo_i < pomo_n)
	{
		long timer_len_ms = is_break ? break_len_ms : pomo_len_ms;
		long remaining_ms = start_ms + timer_len_ms - get_time_ms();

		int max_y, max_x;
		getmaxyx(stdscr, max_y, max_x);

		// 100ms
		usleep(100000);

		move(0, 0);
		clrtobot();

		if (remaining_ms <= 0)
		{
			// Prompt to start next timer.
			mvprintw(max_y / 2, max_x / 2 - 17, "Timer's up! Start next timer? [Y/n]");
			char prompt_answer = getch();
			if (prompt_answer == 'n' || prompt_answer == 'N')
			{
				goto finish;
			}

			pomo_i++;
			is_break = !is_break;
			start_ms = get_time_ms();
		}

		// Show timer status
		char *timer_name = is_break ? BREAK_TXT : POMO_TXT;

		char title[64];
		sprintf(title, "%s %lu/%lu", timer_name, pomo_i, pomo_n);
		mvprintw(max_y / 2, max_x / 2 - strlen(title) / 2, title);

		long remaining_mn = remaining_ms / 60 / 1000 % 60;
		long remaining_s = remaining_ms / 1000 % 60;
		mvprintw(max_y / 2 + 1, max_x / 2 - 2, "%02lu:%02lu", remaining_mn, remaining_s);
		refresh();
	}

finish:
	endwin();
}

int main(int argc, char **argv)
{
	// No arguments, use default settings (4x25)
	long pomo_n = POMO_N;
	long pomo_len_ms = POMO_LEN_MS;
	long break_len_ms = BREAK_LEN_MS;

	// First argument: Pomo count
	// Second argument: Pomo length (optional)
	// Third argument: Break length (optional)	
	if (argc > 1)
	{
		pomo_n = argtol(argv[1]);
	}
	if (argc > 2)
	{
		pomo_len_ms = argtol(argv[2]) * 60 * 1000;
	}
	if (argc > 3)
	{
		break_len_ms = argtol(argv[3]) * 60 * 1000;
	}

	run_timer(pomo_n, pomo_len_ms, break_len_ms);

	return SUCCESS;
}
