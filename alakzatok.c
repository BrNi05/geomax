#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "alakzatok.h"
#include "menu_helper_funcs.h"
#include "debugmalloc.h"

WorkContainer worksheet = { 0, 0, 0, 0, 0 };
ListHolder *shapeListManager;

const double PI = 3.14159265359;
const double EQUAL_THRESHOLD = 0.0001;
const int INFINITE_SLOPE = 10000;
const int NO_INTERCEPT = -999999;

// ALAKZATOK.C - MAIN FUNCTIONS -------------------------------------------------------------------------------------

void shape_db_init()
{
    worksheet.allShapesCount = 0; 
    worksheet.linesCount = 0;
    worksheet.circlesCount = 0;
    worksheet.segmentsCount = 0;
    worksheet.viewportMax = 50; // default, minimum value

    shapeListManager = (ListHolder*) malloc(sizeof(ListHolder));
    shapeListManager->firstCircle = NULL;
    shapeListManager->firstLine = NULL;
    shapeListManager->firstSegment = NULL;
}

int worksheet_data_request(WORKSHEET_DATA_TYPE type)
{
    switch (type)
    {
        case SHAPE_COUNT:
            return worksheet.allShapesCount;
        case LINE_COUNT:
            return worksheet.linesCount;
        case CIRCLE_COUNT:
            return worksheet.circlesCount;
        case SEGMENT_COUNT:
            return worksheet.segmentsCount;
        case VIEWPORT_MAX:
            return worksheet.viewportMax;
        default:
            return 0; // lehetetlen eset
    }
}

void worksheet_data_override(int data, WORKSHEET_DATA_TYPE type)
{
    switch (type)
    {
        case SHAPE_COUNT:
            worksheet.allShapesCount = data;
            break;
        case LINE_COUNT:
            worksheet.linesCount = data;
            break;
        case CIRCLE_COUNT:
            worksheet.circlesCount = data;
            break;
        case SEGMENT_COUNT:
            worksheet.segmentsCount = data;
            break;
        case VIEWPORT_MAX:
            if (data < 50) { data = 50; }
            worksheet.viewportMax = data;
            break;
    }

    worksheet.allShapesCount = worksheet.linesCount + worksheet.circlesCount + worksheet.segmentsCount;
}

ListHolder *get_shape_list()
{
    return shapeListManager;
}

