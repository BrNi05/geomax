#ifndef MENU_VEZERLO_H
#define MENU_VEZERLO_H

// Menu entry struct - name and associated function
typedef struct MenuItem
{
    char *name;
    void (*function)();
} MenuItem;

// Console windows parameters - title, width, height
typedef struct ConsoleParams
{
    char title[100];
    int width;
    int height;
} ConsoleParams;

// Menu - new shape: input max length
enum { INPUT_LENGTH = 128 };

// Params: entry - list of menu items, entryNo - number of items, _consoleParams - console window parameters, mode - 1: return cursor pos, 0: func call
// Returns: NULL call: cursor position, non-NULL call: funtion call, reutrn value insignificant
int draw_menu(const MenuItem *entry, int entryNo, const ConsoleParams *_consoleParams, int mode);

// Params: __consoleParams - console window parameters
void set_console_dimensions(const ConsoleParams *__consoleParams);

// Complex: set console title with slide number
// Params: current slide, number of slides, console window parameters
// Simplex call: sets the "calculated" title (menu_helper_funcs.c)
void complex_setctitle(int _currentSlide, int _numOfSlides, const ConsoleParams *__consoleParams);

// Calculates the maximum number of entries that can be printed on a single console page
// Params: _entryPerPage - number of entries per page, _entryNo - number of all entries, _currentSlide - current slide
// Returns the minimum of two integers - reason: the last slide can be shorter / less entries than entryPerPage
int print_length(int _entryPerPage, int _entryNo, int _currentSlide);


// SUBMENU FUNCTIONS ------------------------------------------------------------------------------------------------

// Promts the user to enter a path, calls an IO function and based on the return value, prints a message
void load_proj();

// Promts the user to enter a path, calls an IO function and based on the return value, prints a message
void save_proj();

// Assists the user to select shape type and exact shape, handles errors and displays shape data
void datapanel();

// Promts the user to enter shape data, validates the input, calls a shape creation function and prints a message
void new_shape();

// Assists the user to select shape type and exact shape, handles errors and removes the selected shape
void remove_shape();

// Promts the user to confirm the deletion of all shapes, calls a function to delete all shapes and prints a message, handels errors as well
// Params: if true, the funtion will not redirect the user to the main menu, but will step back on callstack
void clear_work(bool returnControl);

// Assists the user to select shape type and exact shape, handles errors and calls a function to edit the selected shape
void szerkesztes();

// calls the built in exit() function
// And: frees up memory, deletes temporary files
void exit_();

// Opens the SVG file in the default browser while hanling errors
void open_svg_now();

#endif