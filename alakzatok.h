#ifndef ALAKZATOK_H
#define ALAKZATOK_H

#include "alakzatok_precursor.h"
#include "menu_vezerlo.h"

// ALAKZATOK.C - MAIN FUNCTIONS ---------------------------------------------------------------------------------------

extern const double PI; // 3.14159265359
extern const double EQUAL_THRESHOLD; // 0.0001
extern const int INFINITE_SLOPE; // 10000
extern const int NO_INTERCEPT; // -999999

// Sets "database" values to initial, allocates memory
void shape_db_init();

// Return: data about the worksheet using an enum as selector
int worksheet_data_request(WORKSHEET_DATA_TYPE type);

// Params: data to be set, type of the data
// Should be used with caution! - but has built in logic to set connected data accordingly
void worksheet_data_override(int data, WORKSHEET_DATA_TYPE type);

// Returns: a pointer to the shapeListManager struct, that holds all the shapes
ListHolder *get_shape_list();

// Params: input from the user (that is presumably a shape "code")
// Returns: 1 if the shape was created, 0 if not (invalid input)
int create_new_shape(char input[INPUT_LENGTH]);

// Deletes all shapes and performs every operation to "reset" the workspace
void delete_all_shapes();

// Params: type of shape that is selected, index of the shape in the linked list
// Returns the shape itself (line, circle, segment)
Shape find_shape(SHAPE_TYPE shapeTypeSelect, int shapeSelect);

// ALAKZATOK.C - CIRCLE CREATION -------------------------------------------------------------------------------------

// Params: data that is needed to create a circle
// Returns: a circle with the given data
Circle circle_calc(double origoX, double origoY, double radius);

// Params: a circle struct
// Returns: a new circle linked list node with the given circle data
CircleListElement *create_circle_node(Circle circle);

// Params: a circle linked list node
void add_circle_node_tolist(CircleListElement *node);

// Params: index of the circle in the linked list (0: firstCircle)
// Returns: the circle data at the given index
Circle return_circle_data(int index);

// Params: a circle
// Returns: additional data about the circle (diameter, circumference, area)
double *return_circle_additional_data(Circle circle);

// ALAKZATOK.C - SEGMENT CREATION ------------------------------------------------------------------------------------

// Params: data that is needed to create a segment (multiple ways to define a segment)
// Returns: a segment with the given data
Segment segment_calc(double X0, double Y0, double X1, double Y1, double length, double vectorX, double vectorY, char type);

// Params: a segment struct
SegmentListElement *create_segment_node(Segment segment);

// Params: a segment linked list node
void add_segment_node_tolist(SegmentListElement *node);

// Params: index of the segment in the linked list (0: firstSegment)
// Returns: the segment data at the given index
Segment return_segment_data(int index);

// Params: a segment
// Returns: additional data about the segment (monotonity, intersection with the y-axis, zerus point)
double *return_segment_additional_data(Segment segment);

// ALAKZATOK.C - LINE CREATION ----------------------------------------------------------------------------------------

// Params: data that is needed to create a line (multiple ways to define a line)
// Returns: a line with the given data
Line line_calc(double X0, double Y0, double slope, double vectorX, double vectorY, char type);

// Params: a line struct
// Returns: a new line linked list node with the given line data
LineListElement *create_line_node(Line line);

// Params: a line linked list node
void add_line_node_tolist(LineListElement *node);

// Params: index of the line in the linked list (0: firstLine)
// Returns: the line data at the given index
Line return_line_data(int index);

// Params: a line
// Returns: additional data about the line (monotonity, intersection with the y-axis, zerus point)
double *return_line_additional_data(Line line);

// ALAKZATOK_HELPER.C - ALAKZATOK.C helper functions ------------------------------------------------------------------

// Params: two points of a segment or line
// Returns: 1 - increasing, -1 - decreasing, 0 - constant (or basically 0 slope), 9999 - vertical line (infinite slope)
int check_monotonity(Vector2 point1, Vector2 point2);

// Params: future x and y coordinates of the vector
// Returns: a vector with the given coordinates
Vector2 vector2(double x, double y);

// Params: a line, on which the point is needs to be generated
// Returns: a point on the line, that differs from x0 and y0
Vector2 gen_point(Line line);

