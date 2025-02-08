#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#include "menu_vezerlo.h"
#include "menu_structs.h"
#include "alakzatok.h"
#include "io_handler.h"
#include "menu_helper_funcs.h"
#include "debugmalloc.h"

// MAIN PROGRAM -------------------------------------------------------------------------------------------------

int main(void)
{
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
    #elif defined(__APPLE__) && defined(__MACH__)
        perror("Unsupported platform."); exit(EXIT_FAILURE);
    #endif

    delete_temp_files();
    shape_db_init();

    while (1 == 1) { draw_menu(mainMenu, 9, &mainMenuParams, 0); }

    return 0;
}

int draw_menu(const MenuItem *entry, int entryNo, const ConsoleParams *_consoleParams, int mode)
{
    int cursor = 0;
    int numOfSlides = 1; int currentSlide = 1; // count from 1
    int entryPerPage = 0;
    bool selected = false;

    if (entry == NULL) { entry = mainMenu; entryNo = 9; _consoleParams = &mainMenuParams; }

    set_console_dimensions(_consoleParams);
    
    entryPerPage = _consoleParams->height - 2;
    numOfSlides = entryNo / entryPerPage + 1;

    while(!selected)
    {
        complex_setctitle(currentSlide, numOfSlides, _consoleParams);
        
        for (int i = 0; i < print_length(entryPerPage, entryNo, currentSlide); i++)
        {
            if (i == cursor) { printf("> "); }
            else { printf("  "); }
            printf("  %s\n", entry[i + (currentSlide - 1) * entryPerPage].name);
        }

        int input = getchar();

        switch (input)
        {

            // W, w (up)
            case 87:
            case 119:
                if (cursor > 0) { cursor--; }
                else { cursor = print_length(entryPerPage, entryNo, currentSlide) - 1; }
            break;

            // S, s (down)
            case 83:
            case 115:
                if (cursor < print_length(entryPerPage, entryNo, currentSlide) - 1) { cursor++; }
                else { cursor = 0; }
            break;

            // A, s (previous slide)
            case 65:
            case 97:
                if (currentSlide > 1) { currentSlide--; }
                else { currentSlide = numOfSlides; }
                cursor = 0;
            break;

            // D, d (next slide)
            case 68:
            case 100:
                if (currentSlide < numOfSlides) { currentSlide++; }
                else { currentSlide = 1; }
                cursor = 0;
            break;

            // E, e (select)
            case 69:
            case 101:
                selected = true;
            break;
        
            default:
                break;
        }

        clear_console();
    }

    if (mode) { return cursor + (currentSlide - 1) * entryPerPage; }
    else { entry[cursor + (currentSlide - 1) * entryPerPage].function(); return 0; }
}

void set_console_dimensions(const ConsoleParams *__consoleParams)
{
    clear_console();

    if (_WIN32)
    {
        char command[40] = { '\0' }; // snprintf buffer
        snprintf(command, sizeof(command), "mode con: cols=%d lines=%d", __consoleParams->width, __consoleParams->height);
        system(command);
    }
    // Source: https://stackoverflow.com/questions/8688846/resize-command-prompt-through-commands

    else { printf("\e[8;%d;%dt", __consoleParams->height, __consoleParams->width); }
}

void complex_setctitle(int _currentSlide, int _numOfSlides, const ConsoleParams *__consoleParams)
{
    char buffer[30] = { '\0' };
    char titleCopy[80] = { '\0' }; strcpy(titleCopy, __consoleParams->title);

    memset(buffer, EOF, sizeof(buffer)); snprintf(buffer, sizeof(buffer), " - %d / %d. oldal", _currentSlide, _numOfSlides);
    simplex_setctitle(strcat(titleCopy, buffer));
}

int print_length(int _entryPerPage, int _entryNo, int _currentSlide)
{
    return min(_entryPerPage, _entryNo - (_currentSlide - 1) * _entryPerPage);
}


// SUBMENU FUNCTIONS -------------------------------------------------------------------------------------------

