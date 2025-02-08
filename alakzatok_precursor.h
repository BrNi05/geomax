#ifndef ALKZATOK_PRECURSOR_H
#define ALKZATOK_PRECURSOR_H

// The main data structure that keeps track of the workspace
typedef struct WorkContainer
{
    int allShapesCount;
    int linesCount;
    int circlesCount;
    int segmentsCount;

    int viewportMax;
} WorkContainer;

// Used for functions that write or read data in the WorkContainer defined struct
typedef enum WORKSHEET_DATA_TYPE
{ 
    SHAPE_COUNT = 1,
    LINE_COUNT = 2,
    CIRCLE_COUNT = 3,
    SEGMENT_COUNT = 4,
    VIEWPORT_MAX = 5,
} WORKSHEET_DATA_TYPE;

// Holds two double values, used for coordinates
typedef struct Vector2
{
    double x;
    double y;
} Vector2;

// Holds the intersection points of two shapes (that can be 0, 1, 2, infinity)
typedef struct VectorX
{
    Vector2 intersections[2];
    int intersectionCount;
} VectorX;

// Holds the data of a circle
typedef struct Circle
{
    double origoX;
    double origoY;
    double radius;
} Circle;

// Holds the data of a circle, and a pointer to the next element - used for linked lists
typedef struct CircleListElement
{
    Circle circle;
    
    struct CircleListElement *next;
    struct CircleListElement *prev;
} CircleListElement;

// Holds the data of a line
typedef struct Line
{
    double X0;
    double Y0;
    double slope;
    Vector2 dirVector;
    Vector2 normVector;
    
} Line;

// Holds the data of a line, and a pointer to the next element - used for linked lists
typedef struct LineListElement
{
    Line line;
    
    struct LineListElement *next;
    struct LineListElement *prev;
} LineListElement;

// Holds the data of a segment
typedef struct Segment
{
    double X0;
    double Y0;
    double X1;
    double Y1;
    double length;
    Vector2 dirVector;
    Vector2 normVector;
} Segment;

// Holds the data of a segment, and a pointer to the next element - used for linked lists
typedef struct SegmentListElement
{
    Segment segment;
    
    struct SegmentListElement *next;
    struct SegmentListElement *prev;
} SegmentListElement;

// Holds the first elements of the linked lists, used for accessing the lists
typedef struct ListHolder
{
    CircleListElement *firstCircle;
    LineListElement *firstLine;
    SegmentListElement *firstSegment;
} ListHolder;

// Used for shape related write and read operations which need to know the shape type
typedef enum SHAPE_TYPE
{
    CIRCLE = 1,
    LINE = 2,
    SEGMENT = 3,
    UNDEFINED = 4
} SHAPE_TYPE;

// Defines the terminal points of a segment, that acts as a line (since .svg does not support lines)
typedef struct LineTerminus
{
    Vector2 start;
    Vector2 end;
} LineTerminus;

// UNION AND RELATED ------------------------------------------------------------------------------------------------

// Used for storing different shapes in a union
union ShapeUnion
{
    Circle circle;
    Line line;
    Segment segment;
};

// Used for functions that can work on more than one shape
typedef struct Shape
{
    SHAPE_TYPE type;
    union ShapeUnion shape;
} Shape;

#endif