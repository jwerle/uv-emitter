
#ifndef __GETCH_H__
#define __GETCH_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

int
getch ();

static void
on_signal (int signal);

static struct termios _term;

static int signals[] = {
  SIGABRT,
  SIGINT,
  SIGKILL,
  SIGQUIT,
  SIGSTOP,
  SIGTERM
};

int
getch () {
  int c = EOF;
  int i = 0;
  int len = sizeof(signals) / sizeof(signals[0]);
  struct termios term, ttmp;

  // get stdin attributes
  tcgetattr(STDIN_FILENO, &term);

  // get stdin attributes and store in static `_term variable for
  // a global copy
  tcgetattr(STDIN_FILENO, &_term);

  // store state of stdin
  ttmp = term;

  // canonical input and echo modes
  ttmp.c_lflag &= ~(ICANON|ECHO);

  // set stdin attributes on tmp termio struct now with `TCSANOW`
  tcsetattr(STDIN_FILENO, TCSANOW, &ttmp);

  // bind signal handles
  for (; i < len; ++i) {
    signal(signals[i], on_signal);
    signal(signals[i], on_signal);
    signal(signals[i], on_signal);
    signal(signals[i], on_signal);
    signal(signals[i], on_signal);
    signal(signals[i], on_signal);
  }

  // get char from modified stdin
  c = getchar();

  // reset stdin to previous state
  tcsetattr(STDIN_FILENO, TCSANOW, &term);

  return c;
}


static void
on_signal (int signal) {
  tcsetattr(STDIN_FILENO, TCSANOW, &_term);
  exit(1);
}


#endif
