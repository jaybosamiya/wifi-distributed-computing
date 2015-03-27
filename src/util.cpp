/* util.cpp */

#include "util.h"

#include <iostream>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <getopt.h>
#include <sys/wait.h>

using namespace std;

static int verbose_flag = 0;
static int debug_flag = 0;

const int BUFSIZE(200);
static char TEMPOUT[BUFSIZE];

extern char * interface;

int own_id;

// Parameter management

int handle_params(int argc, char ** argv) {
  int help_flag = false;

  // Make sure that only root can run this program
  if ( geteuid() ) {
    help_flag = true;
  }

  if ( argc == 1 ) {
    help_flag = true;
  }

  // Option parsing loop
  while (1) {
    int c;

    static struct option long_options[] =
      {
        {"machineid", required_argument, &own_id        ,  1},
        {"verbose"  , no_argument      , &verbose_flag  ,  1},
        {"debug"    , no_argument      , &debug_flag    ,  1},
        {"help"     , no_argument      , &help_flag     ,  1},
        {0, 0, 0, 0}
      };
    int option_index = 0;

    c = getopt_long_only (argc, argv, "",
                     long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c)
      {
      case 0:
        if ( long_options[option_index].flag == &own_id ) {
          own_id = atoi(optarg);
        }
        break;

      default:
        cerr << "Invalid option. Try " << argv[0] << " -h for help. Quitting.\n";
        return -1;
      }
  }

  if ( own_id <= 0 ) {
    own_id = generate_random(1,4294967295);
  }

  if (help_flag) {
    cerr << "Usage: " << argv[0] << " [options] interface\n"
            "  -v, --verbose   : Output more information\n"
            "  -d, --debug     : Show debugging information\n"
            "  -h, --help      : Show this help text\n"
            "\n"
            "Note: This program needs to be run as root\n"
    ;
    return -1;
  }

  while (optind < argc) {
    if ( ! interface ) {
      interface = strdup(argv[optind++]);
    } else {
      cerr << "Too many interfaces. Try " << argv[0] << " -h for help. Quitting.\n";
      return -1;
    }
  }

  if ( ! interface ) {
    cerr << "No interface specified. Try " << argv[0] << " -h for help. Quitting.\n";
    return -1;
  }

  return 0;
}

// Flag management

bool is_verbose() {
  return verbose_flag;
}

bool is_debug() {
  return debug_flag;
}

// Display error to stderr
void error(const char * fmt, ...) {
  va_list argp;

  va_start(argp, fmt);
  vsnprintf(TEMPOUT, sizeof(TEMPOUT), fmt, argp);
  va_end(argp);

  cerr << TEMPOUT << endl;
}

// Display message only when verbose
void verbose(const char * fmt, ...) {
  va_list argp;

  va_start(argp, fmt);
  vsnprintf(TEMPOUT, sizeof(TEMPOUT), fmt, argp);
  va_end(argp);

  if ( verbose_flag ) {
    cout << TEMPOUT << endl;
  }
}

// Display message only when debugging
void debug(const char * fmt, ...) {
  va_list argp;

  va_start(argp, fmt);
  vsnprintf(TEMPOUT, sizeof(TEMPOUT), fmt, argp);
  va_end(argp);

  if ( debug_flag ) {
    cout << TEMPOUT << endl;
  }
}

// Run a command-line utility
// Run using fork-exec combo
int run_command(char * const argv[]) {
  debug("Running program %s",argv[0]);

  pid_t p = fork();

  if ( p == 0 ) {
    if ( execvp(argv[0],argv) ) {
      error("Error during execution of %s",argv[0]);
      abort();
    }
  } else {
    int status;
    waitpid(p,&status,0);
    debug("%s returned value %d",argv[0],status);
    return status;
  }
}

// Timer utilities

float diff(timespec start, timespec end) {
  timespec temp;
  if ((end.tv_nsec-start.tv_nsec)<0) {
    temp.tv_sec = end.tv_sec-start.tv_sec-1;
    temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec-start.tv_sec;
    temp.tv_nsec = end.tv_nsec-start.tv_nsec;
  }
  return (float)temp.tv_sec + 1e-9 * temp.tv_nsec;
}

Timer::Timer() {
  reset();
}

void Timer::reset() {
 clock_gettime(CLOCK_MONOTONIC_COARSE,&start_time);
}

float Timer::get_time() {
  timespec temp;
  clock_gettime(CLOCK_MONOTONIC_COARSE,&temp);
  return diff(start_time,temp);
}

int generate_random(int l, int r) { // Generates a random integer in range [l,r]
  int x = random();
  x %= (r-l+1);
  x += l;
  return x;
}