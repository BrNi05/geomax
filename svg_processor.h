#ifndef SVG_PROCESSOR_H
#define SVG_PROCESSOR_H

// Max length of a line in SVG file
enum { SVG_CONTENT_LENGTH = 256 };

// A struct that stores important data to be loaded in SVG file
typedef struct SVG_Content
{
    char baseData[SVG_CONTENT_LENGTH];
    char coordLineX[SVG_CONTENT_LENGTH];
    char coordLineY[SVG_CONTENT_LENGTH];
    char endData[7];
    char startReadSign[9], endreadSign[9];
} SVG_Content;

// Receives a line from the SVG file and processes it, calls other functions to fill the lists
// Params: _buffer - line from the SVG file
// Returns: 1 if specMark is found, 0 if not
int load_svg(char* _buffer);

// Creates an SVG file by writing the data in the SVG_Content struct
// Params: content - struct that stores the data, originates from IO functions
// Returns: 0 if success, -1 if memory allocation failes
int save_svg(SVG_Content *content);

#endif