int create_new_shape(char input[INPUT_LENGTH])
{
    Circle circle; Segment segment; Line line; char type_buffer = '0'; double fallbackX = 0; double fallbackY = 0;
    
    // CIRCLE FILE READ FORMAT
    if (sscanf(input, "<circle cx=\"%lf\" cy=\"%lf\" r=\"%lf\" stroke=\"blue\" stroke-width=\"1\" fill=\"none\" />", &circle.origoX, &circle.origoY, &circle.radius) == 3)
    {
        Vector2 coordSwitch = TopLeftToDescartes(vector2(circle.origoX, circle.origoY));
        Circle newCircle = circle_calc(coordSwitch.x, coordSwitch.y, circle.radius);
        
        CircleListElement *newNode = create_circle_node(newCircle);
        add_circle_node_tolist(newNode);
        return 1;
    }

    // CIRCLE USER INPUT FORMAT
    else if (sscanf(input, "c-%lf:%lf-%lf", &circle.origoX, &circle.origoY, &circle.radius) == 3)
    {
        Circle newCircle = circle_calc(circle.origoX, circle.origoY, circle.radius);
        if (newCircle.radius != -1)
        {
            CircleListElement *newNode = create_circle_node(newCircle);
            add_circle_node_tolist(newNode);
            return 1;
        }
        else { return 0; }

        return 1;
    }

    // SEGMENT FILE READ FORMAT
    else if (!strstr(input, "purple") && sscanf(input, "<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"green\" stroke-width=\"1\" />", &segment.X0, &segment.Y0, &segment.X1, &segment.Y1) == 4)
    {
        Vector2 coordSwitch1 = TopLeftToDescartes(vector2(segment.X0, segment.Y0));
        Vector2 coordSwitch2 = TopLeftToDescartes(vector2(segment.X1, segment.Y1));
        Segment newSegment = segment_calc(coordSwitch1.x, coordSwitch1.y, coordSwitch2.x, coordSwitch2.y, 0, 0, 0, '0');
        SegmentListElement *newNode = create_segment_node(newSegment);
        add_segment_node_tolist(newNode);
        return 1;
    }

    // SEGMENT USER INPUT FORMAT TYPE 1
    else if (sscanf(input, "sz-%lf:%lf-%lf-%lf:%lf-%c", &segment.X0, &segment.Y0, &segment.length, &segment.dirVector.x, &segment.dirVector.y, &type_buffer) == 6)
    {
        Segment newSegment = segment_calc(segment.X0, segment.Y0, 0, 0, segment.length, segment.dirVector.x, segment.dirVector.y, type_buffer);
        if (!double_zero(newSegment.dirVector.x) || !double_zero(newSegment.dirVector.y))
        {
            SegmentListElement *newNode = create_segment_node(newSegment);
            add_segment_node_tolist(newNode);
            return 1;
        }
        else { return 0; }
    }

    // SEGMENT USER INPUT FORMAT TYPE 2
    else if (sscanf(input, "sz-%lf:%lf-%lf:%lf", &segment.X0, &segment.Y0, &segment.X1, &segment.Y1) == 4)
    {
        Segment newSegment = segment_calc(segment.X0, segment.Y0, segment.X1, segment.Y1, 0, 0, 0, '0');
        if (!double_zero(newSegment.dirVector.x) || !double_zero(newSegment.dirVector.y))
        {
            SegmentListElement *newNode = create_segment_node(newSegment);
            add_segment_node_tolist(newNode);
            return 1;
        }
        else { return 0; }
    }

    // LINE FILE READ FORMAT
    else if (!strstr(input, "green") && sscanf(input, "<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"purple\" stroke-width=\"1\" />", &line.X0, &line.Y0, &fallbackX, &fallbackY) == 4)
    {
        // Used only for file reading, since in .svg, the line is saved as a segment, but when used in program the x1, y1 coordinates are discarded
        // -1 multiplication since SVG Y-axis is inverted
        line.slope = -1 * (fallbackY - line.Y0) / (fallbackX - line.X0); // 0 division protection not needed, since a zero div save is not possible
        Vector2 coordSwitch = TopLeftToDescartes(vector2(line.X0, line.Y0));
        Line newLine = line_calc(coordSwitch.x, coordSwitch.y, line.slope, 0, 0, '0');
        LineListElement *newNode = create_line_node(newLine);
        add_line_node_tolist(newNode);
        return 1;
    }

    // LINE USER INPUT FORMAT TYPE 1
    else if (sscanf(input, "e-%lf:%lf-%lf:%lf-%c", &line.X0, &line.Y0, &line.dirVector.x, &line.dirVector.y, &type_buffer) == 5)
    {
        Line newLine = line_calc(line.X0, line.Y0, 0, line.dirVector.x, line.dirVector.y, type_buffer);
        if (!double_zero(newLine.dirVector.x) || !double_zero(newLine.dirVector.y))
        {
            LineListElement *newNode = create_line_node(newLine);
            add_line_node_tolist(newNode);
            return 1;
        }
        else { return 0; }

        return 1;
    }

    // LINE USER INPUT FORMAT TYPE 2
    else if (sscanf(input, "e-%lf:%lf-%lf", &line.X0, &line.Y0, &line.slope) == 3)
    {
        Line newLine = line_calc(line.X0, line.Y0, line.slope, 0, 0, '0');
        if (!double_zero(newLine.dirVector.x) || !double_zero(newLine.dirVector.y))
        {
            LineListElement *newNode = create_line_node(newLine);
            add_line_node_tolist(newNode);
            return 1;
        }
        else { return 0; }

        return 1;
    }

    return 0;
}