void load_proj()
{
    char path[PATH_LENGTH] = { '\0' };
    
    simplex_setctitle("GeoMax Editor - Projekt betöltése");
    set_console_dimensions(&loadMenuParams);

    print_dirback();
    printf("\nAdj meg egy Windows/Linux sztenderd elérési utat! \n");
    printf("Az elérési út és a fájlnév NEM tartalmazhat SPACE vagy speciális karaktereket!\n");
    printf("Pld.: C:\\Users\\User\\Desktop\\save.svg\n");
    printf("\nForrás: ");
    
    if (scanf("%s", path)) { if(check_backsign(path)) { return; }; }
    goto_sleep(1000);
    printf("\nA beolvasott útvonal: %s\n", path);

    printf("A mentett projekt betöltése kitörli a jelenlegi munkalapot.\n"); clear_work(true);

    switch (read_file(path))
    {
        case 0:
            printf("Sikeres beolvasás, támogatott fájl! Megnyitás...\n");
            printf("Adatok betöltve! Visszairányítás a menübe!\n");
            break;
        case 1:
            printf("Nem található a file, megnyitási hiba!\n");
            break;
        case 2:
            printf("Sikeres beolvasás, nem támogatott formátum!\n");
            break;
        case 3:
            printf("Sikeres beolvasás, támogatott formátum, mentési hiba!\n");
            break;
    }

    goto_sleep(1500);
}

void save_proj()
{
    if (workspace_not_empty())
    {
        char path[PATH_LENGTH] = { '\0' };
        char filename[PATH_LENGTH] = { '\0' };
    
        simplex_setctitle("GeoMax Editor - Projekt mentése");
        set_console_dimensions(&saveMenuParams);

        print_dirback();
        printf("\nAdj meg egy Windows/Linux sztenderd mappa elérési utat!\n");
        printf("Az elérési út és a fájlnév NEM tartalmazhat SPACE vagy speciális karaktereket!\n");
        printf("\nElérési út: "); scanf("%s", path);
        if (check_backsign(path)) { return; }
        printf("Fájlnév: "); clear_buffer(); scanf("%s", filename);
    
        goto_sleep(1000);

        switch (save_file(path, filename))
        {
            case 0:
                printf("\nSikeres mentés!\n");
                break;
            case 1:
                printf("\nSikertelen mentés! Fájkezelési hiba!\n");
                break;
            case 2:
                printf("\nSikertelen mentés! A fájl már létezik!\n");
                break;
            case 3:
                printf("\nSikertelen mentés! Memóriahiba!\n");
                break;
        }
    
        goto_sleep(1000);
    }

    else { printf("Visszairányítás a menübe..."); goto_sleep(1000); }
}

