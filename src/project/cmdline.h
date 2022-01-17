#ifndef CMDLINE_H_INCLUDE
#define CMDLINE_H_INCLUDE

#include "microrl.h"

typedef enum cmdline_status_e {
    CMDLINE_OK,
    CMDLINE_ERR
} cmdline_status_t;

// Set up the command line
cmdline_status_t cmdline_init(microrl_t* readline);

// execute callback
int execute(int argc, const char* const* argv);

// completion callback
 char** complet(int argc, const char* const* argv);

// ctrl+c callback
void sigint(void);

// main task
void process_uart(void);

#endif
