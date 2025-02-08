#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#include "menu_vezerlo.h"
#include "menu_helper_funcs.h"
#include "alakzatok.h"
#include "menu_structs.h"
#include "debugmalloc.h"


const MenuItem mainMenu[9] = {
    { "Mentés betöltése", load_proj },
    { "Projekt mentés", save_proj },
    { "Adatpanel", datapanel },
    { "Új alakzat felvétele", new_shape },
    { "Alakzat törlése", remove_shape },
    { "Munkalap törlése", clear_work_wrapper },
    { "Szerkesztés", szerkesztes },
    { "Kilépés", exit_ },
    { "SVG megnyitása most", open_svg_now }
};

const MenuItem calcOpt_circle[3] = {
    { "Eltolás valós vektorral", NULL },
    { "Metszéspont bármely más alakzattal", NULL },
    { "Nagyítás/kicsinyítés skalárral", NULL }
};

const MenuItem calcOpt_linesegm[4] = {
    { "Merőleges állítás", NULL },
    { "Eltolás valós vektorral", NULL },
    { "Metszéspont bármely más alakzattal", NULL },
    { "Párhuzamosság vizsgálata", NULL },
};

const ConsoleParams mainMenuParams = { "GeoMax Editor", 44, 7 };

const ConsoleParams saveMenuParams = { "GeoMax Editor - Projekt mentése", 80, 12 };

const ConsoleParams loadMenuParams = { "GeoMax Editor - Projekt betöltése", 80, 12 };

const ConsoleParams addShapeMenuParams = { "GeoMax Editor - Új alakzat felvétele", 80, 12 };

const ConsoleParams dataPanelMenuParams = { "GeoMax Editor - Adatpanel", 80, 13 };

const ConsoleParams removeShapeMenuParams = { "GeoMax Editor - Alakzat törlése", 80, 12 };

const ConsoleParams clearWorkMenuParams = { "GeoMax Editor - Munkalap törlése", 80, 12 };

const ConsoleParams operationMenuParams = { "GeoMax Editor - Szerkesztés", 80, 12 };

const ConsoleParams chooseOperationMenuParams = { "GeoMax Editor - Művelet választása", 80, 12 };

const ConsoleParams svgOpenMenuParams = { "GeoMax Editor - SVG megnyitása most", 80, 12 };

const ConsoleParams shapeSelector = { "GeoMax Editor - Alakzat kiválasztása", 80, 12 };


void clear_work_wrapper()
{
    clear_work(false);
}

void clear_buffer()
{
    while ((getchar()) != '\n' && getchar() != EOF);
    // Source: https://stackoverflow.com/questions/7898215/how-can-i-clear-an-input-buffer-in-c
}

void clear_console()
{
    if (_WIN32) { system("cls"); }
    else { system("clear"); }
    // Source: https://www.geeksforgeeks.org/clear-console-c-language/
}

void simplex_setctitle(char *title)
{
    if (_WIN32)
    {
        int wCharCount = MultiByteToWideChar(CP_UTF8, 0, title, -1, NULL, 0);
        wchar_t *wTitle = (wchar_t*) malloc(wCharCount * sizeof(wchar_t));
        if (wTitle != NULL)
        {
            MultiByteToWideChar(CP_UTF8, 0, title, -1, wTitle, wCharCount);
            SetConsoleTitleW(wTitle);
            free(wTitle);
        }
        // Source: https://stackoverflow.com/questions/6693010/how-do-i-use-multibytetowidechar, https://stackoverflow.com/questions/13219182/set-console-title-in-c-using-a-string
        else { SetConsoleTitle("GeoMax Editor - MEMORIAHIBA"); }
    }
    else { printf("\033]0;%s\007", title); fflush(stdout); }
}

void goto_sleep(int miliseconds)
{
    if (_WIN32) { Sleep((DWORD)miliseconds); }
    else
    {
        struct timespec timespec;
        timespec.tv_sec = (time_t)(miliseconds / 1000);
        timespec.tv_nsec = (long)((miliseconds % 1000) * 1000000);
        nanosleep(&timespec, NULL);
        // Source: https://stackoverflow.com/questions/7684359/how-to-use-nanosleep-in-c-what-are-tim-tv-sec-and-tim-tv-nsec
    }
}

void back_to_menu(int miliseconds)
{
    if (miliseconds <= 0) { miliseconds = 1000; }
    printf("Visszalépés a menübe...\n");
    goto_sleep(miliseconds);
}

int workspace_not_empty()
{
    if (worksheet_data_request(SHAPE_COUNT) == 0) { printf("A munkalap üres!\n"); return 0; }
    else { return 1; }
}

void print_dirback()
{
    printf("Visszalépés a menübe: K\n");
}