void datapanel()
{
    simplex_setctitle("GeoMax Editor - Adatpanel");
    set_console_dimensions(&dataPanelMenuParams);

    printf("Viewport mérete: %d x %d\n\n", worksheet_data_request(VIEWPORT_MAX), worksheet_data_request(VIEWPORT_MAX));

    if (workspace_not_empty())
    {
        SHAPE_TYPE shapeTypeSelect = choose_shape(); // UNDEFINED return - back to menu
        if (worksheet_data_request(CIRCLE_COUNT) == 0 && shapeTypeSelect == CIRCLE) { printf("\nNincs kör a munkalapon!\n"); back_to_menu(1000); return; }
        else if (worksheet_data_request(SEGMENT_COUNT) == 0 && shapeTypeSelect == SEGMENT) { printf("\nNincs szakasz a munkalapon!\n"); back_to_menu(1000); return; }
        else if (worksheet_data_request(LINE_COUNT) == 0 && shapeTypeSelect == LINE) { printf("\nNincs egyenes a munkalapon!\n"); back_to_menu(1000); return; }
        
        int shapeSelect = print_list_content(shapeTypeSelect);

        if (shapeTypeSelect == CIRCLE)
        {
            Circle target = return_circle_data(shapeSelect);
            double *circleAdditionalData = return_circle_additional_data(target); if (circleAdditionalData == NULL) {  }
            printf("Kör\n\nKözéppont: %.4lf, %.4lf,\nSugár: %.4lf\nÁtmérő: %.4lf\nKerület: %.4lf\nTerület: %.4lf\n", target.origoX, target.origoY, target.radius, circleAdditionalData[0], circleAdditionalData[1], circleAdditionalData[2]);
            free(circleAdditionalData);
        }

        else if (shapeTypeSelect == SEGMENT)
        {
            Segment target = return_segment_data(shapeSelect); Shape targetShape; targetShape.type = SEGMENT; targetShape.shape.segment = target;
            double *segmentAdditionalData = return_segment_additional_data(target); if (segmentAdditionalData == NULL) { print_mem_error(true); return; }
            char monotonic = ((int)segmentAdditionalData[0] == 0 ? '-' : ((int)segmentAdditionalData[0] > 0) ? ((int)segmentAdditionalData[0] >= 10000 ? '|' : 'N') : 'C');
            double tm = segmentAdditionalData[1] == NO_INTERCEPT ? INFINITY : point_is_on_segment(targetShape, vector2(0, segmentAdditionalData[1])) ? segmentAdditionalData[1] : INFINITY;;
            double zh = segmentAdditionalData[2] == NO_INTERCEPT ? INFINITY : point_is_on_segment(targetShape, vector2(segmentAdditionalData[2], 0)) ? segmentAdditionalData[2] : INFINITY;;
            printf("Szakasz\n\nKezdőpont: %.4lf, %.4lf\nVégpont: %.4lf, %.4lf\nHossz: %.4lf\nIrányvektor: %.4lf, %.4lf\nNormálvektor: %.4lf, %.4lf\nMonotonitás: %c\nTengelymetszet: %.4lf\nZérushely: %.4lf\n", target.X0, target.Y0, target.X1, target.Y1, target.length, target.dirVector.x, target.dirVector.y, target.normVector.x, target.normVector.y, monotonic, tm, zh);
            free(segmentAdditionalData);
        }

        else if (shapeTypeSelect == LINE)
        {
            Line target = return_line_data(shapeSelect);
            double *lineAdditionalData = return_line_additional_data(target); if (lineAdditionalData == NULL) { print_mem_error(true); return; }
            char monotonic = ((int)lineAdditionalData[0] == 0 ? '-' : ((int)lineAdditionalData[0] > 0) ? (double_equal(target.dirVector.x, 0) ? '|' : 'N') : 'C');
            printf("Egyenes\n\nKiindulópont: %.4lf, %.4lf\nMeredekség: %.4lf\nIrányvektor: %.4lf, %.4lf\nNormálvektor: %.4lf, %.4lf\nMonotonitás: %c\nTengelymetszet: %.4lf\nZérushely: %.4lf\n", target.X0, target.Y0, double_equal(10000, target.slope) ? INFINITY : target.slope, target.dirVector.x, target.dirVector.y, target.normVector.x, target.normVector.y, monotonic, lineAdditionalData[1] == NO_INTERCEPT || fabs(lineAdditionalData[1] >= 100000) ? INFINITY : lineAdditionalData[1], lineAdditionalData[2] == NO_INTERCEPT ? INFINITY : lineAdditionalData[2]);
            free(lineAdditionalData);
        }

        clear_buffer(); printf("\n"); print_dirback(); getchar(); back_to_menu(1000);
    }
    else { back_to_menu(1000); }
}

void new_shape()
{    
    simplex_setctitle("GeoMax Editor - Új alakzat felvétele");
    set_console_dimensions(&addShapeMenuParams);

    print_dirback();
    printf("Add meg a formátum szerint az alakzat paramétereit:\n\n");
    printf("Kör: c-[X0:Y0]-[sugár]\n");
    printf("Szakasz: sz-[X0:Y0]-[X1:Y1] vagy sz-[X0:Y0]-[HOSSZ]-[X0:Y0 vektor]-[V/N]\n");
    printf("Egyenes: e-[X0:Y0]-[MEREDEKSÉG] vagy e-[X0:Y0]-[X0:Y0 vektor]-[V/N]\n");

    printf("\n\nBemenet: ");
    char input[INPUT_LENGTH] = { '\0' }; scanf("%s", input);

    if (check_backsign(input)) { return; }
    if (create_new_shape(input))
    {
        printf("\nSikeres felvétel!\n");
        back_to_menu(1000);
    }
    else
    {
        printf("\nHibás bemenet!\n");
        back_to_menu(1000);
    }
}

