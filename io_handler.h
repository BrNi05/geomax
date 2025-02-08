#ifndef IO_HANDLER_H
#define IO_HANDLER_H

// Max length of a file/dir path
enum { PATH_LENGTH = 256 };

// Max length of a line in file
enum { SVG_LINE_LENGTH = 256 };

// Params: path to a specific file
// Return value: indicates success or failure with error code
int read_file(char *path);

// Params: path to  the desired dir and a filename
// Return value: indicates success or failure with error code
int save_file(char *path, char *filename);

// Saves the current workspace to the %temp% folder of the user
// Return value: indicates success or failure with error code
int save_file_temp();

// Writes the path of the %temp% or equivalent folder to the buffer
// Params: buffer to write the path to (non null, PATH_LENGTH)
// Return value: indicates success or failure with error code
int tempfolder_path(char *buffer);

// Deletes the temporary files created by the program from the %temp% or equivalent folder
void delete_temp_files();

// Helper function - strcat path and filename with extension
// Params: path to the desired dir and a filename
// Return value: the full path to the file
// Using malloc, return value SHOULD BE FREED
char* create_savepath(char *_path, char *_filename);

#endif