void delete_all_shapes()
{ 
    CircleListElement *iterator1 = shapeListManager->firstCircle;
    while (iterator1 != NULL)
    {
        CircleListElement *next = iterator1->next;
        free(iterator1);
        iterator1 = next;
    }
    shapeListManager->firstCircle = NULL;

    LineListElement *iterator2 = shapeListManager->firstLine;
    while (iterator2 != NULL)
    {
        LineListElement *next = iterator2->next;
        free(iterator2);
        iterator2 = next;
    }
    shapeListManager->firstLine = NULL;

    SegmentListElement *iterator3 = shapeListManager->firstSegment;
    while (iterator3 != NULL)
    {
        SegmentListElement *next = iterator3->next;
        free(iterator3);
        iterator3 = next;
    }
    shapeListManager->firstSegment = NULL;

    worksheet_data_override(0, LINE_COUNT);
    worksheet_data_override(0, CIRCLE_COUNT);
    worksheet_data_override(0, SEGMENT_COUNT);

    Shape nullShape; nullShape.type = UNDEFINED; resize_viewport(nullShape);
}

Shape find_shape(SHAPE_TYPE shapeTypeSelect, int shapeSelect)
{
    Shape selected;
    
    if (shapeTypeSelect == CIRCLE)
    {
        CircleListElement *current = get_shape_list()->firstCircle;
        for (int i = 0; i < shapeSelect; i++) { current = current->next; }
        selected.shape.circle = current->circle; selected.type = CIRCLE;
    }
    else if (shapeTypeSelect == SEGMENT)
    { 
        SegmentListElement *current = get_shape_list()->firstSegment;
        for (int i = 0; i < shapeSelect; i++) { current = current->next; }
        selected.shape.segment = current->segment; selected.type = SEGMENT;
    }
    else if (shapeTypeSelect == LINE)
    { 
        LineListElement *current = get_shape_list()->firstLine;
        for (int i = 0; i < shapeSelect; i++) { current = current->next; }
        selected.shape.line = current->line; selected.type = LINE;
    }

    return selected;
}

// ALAKZATOK.C - CIRCLE CREATION -------------------------------------------------------------------------------------

Circle circle_calc(double origoX, double origoY, double radius)
{
    Circle circle;
    
    if (check_existing_circle(circle)) { circle.radius = -1; return circle; }

    if (radius <= 0) // invalid inputs
    {
        circle.radius = -1;
    }
    else
    {
        circle.origoX = origoX;
        circle.origoY = origoY;
        circle.radius = radius;
    }
    return circle;
}

CircleListElement *create_circle_node(Circle circle)
{
    CircleListElement *newNode = (CircleListElement*) malloc(sizeof(CircleListElement));
    if (newNode == NULL) { print_mem_error(true); }
    else
    {
        newNode->circle = circle;
        newNode->next = NULL;
        newNode->prev = NULL;
    }

    return newNode;
}

void add_circle_node_tolist(CircleListElement *node)
{
    if (shapeListManager->firstCircle == NULL) { node->next = NULL; shapeListManager->firstCircle = node; }
    else
    {
        node->next = shapeListManager->firstCircle;
        shapeListManager->firstCircle->prev = node;
        node->prev = NULL;
        shapeListManager->firstCircle = node; // "LIFO"
    }

    worksheet_data_override(worksheet_data_request(CIRCLE_COUNT) + 1, CIRCLE_COUNT);
    Shape circleShape; circleShape.type = CIRCLE; circleShape.shape.circle = node->circle; resize_viewport(circleShape);
}