// Params: type of the shape to be deleted, index of the shape (in the linked list)
void delete_shape(SHAPE_TYPE type, int index);

// Params: two double type numbers
// Returns: true if the two numbers are basically equal, false if not
bool double_equal(double a, double b);

// Params: a double type number
// Returns: true if the number is basically 0, false if not
bool double_zero(double a);

// Params: a new circle
// Returns: true if the circle is already in the linked list, false if not
bool check_existing_circle(Circle circle);

// Params: a new line
// Returns: true if the line is already in the linked list, false if not
bool check_existing_line(Line line);

// Params: a new segment
// Returns: true if the segment is already in the linked list, false if not
bool check_existing_segment(Segment segment);

// ALAKZATOK_HELPER.C - SHAPE OPERATIONS AND REALTED FUNCTIONS --------------------------------------------------------

// Params: shape itself from a linked list (that carries the type as well), operation to be performed
void shape_operation(Shape shape, int operation);

// Prints a message and handles user input
// Used for determining if the result of an operation should form a new shape or not
bool modify_original_promt();

// Returns: a Vector2 given by the user
// Requests the user to input two coordinates.
Vector2 request_vector();

// Returns: a scalar given by the user
// Requests the user to input a scalar value.
double request_scalar();

// Params: a shape, that should be a line or a segment, and a point where the perpendicular line should be drawn
// Returns: the perpendicular line (not segment!) to the given shape
Shape perpendicular(Shape linear, Vector2 where, double length);

// Params: a shape, and an offset vector
// Returns: the shape's coordinates (effectively new shape) offset by the given vector
Shape offset_byvector(Shape shape, Vector2 vector);

// Params: two shapes, that are to be intersected
// Returns: the intersection point(s) of the two shapes (with a special vector)
VectorX intersections(Shape shape1, Shape shape2);

// A VectorX that holds the intersection points of two shapes (that can be 0, 1, 2, infinity)
// Prints the intersection points to the console
void intersect_helper(VectorX intersectionList);

// Params: a line or segment in the form of a shape
// Returns: the b constant of the line (y = ax + b)
double return_b(Shape linear);

// Params: a segment and a point, in which two segments can intersect
// Returns: true if the point is on the segment, false if not
bool point_is_on_segment(Shape segment, Vector2 point);

// Params: two shapes (linears), that are to be checked
// Returns: true if the two shapes are parallel, false if not
bool is_parallel(Shape shape1, Shape shape2);

// Params: a circle, a scalar used for scaling
// Returns: the circle scaled by a scalar (returned as a shape for further processing)
Shape scale(Circle circle, double scalar);

// Params: line and segment (on of which is NULL), mode that determines the x or y intersection calculation (0: y, 1: x), monotonity of the line (calculated by caller)
// Returns: the intersection point with x or y axis (only the non 0 value)
// The return value is -999999 if the there is no intersection
double xy_intersect(Line *line, Segment *segment, int mode, int monotonity);

// Params: two circles, that are to be intersected
// Returns: the intersection point(s) of the two circles
// Can be 1 or 2 points
VectorX twopoint_circle(Circle circle1, Circle circle2);

// Params: the original, unmodified shape, the new, calculated shape, justModify: a boolean that determines if a shape should be created or just modify the original
// Used the same functions as user input or file reading, but with a calculated shape
void process_new_shape(Shape originalShape, Shape newShapeElement, bool justModify);

// ALAKZATOK_HELPER.C - VIEWPORT OPERATIONS -----------------------------------------------------------------------------------

// Params: a shape, that is to be checked
// Checks if the new shape extendes beyond the future SVG viewport, and resizes accordingly
void resize_viewport(Shape shape);

// Params: a point in Descartes coordinate system
// Returns: the same point in the coordinate system used by SVGS
Vector2 DescartesToTopLeft(Vector2 input);

// Params: a point in SVGs coordinate system
// Returns: the same point in the Descartes coordinate system
Vector2 TopLeftToDescartes(Vector2 input);

// Params: a line
// Returns: the start and end points of the line
// To be used for SVG saving, since a line is defined by two points
LineTerminus line_terminus(Line line);

#endif