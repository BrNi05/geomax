#ifndef MENU_STRUCTS
#define MENU_STRUCTS

#include "menu_vezerlo.h"

// MenuItem struct for the main menu
extern const MenuItem mainMenu[9];
// MenuItem struct for the circle related operations
extern const MenuItem calcOpt_circle[3];

// MenuItem struct for the line and segment related operations
extern const MenuItem calcOpt_linesegm[4];

// ConsoleParams struct for the main menu
extern const ConsoleParams mainMenuParams;

// ConsoleParams struct for the save menu
extern const ConsoleParams saveMenuParams;

// ConsoleParams struct for the load menu
extern const ConsoleParams loadMenuParams;

// ConsoleParams struct for the add shape menu
extern const ConsoleParams addShapeMenuParams;

// ConsoleParams struct for the data panel menu
extern const ConsoleParams dataPanelMenuParams;

// ConsoleParams struct for the remove shape menu
extern const ConsoleParams removeShapeMenuParams;

// ConsoleParams struct for the clear workspace menu
extern const ConsoleParams clearWorkMenuParams;

// ConsoleParams struct for the operation menu
extern const ConsoleParams operationMenuParams;

// ConsoleParams struct for the choose operation menu (inside choose operation menu)
extern const ConsoleParams chooseOperationMenuParams;

// ConsoleParams struct for the SVG open window
extern const ConsoleParams svgOpenMenuParams;

extern const ConsoleParams shapeSelector;

#endif