Circle return_circle_data(int index)
{
    CircleListElement *current = shapeListManager->firstCircle;
    for (int i = 0; i < index; i++) { current = current->next; }

    return current->circle;
}

double *return_circle_additional_data(Circle circle)
{
    double *data = (double*) malloc(3 * sizeof(double));
    if (data == NULL) { print_mem_error(true); return NULL; }

    data[0] = circle.radius * 2;
    data[1] = circle.radius * 2 * PI;
    data[2] = circle.radius * circle.radius * PI;

    return data;
}

// ALAKZATOK.C - SEGMENT CREATION ------------------------------------------------------------------------------------

Segment segment_calc(double X0, double Y0, double X1, double Y1, double length, double vectorX, double vectorY, char type)
{
    Segment segment;
    
    int saveMode = type == '0' ? 0 : 1; // 0: point and point, 1: point and vector

    bool isPoint = saveMode == 0 && (double_equal(X0, X1) && double_equal(Y0, Y1));
    bool lengthErr = saveMode == 1 && (length < 0 || double_zero(length));
    bool vectorErr = saveMode == 1 && (double_zero(vectorX) && double_zero(vectorY));
    bool typeErr = saveMode == 1 && (type != 'V' && type != 'N');

    if (isPoint || lengthErr || vectorErr || typeErr) // invalid inputs
    {
        segment.dirVector = vector2(0, 0); return segment;
    }
    else
    {
        Vector2 dirVector; Vector2 normVector;
        
        if (saveMode == 0)
        {
            length = sqrt(pow(X1 - X0, 2) + pow(Y1 - Y0, 2));
            dirVector = vector2((X1 - X0) / length, (Y1 - Y0) / length);
            normVector = vector2(-dirVector.y, dirVector.x);
        }
        else if (saveMode == 1)
        {
            if (type == 'V') { dirVector = vector2(vectorX, vectorY); normVector = vector2(-vectorY, vectorX); }
            else { normVector = vector2(vectorX, vectorY); dirVector = vector2(-vectorY, vectorX); }
            if (double_zero(dirVector.x)) { dirVector.x = EQUAL_THRESHOLD; } // 0 division protection
            
            double piece = sqrt(pow(length, 2) / (pow(dirVector.x, 2) + pow(dirVector.y, 2)));
            if (double_zero(dirVector.x)) { Y1 = Y0 + length; X1 = X0 + EQUAL_THRESHOLD; }
            else if (double_zero(dirVector.y)) { X1 = X0 + length; Y1 = Y0 + EQUAL_THRESHOLD; }
            else
            {
                bool growing = dirVector.x * dirVector .y > 0;
                X1 = X0 + fabs(dirVector.x) * piece;
                Y1 = Y0 + fabs(dirVector.y) * piece * (double)(growing ? 1 : -1);
            }
        }
        
        segment.X0 = X0;
        segment.Y0 = Y0;
        segment.X1 = X1;
        segment.Y1 = Y1;
        segment.length = length;
        segment.dirVector = dirVector;
        segment.normVector = normVector;

        if (check_existing_segment(segment)) { segment.dirVector = vector2(0, 0); return segment; }
        else { return segment; }
    }
}

SegmentListElement *create_segment_node(Segment segment)
{
    SegmentListElement *newNode = (SegmentListElement*) malloc(sizeof(SegmentListElement));
    if (newNode == NULL) { print_mem_error(true); return NULL;}
    else
    {
        newNode->segment = segment;
        newNode->next = NULL;
        newNode->prev = NULL;
    }

    return newNode;
}

void add_segment_node_tolist(SegmentListElement *node)
{
    if (shapeListManager->firstSegment == NULL) { node->next = NULL; shapeListManager->firstSegment = node; }
    else
    {
        node->next = shapeListManager->firstSegment;
        shapeListManager->firstSegment->prev = node;
        node->prev = NULL;
        shapeListManager->firstSegment = node; // "LIFO"
    }

    worksheet_data_override(worksheet_data_request(SEGMENT_COUNT) + 1, SEGMENT_COUNT);
    Shape segmentShape; segmentShape.type = SEGMENT; segmentShape.shape.segment = node->segment; resize_viewport(segmentShape);
}