bool check_backsign(char *input)
{
    if (strcmp(input, "K") == 0 || strcmp(input, "k") == 0 || strcmp(input, "0") == 0)
    {
        back_to_menu(1000);
        return true;
    }
    else { return false; }
}

SHAPE_TYPE choose_shape()
{
    printf("Válassz egy alakzatot: C - kör, E - egyenes, S - szakasz\n");
    clear_buffer(); char input = getchar();
    
    if (input == 'C' || input == 'c') { return CIRCLE; }
    else if (input == 'E' || input == 'e') { return LINE; }
    else if (input == 'S' || input == 's') { return SEGMENT; }
    else { printf("Érvénytelen bemenet! Kiválasztva: kör"); goto_sleep(1000); return CIRCLE; }
}

void print_mem_error(bool sendBack)
{
    printf("Memóriafoglalási hiba!\n");
    if (sendBack) { back_to_menu(1000); }
}

int print_list_content(SHAPE_TYPE type)
{
    int selection = 0;
    int iter = 0;
    char buffer[100] = { '\0' };

    if (type == CIRCLE)
    {
        CircleListElement *current = get_shape_list()->firstCircle;
        MenuItem *circleDisp = (MenuItem*) malloc(worksheet_data_request(CIRCLE_COUNT) * sizeof(MenuItem));
        if (circleDisp == NULL) { print_mem_error(true); }

        Circle currentCircle;
        while (current != NULL)
        {
            currentCircle = current->circle;
            snprintf(buffer, 100, "Kör: O(%.4lf, %.4lf), r = %.4lf", currentCircle.origoX, currentCircle.origoY, currentCircle.radius);
            circleDisp[iter].name = strdup_override(buffer); if (circleDisp[iter].name == NULL) { circleDisp[iter].name = "MALLOC HIBA"; }
            circleDisp[iter].function = NULL;

            current = current->next; iter++;
        }

        selection = draw_menu(circleDisp, worksheet_data_request(CIRCLE_COUNT), &shapeSelector, 1);
        for (int i = 0; i < worksheet_data_request(CIRCLE_COUNT); i++) { free(circleDisp[i].name); }
        free(circleDisp);
    }
    else if (type == LINE)
    {
        LineListElement *current = get_shape_list()->firstLine;
        MenuItem *lineDisp = (MenuItem*) malloc(worksheet_data_request(LINE_COUNT) * sizeof(MenuItem));
        if (lineDisp == NULL) { print_mem_error(true); }

        Line currentLine;
        while (current != NULL)
        {
            currentLine = current->line;
            snprintf(buffer, 100, "Egyenes: P(%.4lf, %.4lf), m = %.4lf", currentLine.X0, currentLine.Y0, currentLine.slope);
            lineDisp[iter].name = strdup_override(buffer); if (lineDisp[iter].name == NULL) { lineDisp[iter].name = "MALLOC HIBA"; }
            lineDisp[iter].function = NULL;

            current = current->next; iter++;
        }

        selection = draw_menu(lineDisp, worksheet_data_request(LINE_COUNT), &shapeSelector, 1);
        for (int i = 0; i < worksheet_data_request(LINE_COUNT); i++) { free(lineDisp[i].name); }
        free(lineDisp);
    }
    else if (type == SEGMENT)
    {
        SegmentListElement *current = get_shape_list()->firstSegment;
        MenuItem *segmentDisp = (MenuItem*) malloc(worksheet_data_request(SEGMENT_COUNT) * sizeof(MenuItem));
        if (segmentDisp == NULL) { print_mem_error(true); }

        Segment currentSegment;
        while (current != NULL)
        {
            currentSegment = current->segment;
            snprintf(buffer, 100, "Szakasz: P1(%.4lf, %.4lf), P2(%.4lf, %.4lf)", currentSegment.X0, currentSegment.Y0, currentSegment.X1, currentSegment.Y1);
            segmentDisp[iter].name = strdup_override(buffer); if (segmentDisp[iter].name == NULL) { segmentDisp[iter].name = "MALLOC HIBA"; }
            segmentDisp[iter].function = NULL;

            current = current->next; iter++;
        }

        selection = draw_menu(segmentDisp, worksheet_data_request(SEGMENT_COUNT), &shapeSelector, 1);
        for (int i = 0; i < worksheet_data_request(SEGMENT_COUNT); i++) { free(segmentDisp[i].name); }
        free(segmentDisp);
    }

    return selection;
}

char *strdup_override(const char *source)
{
    char *newString = (char*) malloc(strlen(source) + 1);
    if (newString == NULL) { print_mem_error(true); return NULL; }
    strcpy(newString, source);
    return newString;
}