void remove_shape()
{
    simplex_setctitle("GeoMax Editor - Alakzat törlése");
    set_console_dimensions(&removeShapeMenuParams);

    if (workspace_not_empty())
    {
        SHAPE_TYPE shapeTypeSelect = choose_shape();
        WORKSHEET_DATA_TYPE shapeCount = (shapeTypeSelect == CIRCLE ? CIRCLE_COUNT : (shapeTypeSelect == SEGMENT ? SEGMENT_COUNT : LINE_COUNT));
        if (worksheet_data_request(shapeCount) == 0) { printf("\nNincs ilyen alakzat a munkalapon!\n"); back_to_menu(1000); return; }
        int shapeSelect = print_list_content(shapeTypeSelect);
        delete_shape(shapeTypeSelect, shapeSelect);
        printf("Sikeres törlés!\n");
    }
    
    printf("Visszairányítás a menübe..."); back_to_menu(1000);
}

void clear_work(bool returnControl)
{
    simplex_setctitle("GeoMax Editor - Munkalap törlése");
    set_console_dimensions(&clearWorkMenuParams);

    if (workspace_not_empty())
    {
        print_dirback();
        printf("\nAz összes alakzat törlése.. Y/N?\n");
        clear_buffer();
        char input = getchar();
        if (check_backsign(&input)) { return; }

        if (input == 'Y' || input == 'y')
        {
            printf("\nTörlés folyamatban...\n");
            delete_all_shapes();
            printf("\nAz összes alakzat törölve!\n");
        }
        else if (input == 'N' || input == 'n') { printf("\nMűvelet megszakítva!\n"); }
        else { printf("\nÉrvénytelen bemenet!\n"); back_to_menu(1000); return; }
    }

    if (!returnControl) { back_to_menu(1000); return; }
    else { clear_console(); return; }
}

void szerkesztes()
{
    simplex_setctitle("GeoMax Editor - Szerkesztés");
    set_console_dimensions(&operationMenuParams);

    if (workspace_not_empty())
    {
        SHAPE_TYPE shapeTypeSelect = choose_shape();
        if (worksheet_data_request(CIRCLE_COUNT) == 0 && shapeTypeSelect == CIRCLE) { printf("\nNincs kör a munkalapon!\n"); back_to_menu(1000); return; }
        else if (worksheet_data_request(SEGMENT_COUNT) == 0 && shapeTypeSelect == SEGMENT) { printf("\nNincs szakasz a munkalapon!\n"); back_to_menu(1000); return; }
        else if (worksheet_data_request(LINE_COUNT) == 0 && shapeTypeSelect == LINE) { printf("\nNincs egyenes a munkalapon!\n"); back_to_menu(1000); return; }
        int shapeSelect = print_list_content(shapeTypeSelect);
        int operation = draw_menu((shapeTypeSelect == CIRCLE ? calcOpt_circle : calcOpt_linesegm), (shapeTypeSelect == CIRCLE ? 3 : 4), &chooseOperationMenuParams, 1);

        Shape selected = find_shape(shapeTypeSelect, shapeSelect); selected.type = shapeTypeSelect;
        shape_operation(selected, operation);
    }

    back_to_menu(1000);
}

void exit_()
{
    delete_all_shapes();
    free(get_shape_list());
    delete_temp_files();
    debugmalloc_log_file("memdump.txt");
    exit(EXIT_SUCCESS);
}

void open_svg_now()
{
    simplex_setctitle("GeoMax Editor - SVG megnyitása most");
    set_console_dimensions(&svgOpenMenuParams);

    if (!workspace_not_empty()) { printf("Nincs mit kirajzolni!\n"); }
    else
    {
        switch (save_file_temp())
        {
            case 0:
                printf("Siker! Megnyitás...\n");
                break;
            case 1:
            case 2:
                printf("A megnyitás sikertelen!\n");
                break;
            case 3:
                printf("Sikertelen mentés! Memóriahiba!\n");
                break;
            case 4:
                printf("Sikertelen mentés! A temp mappa nem elérhető!\n");
                break;
        }
    }

    back_to_menu(1000);
}