Segment return_segment_data(int index)
{
    SegmentListElement *current = shapeListManager->firstSegment;
    for (int i = 0; i < index; i++) { current = current->next; }

    return current->segment;
}

double *return_segment_additional_data(Segment segment)
{
    double *data = (double*) malloc(3 * sizeof(double));
    if (data == NULL) { print_mem_error(true); return NULL; }

    data[0] = check_monotonity(vector2(segment.X0, segment.Y0), vector2(segment.X1, segment.Y1));
    data[1] = xy_intersect(NULL, &segment, 0, data[0]);
    data[2] = xy_intersect(NULL, &segment, 1, data[0]);

    return data;
}

// ALAKZATOK.C - LINE CREATION ------------------------------------------------------------------------------------------

Line line_calc(double X0, double Y0, double slope, double vectorX, double vectorY, char type)
{
    Line line;

    int saveMode = type == '0' ? 0 : 1; // 0: point and slope, 1: point and vector

    if ((saveMode == 1 && (double_zero(vectorX) && double_zero(vectorY))) || (saveMode == 1 && (type != 'V' && type != 'N')))
    {
        line.dirVector = vector2(0, 0); return line;
    }
    else
    {
        Vector2 dirVector; Vector2 normVector;

        if (saveMode == 0) // 
        {
            dirVector = vector2(1, slope);
            normVector = vector2(-dirVector.y, dirVector.x);
        }
        else
        {
            if (type == 'V') { dirVector = vector2(vectorX, vectorY); normVector = vector2(-vectorY, vectorX); }
            else { normVector = vector2(vectorX, vectorY); dirVector = vector2(-vectorY, vectorX); }
            if (double_zero(dirVector.x)) { dirVector.x = EQUAL_THRESHOLD; } // 0 division protection
            slope = dirVector.y / dirVector.x;
        }
        
        line.X0 = X0;
        line.Y0 = Y0;
        line.slope = slope;
        line.dirVector = dirVector;
        line.normVector = normVector;

        if (check_existing_line(line)) { line.dirVector = vector2(0, 0); return line; }
        else { return line; }
    }
}

LineListElement *create_line_node(Line line)
{
    LineListElement *newNode = (LineListElement*) malloc(sizeof(LineListElement));
    if (newNode == NULL) { print_mem_error(true); }
    else
    {
        newNode->line = line;
        newNode->next = NULL;
        newNode->prev = NULL;
    }

    return newNode;
}

void add_line_node_tolist(LineListElement *node)
{
    if (shapeListManager->firstLine == NULL) { node->next = NULL; shapeListManager->firstLine = node; }
    else
    {
        node->next = shapeListManager->firstLine;
        shapeListManager->firstLine->prev = node;
        node->prev = NULL;
        shapeListManager->firstLine = node; // "LIFO"
    }

    worksheet_data_override(worksheet_data_request(LINE_COUNT) + 1, LINE_COUNT);
    Shape lineShape; lineShape.type = LINE; lineShape.shape.line = node->line; resize_viewport(lineShape);
}

Line return_line_data(int index)
{
    LineListElement *current = shapeListManager->firstLine;
    for (int i = 0; i < index; i++) { current = current->next; }

    return current->line;
}

double *return_line_additional_data(Line line)
{
    double *data = (double*) malloc(3 * sizeof(double));
    if (data == NULL) { print_mem_error(true); return NULL; }

    data[0] = check_monotonity(vector2(line.X0, line.Y0), gen_point(line));
    data[1] = xy_intersect(&line, NULL, 0, data[0]);
    data[2] = xy_intersect(&line, NULL, 1, data[0]);

    return data;
}