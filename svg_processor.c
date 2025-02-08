#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "io_handler.h"
#include "svg_processor.h"
#include "menu_vezerlo.h"
#include "alakzatok.h"
#include "debugmalloc.h"

const char specMarkStart[9] = "<!--S-->";
const char specMarkEnd[9] = "<!--E-->";
const int viewportCompensation = 5;

int load_svg(char* _buffer)
{
    static bool hasRunOne = false;
    static int ownFileFound = 0;
    static int ownFileEnd = 0;
    static int ownFileStart = 0;

    char *input = strchr(_buffer, '\n'); if (input) { *input = '\0'; }

    if (!hasRunOne)
    {
        int x = 0, y = 0; if(sscanf(_buffer, "<svg width=\"%d\" height=\"%d\" xmlns=\"http://www.w3.org/2000/svg\" encoding=\"UTF-8\" version=\"1.1\">\n", &x, &y) == 2)
        {
            x -= viewportCompensation; y -= viewportCompensation;
            worksheet_data_override(x / 2, VIEWPORT_MAX);
            worksheet_data_override(y / 2, VIEWPORT_MAX);
        }
    }
    hasRunOne = true;

    if (ownFileFound) { ownFileStart = 1; }
    if (strcmp(_buffer, specMarkStart) == 0) { ownFileFound = 1; }
    else if (strcmp(_buffer, specMarkEnd) == 0) { ownFileEnd = 1; }
    
    if (!ownFileFound) { return 0; }

    if (ownFileStart && !ownFileEnd) { create_new_shape(_buffer); }
    memset(_buffer, EOF, SVG_LINE_LENGTH);

    return ownFileFound;
}

int save_svg(SVG_Content *content)
{
    char buffer1[SVG_CONTENT_LENGTH] = { '\0' };
    sprintf(buffer1,
        "<svg width=\"%d\" height=\"%d\" xmlns=\"http://www.w3.org/2000/svg\" encoding=\"UTF-8\" version=\"1.1\">\n",
        worksheet_data_request(VIEWPORT_MAX) * 2 + viewportCompensation, worksheet_data_request(VIEWPORT_MAX) * 2 + viewportCompensation
    );
    
    // Colors the coord lines red, so that it will not be parsed by the load_svg function
    Vector2 coordX1 = DescartesToTopLeft(vector2(-worksheet_data_request(VIEWPORT_MAX) - viewportCompensation, 0));
    Vector2 coordX2 = DescartesToTopLeft(vector2(worksheet_data_request(VIEWPORT_MAX) + viewportCompensation, 0));
    Vector2 coordY1 = DescartesToTopLeft(vector2(0, -worksheet_data_request(VIEWPORT_MAX) - viewportCompensation));
    Vector2 coordY2 = DescartesToTopLeft(vector2(0, worksheet_data_request(VIEWPORT_MAX) + viewportCompensation));
    char buffer2[SVG_CONTENT_LENGTH] = { '\0' };
    sprintf(buffer2, "<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"red\" stroke-width=\"1\" />\n", coordX1.x, coordX1.y, coordX2.x, coordX2.y);
    strcpy(content->coordLineX, buffer2);
    memset(buffer2, EOF, sizeof(buffer2));
    sprintf(buffer2, "<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"red\" stroke-width=\"1\" />\n", coordY1.x, coordY1.y, coordY2.x, coordY2.y);
    strcpy(content->coordLineY, buffer2);

    strcpy(content->baseData, buffer1);
    strcpy(content->endData, "</svg>");
    strcpy(content->startReadSign, specMarkStart);
    strcpy(content->endreadSign, specMarkEnd);

    return 0; // saving process will continue in io_handler.c
}