#ifndef MENU_HELPER_FUNCS_H
#define MENU_HELPER_FUNCS_H

#include "alakzatok.h"

// Used for the function pointers in the menu, hard coded param value call
void clear_work_wrapper();

// Clears the console input buffer, so that getchar() calls are handled more stable
void clear_buffer();

// Using system commands, clears the console
void clear_console();

// Sets the console title using UTF-8 encoding, cross-platform compatible, falls back if memory allocation fails
// Params: title to set
void simplex_setctitle(char *title);

// Makes the execution thread pause for a given time
// Params: time to pause in miliseconds
void goto_sleep(int miliseconds);

// Prints a message that the user is going to be redirected to the menu (via returns - callstack optimized method)
// Params: time to wait before redirecting the user to the main menu
void back_to_menu(int miliseconds);

// Checks if there are no shapes in the workspace
// Returns: 0 if there are no shapes, 1 if there are
int workspace_not_empty();

// Prints the key to press, that is associated with the back function
// Does not handle actual back function - that is : check_backsign();
void print_dirback();

// Checks if the input is back key (K) or is empty, then sends the user back to the main menu
// Params: input to check (oroginates from a scanf)
bool check_backsign(char *input);

// Prompts the user to choose a shape type, then returns the selected type
// Includes a back option and error handling as well
SHAPE_TYPE choose_shape();

// Prints a message, that a malloc was unsuccessful
// HOWEVER, DOES NOT HANDLE THE ERROR, AND DOES NOT RESETS ANY COUNTERS - USE WITH CAUTION!!!
// Params: sendBack - if true, sends the user back to the main menu after a given time
void print_mem_error(bool sendBack);

// Dynamically creates a MenuItem Struct, calls the draw_menu() function and returns the selected shape index
int print_list_content(SHAPE_TYPE type);

// Params: source string
// Reutrns: a dynamically allocated copy of the source string
// Usage: strdup does not work as it should (due to debugmalloc's implementetion), using this as a workaround
char *strdup_override(const char *source);

#endif