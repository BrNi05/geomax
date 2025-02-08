#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
#elif
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

#include "io_handler.h"
#include "menu_vezerlo.h"
#include "menu_helper_funcs.h"
#include "svg_processor.h"
#include "debugmalloc.h"

const char ownFolder[13] = "geoMaxTemp\\";

int read_file(char *path)
{
    FILE *target = fopen(path, "r");
    if (target == NULL) { return 1; }
    
    const char *ftype = strchr(path, '.');
    if (ftype == NULL || strcmp(ftype, ".svg") != 0) { return 2; }

    char buffer[SVG_LINE_LENGTH] = { '\0' };
    int returnVal = 0;

    while ((fgets(buffer, sizeof(buffer), target))) { returnVal = load_svg(buffer); }

    fclose(target);

    return returnVal == 0 ? 3 : 0;
}

int save_file(char *path, char *filename)
{
    char* spath = create_savepath(path, filename);
    
    FILE *target_ = fopen(spath, "r");
    if (target_ != NULL) { free(spath); fclose(target_); return 2; }

    FILE *target = fopen(spath, "w"); free(spath);
    if (target == NULL) { fclose(target); return 1; }

    SVG_Content contentBuffer = { 0 }; save_svg(&contentBuffer);

    fprintf(target, "%s", contentBuffer.baseData);
    fprintf(target, "%s", contentBuffer.coordLineX);
    fprintf(target, "%s", contentBuffer.coordLineY);

    fprintf(target, contentBuffer.startReadSign); fprintf(target, "\n");

    CircleListElement *circleNode;
    for (circleNode = get_shape_list()->firstCircle; circleNode != NULL; circleNode = circleNode->next)
    {
        Vector2 coordSwitch = DescartesToTopLeft(vector2(circleNode->circle.origoX, circleNode->circle.origoY));
        fprintf(target, "<circle cx=\"%lf\" cy=\"%lf\" r=\"%lf\" stroke=\"blue\" stroke-width=\"1\" fill=\"none\" />\n", coordSwitch.x, coordSwitch.y, circleNode->circle.radius);
    }

    LineListElement *lineNode;
    for (lineNode = get_shape_list()->firstLine; lineNode != NULL; lineNode = lineNode->next)
    {
        LineTerminus lineOverride = line_terminus(lineNode->line);
        Vector2 coordSwitch1 = DescartesToTopLeft(vector2(lineOverride.start.x, lineOverride.start.y));
        Vector2 coordSwitch2 = DescartesToTopLeft(vector2(lineOverride.end.x, lineOverride.end.y));
        fprintf(target, "<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"purple\" stroke-width=\"1\" />\n", coordSwitch1.x, coordSwitch1.y, coordSwitch2.x, coordSwitch2.y);
    }

    SegmentListElement *segmentNode;
    for (segmentNode = get_shape_list()->firstSegment; segmentNode != NULL; segmentNode = segmentNode->next)
    {
        Vector2 coordSwitch1 = DescartesToTopLeft(vector2(segmentNode->segment.X0, segmentNode->segment.Y0));
        Vector2 coordSwitch2 = DescartesToTopLeft(vector2(segmentNode->segment.X1, segmentNode->segment.Y1));
        fprintf(target, "<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"green\" stroke-width=\"1\" />\n", coordSwitch1.x, coordSwitch1.y, coordSwitch2.x, coordSwitch2.y);
    }

    fprintf(target, contentBuffer.endreadSign); fprintf(target, "\n");
    fprintf(target, "%s", contentBuffer.endData);

    fclose(target);
    return 0;
}

int save_file_temp()
{
    char filename[PATH_LENGTH] = { '\0' };
    time_t now = time(NULL);
    snprintf(filename, sizeof(filename), "geoMaxTemp_%ld", (long)now);
    
    char tpath[PATH_LENGTH]; int error = tempfolder_path(tpath);
    if (filename[0] == '\0') { return 1; }
    if (tpath[0] == '\0') { return 1; }
    if (error) { return 4; }

    goto_sleep(1000);

    int result = save_file(tpath, filename); if (result != 0) { return result; }

    char buffer[PATH_LENGTH] = { '\0' };
    char *command = create_savepath(tpath, filename);
    if (_WIN32) { strcat(buffer, "start "); } else { strcat(buffer, "xdg-open "); } strcat(buffer, command);
    // Source: https://stackoverflow.com/questions/3037088/how-to-open-the-default-web-browser-in-windows-in-c
    system(command); free(command);

    return result;
}

int tempfolder_path(char *buffer)
{
    // Source: https://stackoverflow.com/questions/12302297/create-a-file-in-temp-using-c-program
    // Source: https://stackoverflow.com/questions/7430248/creating-a-new-directory-in-c
    
    #ifdef _WIN32
    {
        if (GetTempPathA((DWORD)PATH_LENGTH, buffer) == 0) { return 1; }
        else
        {
            strcat(buffer, ownFolder);
            _mkdir(buffer); return 0;
        }
    }
    #elif
    {
        char *tempFolderPath = getenv("TMPDIR");
        if (tempFolderPath == NULL) { tempFolderPath = getenv("TMP"); }
        if (tempFolderPath == NULL) { tempFolderPath = getenv("TEMP"); }
        if (tempFolderPath == NULL) { tempFolderPath = "/tmp"; }

        strcpy(buffer, tempFolderPath);
        strcat(buffer, ownFolder);
        mkdir(buffer, 0777); return 0;
    }
    #endif
}

void delete_temp_files()
{
    char command[PATH_LENGTH] = { '\0' }; char ownFolder_[PATH_LENGTH-20] = { '\0' }; tempfolder_path(ownFolder_);

    if (_WIN32) { snprintf(command, PATH_LENGTH, "rmdir /s /q %s", ownFolder_); }
    else { snprintf(command, PATH_LENGTH, "rm -rf %s", ownFolder_); }
    system(command);

    // Source: recoverit.wondershare.com/file-recovery/force-remove-directory-linux.html?, https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/rmdir
}

char *create_savepath(char *_path, char *_filename)
{
    char *savepath = (char*) malloc(strlen(_path) + strlen(_filename) + 5);
    strcpy(savepath, _path);
    strcat(savepath, _filename);
    strcat(savepath, ".svg");
    savepath[strlen(_path) + strlen(_filename) + 4] = '\0';

    return savepath;
}