#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "alakzatok.h"
#include "menu_helper_funcs.h"
#include "debugmalloc.h"

// ALAKZATOK_HELPER.C - ALAKZATOK.C helper functions ------------------------------------------------------------------

int check_monotonity(Vector2 point1, Vector2 point2)
{
    if (double_equal(point1.y, point2.y)) { return 0; }
    if (double_equal(point1.x, point2.x)) { return INFINITE_SLOPE; }
    Vector2 xSmallPoint = point1.x < point2.x ? point1 : point2;
    Vector2 xBigPoint = point1.x > point2.x ? point1 : point2;
    
    if (xSmallPoint.y < xBigPoint.y) { return 1; }
    else if (xSmallPoint.y > xBigPoint.y) { return -1; }
    else { return 0; }
}

Vector2 vector2(double x, double y)
{
    Vector2 vector; vector.x = x; vector.y = y;
    return vector;
}

Vector2 gen_point(Line line)
{
    Vector2 point = { line.X0 + 1, line.Y0 + line.slope };
    return point;
}

void delete_shape(SHAPE_TYPE type, int index)
{   
    if (type == CIRCLE)
    {
        CircleListElement *iterator1 = get_shape_list()->firstCircle;

        if (index == 0) { get_shape_list()->firstCircle = iterator1->next; free(iterator1); }
        else
        {
            for (int i = 0; i < index; i++) { iterator1 = iterator1->next; }
            iterator1->prev->next = iterator1->next;
            if (index == worksheet_data_request(CIRCLE_COUNT) - 1) { iterator1->next = NULL; } else { iterator1->next->prev = iterator1->prev; }
            free(iterator1);
        }

        worksheet_data_override(worksheet_data_request(CIRCLE_COUNT) - 1, CIRCLE_COUNT);
    }

    if (type == LINE)
    {
        LineListElement *iterator2 = get_shape_list()->firstLine;
        
        if (index == 0) { get_shape_list()->firstLine = iterator2->next; free(iterator2); }
        else
        {
            for (int i = 0; i < index; i++) { iterator2 = iterator2->next; }
            iterator2->prev->next = iterator2->next;
            if (index == worksheet_data_request(LINE_COUNT) - 1) { iterator2->next = NULL; } else { iterator2->next->prev = iterator2->prev; }
            free(iterator2);
        }

        worksheet_data_override(worksheet_data_request(LINE_COUNT) - 1, LINE_COUNT);
    }

    if (type == SEGMENT)
    {
        SegmentListElement *iterator3 = get_shape_list()->firstSegment;
        
        if (index == 0) { get_shape_list()->firstSegment = iterator3->next; free(iterator3); }
        else
        {
            for (int i = 0; i < index; i++) { iterator3 = iterator3->next; }
            iterator3->prev->next = iterator3->next;
            if (index == worksheet_data_request(SEGMENT_COUNT) - 1) { iterator3->next = NULL; } else { iterator3->next->prev = iterator3->prev; }
            free(iterator3);
        }

        worksheet_data_override(worksheet_data_request(SEGMENT_COUNT) - 1, SEGMENT_COUNT);
    }

    Shape nullShape; nullShape.type = UNDEFINED; resize_viewport(nullShape);
}

bool double_equal(double a, double b)
{
    return fabs(a - b) <= EQUAL_THRESHOLD;
}

bool double_zero(double a)
{
    return fabs(a) <= EQUAL_THRESHOLD;
}

bool check_existing_circle(Circle circle)
{
    CircleListElement *next;
    for(next = get_shape_list()->firstCircle; next != NULL; next = next->next)
    {
        if (double_equal(next->circle.origoX, circle.origoX) && double_equal(next->circle.origoY, circle.origoY) && double_equal(next->circle.radius, circle.radius))
        {
            return true;
        }
    }

    return false;
}

bool check_existing_line(Line line)
{
    LineListElement *next;
    for(next = get_shape_list()->firstLine; next != NULL; next = next->next)
    {
        if (double_equal(next->line.X0, line.X0) && double_equal(next->line.Y0, line.Y0) && double_equal(next->line.slope, line.slope))
        {
            return true;
        }
    }

    return false;
}

bool check_existing_segment(Segment segment)
{
    SegmentListElement *next;
    for(next = get_shape_list()->firstSegment; next != NULL; next = next->next)
    {
        if (double_equal(next->segment.X0, segment.X0) && double_equal(next->segment.Y0, segment.Y0) && double_equal(next->segment.X1, segment.X1) && double_equal(next->segment.Y1, segment.Y1))
        {
            return true;
        }
    }

    return false;
}

// ALAKZATOK_HELPER.C - SHAPE OPERATIONS AND REALTED FUNCTIONS ---------------------------------------------------------

void shape_operation(Shape shape, int operation)
{   
    if (shape.type == CIRCLE)
    {
        if (operation == 0)
        {
            simplex_setctitle("GeoMax Editor - Kör eltolása");
            Vector2 vector = request_vector();
            bool isNewShape = modify_original_promt();
            Shape newCircle = offset_byvector(shape, vector);
            process_new_shape(shape, newCircle, isNewShape);
        }
        else if (operation == 1)
        {
            simplex_setctitle("GeoMax Editor - Metszéspontok keresése");
            SHAPE_TYPE comparingShapeType = choose_shape();
            if (worksheet_data_request(comparingShapeType == CIRCLE ? CIRCLE_COUNT : comparingShapeType == LINE ? LINE_COUNT : SEGMENT_COUNT) == 0) { printf("Nincs ilyen alakzat a munkalapon!\n"); back_to_menu(1000); return; }
            int comparingShapeIndex = print_list_content(comparingShapeType);
            Shape comparingShape = find_shape(comparingShapeType, comparingShapeIndex);
            VectorX intersectionList = intersections(shape, comparingShape);
            intersect_helper(intersectionList);
        }
        else if (operation == 2)
        {
            simplex_setctitle("GeoMax Editor - Kör nagyítása/kicsinyítése");
            double scalar = request_scalar();
            bool isNewShape = modify_original_promt();
            Shape newCircle = scale(shape.shape.circle, scalar);
            process_new_shape(shape, newCircle, isNewShape);
        }
    }
    
    if (shape.type == LINE || shape.type == SEGMENT)
    {
        if (operation == 0)
        {
            simplex_setctitle("GeoMax Editor - Merőleges állítása");
            printf("A merőleges állítás pontja:\n"); Vector2 targetPoint = request_vector();
            double length; if (shape.type == SEGMENT) { printf("\nMerőleges hossza:"); length = request_scalar(); }
            bool isNewShape = modify_original_promt();
            Shape newLine = perpendicular(shape, targetPoint, length); if (newLine.type == UNDEFINED) { printf("A művelet nem hajtható végre!\n"); return; }
            process_new_shape(shape, newLine, isNewShape);
        }
        else if (operation == 1)
        {
            simplex_setctitle("GeoMax Editor - Eltolás vektorral");
            Vector2 offsetVector = request_vector();
            bool isNewShape = modify_original_promt();
            Shape newShape = offset_byvector(shape, offsetVector);
            process_new_shape(shape, newShape, isNewShape);
        }
        else if (operation == 2)
        {
            simplex_setctitle("GeoMax Editor - Metszéspontok keresése");
            SHAPE_TYPE comparingShapeType = choose_shape();
            if (worksheet_data_request(comparingShapeType == CIRCLE ? CIRCLE_COUNT : comparingShapeType == LINE ? LINE_COUNT : SEGMENT_COUNT) == 0) { printf("Nincs ilyen alakzat a munkalapon!\n"); back_to_menu(1000); return; }
            int comparingShapeIndex = print_list_content(comparingShapeType);
            Shape comparingShape = find_shape(comparingShapeType, comparingShapeIndex);
            VectorX intersectionList = intersections(shape, comparingShape);
            intersect_helper(intersectionList);
        }
        else if (operation == 3)
        {
            simplex_setctitle("GeoMax Editor - Párhuzamosság vizsgálata");
            SHAPE_TYPE comparingShapeType = choose_shape(); if (comparingShapeType == CIRCLE) { printf("A kör nem párhuzamosítható egyenesekkel vagy szakaszokkal!\n"); return; }
            if (worksheet_data_request(comparingShapeType == CIRCLE ? CIRCLE_COUNT : comparingShapeType == LINE ? LINE_COUNT : SEGMENT_COUNT) == 0) { printf("Nincs ilyen alakzat a munkalapon!\n"); back_to_menu(1000); return; }
            int comparingShapeIndex = print_list_content(comparingShapeType);
            Shape comparingShape = find_shape(comparingShapeType, comparingShapeIndex);
            bool isParallel = is_parallel(shape, comparingShape);
            if (isParallel) { printf("A két alakzat párhuzamos!\n"); } else { printf("A két alakzat nem párhuzamos!\n"); }
            clear_buffer(); print_dirback(); getchar(); return;
        }
    }
}

bool modify_original_promt()
{
    clear_buffer();
    printf("A szerkesztés írja felül az eredeti alakzatot? I/N\n");
    char input = getchar();
    if (input == 'I' || input == 'i') { return true; }
    else { return false; }
}

Vector2 request_vector()
{
    Vector2 vector;
    printf("Kérem adja meg a vektort!\n");
    printf("Vektor bemenet (X:Y formátumban): ");
    int results = scanf("%lf:%lf", &vector.x, &vector.y); if (results != 2) { printf("Hibás bemenet! Érték: (0, 0)\nNyomj ENTERT!\n"); goto_sleep(1000); return vector2(0, 0); }
    return vector;
}

double request_scalar()
{
    double scalar;
    printf("Kérem adja meg a skalárt!\n");
    printf("Skalár bemenet: ");
    if (scanf("%lf", &scalar) == 1) { if (double_zero(scalar)) { printf("A skalár nem lehet 0!\n"); request_scalar(); } }
    else { printf("Hibás bemenet! Érték: 1\nNyomj ENTERT!\n"); goto_sleep(1000); return 1; }
    return scalar;
}

Shape perpendicular(Shape linear, Vector2 where, double length)
{
    Shape newShape;

    if (linear.type == LINE)
    {
        Line line = linear.shape.line;
        Line newLine = line_calc(where.x, where.y, 0, line.normVector.x, line.normVector.y, 'V');
        newShape.shape.line = newLine; newShape.type = LINE;
    }
    else if (linear.type == SEGMENT)
    {
        Segment segment = linear.shape.segment;
        double slope = double_equal(segment.X0, segment.X1) ? INFINITE_SLOPE : -1 * (segment.Y1 - segment.Y0) / (segment.X1 - segment.X0);
        double x1 = sin(slope) * length + segment.X0; double y1 = cos(slope) * length + segment.Y0;
        Segment newSegment = segment_calc(segment.X0, segment.Y0, x1, y1, 0, 0, 0, '0');
        newShape.shape.segment = newSegment; newShape.type = SEGMENT;
    }
    else { newShape.type = UNDEFINED; }

    return newShape;
}

Shape offset_byvector(Shape shape, Vector2 vector)
{
    if (shape.type == CIRCLE)
    {
        Circle circle = shape.shape.circle;
        circle.origoX += vector.x;
        circle.origoY += vector.y;
        shape.shape.circle = circle; shape.type = CIRCLE;
    }
    else if (shape.type == LINE)
    {
        Line line = shape.shape.line;
        line.X0 += vector.x;
        line.Y0 += vector.y;
        shape.shape.line = line; shape.type = LINE;
    }
    else if (shape.type == SEGMENT)
    {
        Segment segment = shape.shape.segment;
        segment.X0 += vector.x;
        segment.Y0 += vector.y;
        segment.X1 += vector.x;
        segment.Y1 += vector.y;
        shape.shape.segment = segment; shape.type = SEGMENT;
    }
    return shape;
}

VectorX intersections(Shape shape1, Shape shape2) // 0, 1, 2
{
    VectorX intersectionList;

    if (shape1.type == CIRCLE && shape2.type == CIRCLE)
    {
        Circle circle1 = shape1.shape.circle; Circle circle2 = shape2.shape.circle;
        double distX = circle2.origoX - circle1.origoX; double distY = circle2.origoY - circle1.origoY;
        double dist = sqrt(pow(distX, 2) + pow(distY, 2));

        // Same origo, far away, concentric
        if (double_zero(dist) || dist > circle1.radius + circle2.radius || dist < fabs(circle1.radius - circle2.radius))
        {
            intersectionList.intersectionCount = 0;
            if (shape1.shape.circle.origoX == shape2.shape.circle.origoX && shape1.shape.circle.origoY == shape2.shape.circle.origoY && shape1.shape.circle.radius == shape2.shape.circle.radius) { printf("Ugyanazon kör van kiválasztva!\n"); }
        }
        
        else { intersectionList = twopoint_circle(shape1.shape.circle, shape2.shape.circle); }
    }

    else if (shape1.type != CIRCLE && shape2.type != CIRCLE) // every line and segmenent comination
    {
        double slope1 = shape1.type == LINE ? shape1.shape.line.slope : shape1.shape.segment.dirVector.y / shape1.shape.segment.dirVector.x;
        double slope2 = shape2.type == LINE ? shape2.shape.line.slope : shape2.shape.segment.dirVector.y / shape2.shape.segment.dirVector.x;

        if (is_parallel(shape1, shape2)){ intersectionList.intersectionCount = 0; }
        else
        {
            double b1 = return_b(shape1); double b2 = return_b(shape2);
            
            double x = (b2 - b1) / (slope1 - slope2); double y = slope1 * x + b1; // prev if case protects from 0 division
            if (double_zero(x)) { x = 0; }
            intersectionList.intersections[0].x = x; intersectionList.intersections[0].y = y; Vector2 point = vector2(x, y);
            intersectionList.intersectionCount = 1;
            if (shape1.type == SEGMENT && (!point_is_on_segment(shape1, point) || (shape2.type == SEGMENT && !point_is_on_segment(shape2, point)))) { intersectionList.intersectionCount = 0; }
        }
        // Infitine intersections is not possible, since no lines can be the same as an existing one
    }
    
    else if ((shape1.type == CIRCLE && shape2.type == LINE) || (shape1.type == LINE && shape2.type == CIRCLE) || (shape1.type == CIRCLE && shape2.type == SEGMENT) || (shape1.type == SEGMENT && shape2.type == CIRCLE))
    {
        Circle circle;
        circle = shape1.type == CIRCLE ? shape1.shape.circle : shape2.shape.circle;
        bool lineCmp = shape1.type == CIRCLE ? shape2.type == LINE : shape1.type == LINE;

        double slope = lineCmp ? shape2.shape.line.slope : shape2.shape.segment.dirVector.y / shape2.shape.segment.dirVector.x;
        double lineConst = return_b(shape1.type == CIRCLE ? shape2 : shape1);

        double a = 1 + pow(slope, 2);
        double b = -2 * circle.origoX + 2 * slope * (lineConst - circle.origoY);
        double c = pow(circle.origoX, 2) + pow(lineConst - circle.origoY, 2) - pow(circle.radius, 2);
        double discriminant = pow(b, 2) - 4 * a * c;

        if (discriminant < 0) { intersectionList.intersectionCount = 0; }
        else if (double_zero(discriminant))
        {
            intersectionList.intersectionCount = 1;
            intersectionList.intersections[0].x = -b / (2*a); intersectionList.intersections[0].y = slope * intersectionList.intersections[0].x + lineConst;
        }
        else
        {
            intersectionList.intersectionCount = 2;
            intersectionList.intersections[0].x = (-b + sqrt(discriminant)) / (2*a); intersectionList.intersections[0].y = slope * intersectionList.intersections[0].x + lineConst;
            intersectionList.intersections[1].x = (-b - sqrt(discriminant)) / (2*a); intersectionList.intersections[1].y = slope * intersectionList.intersections[1].x + lineConst;
        }
    }

    return intersectionList;
}

void intersect_helper(VectorX intersectionList)
{
    simplex_setctitle("GeoMax Editor - Metszéspontok");
    
    if (intersectionList.intersectionCount == 0) { printf("A két alakzat nem metszi egymást!\n"); }
    else if (intersectionList.intersectionCount == 1) { printf("Egy metszéspont található: (%.2lf, %.2lf)\n", intersectionList.intersections[0].x, intersectionList.intersections[0].y); }
    else if (intersectionList.intersectionCount == 2) { printf("Két metszéspont található: (%.2lf, %.2lf) és (%.2lf, %.2lf)\n", intersectionList.intersections[0].x, intersectionList.intersections[0].y, intersectionList.intersections[1].x, intersectionList.intersections[1].y); }
    clear_buffer(); print_dirback(); int input = getchar(); input++; back_to_menu(1000); return;
}

double return_b(Shape linear)
{
    if (linear.type == LINE) { return linear.shape.line.Y0 - linear.shape.line.slope * linear.shape.line.X0; } // b = y - ax
    else if (linear.type == SEGMENT) { return linear.shape.segment.Y0 - (linear.shape.segment.dirVector.y / linear.shape.segment.dirVector.x) * linear.shape.segment.X0; }
    else { return 0; }
}

bool point_is_on_segment(Shape segment, Vector2 point)
{
    bool onIt = false;
    double x0 = segment.shape.segment.X0; double x1 = segment.shape.segment.X1; double smallX = x0 < x1 ? x0 : x1; double bigX = x0 > x1 ? x0 : x1;
    double y0 = segment.shape.segment.Y0; double y1 = segment.shape.segment.Y1;
    double smallY = y0 < y1 ? y0 : y1; double bigY = y0 > y1 ? y0 : y1;

    if (double_equal(x0, x1)) { if (double_equal(point.x, x0) && point.y >= smallY && point.y <= bigY) { onIt = true; } }
    else if (double_equal(y0, y1)) { if (double_equal(point.y, y0) && point.x >= smallX && point.x <= bigX) { onIt = true; } }
    else
    {
        double slope = (y1 - y0) / (x1 - x0);
        double slopeWithPoint = (point.y - y0) / (point.x - x0);
        if (double_equal(slope, slopeWithPoint) && point.x >= smallX && point.x <= bigX && point.y >= smallY && point.y <= bigY) { onIt = true; }
    }
    
    return onIt;
}

bool is_parallel(Shape shape1, Shape shape2)
{
    double slope1 = 0; double slope2 = 0;
    if (shape1.type == LINE) { slope1 = shape1.shape.line.slope; }
    else if (shape1.type == SEGMENT) { slope1 = (shape1.shape.segment.Y1 - shape1.shape.segment.Y0) / (double_equal(shape1.shape.segment.X1, shape1.shape.segment.X0) ? EQUAL_THRESHOLD : shape1.shape.segment.X1 - shape1.shape.segment.X0); }

    if (shape2.type == LINE) { slope2 = shape2.shape.line.slope; }
    else if (shape2.type == SEGMENT) { slope2 = (shape2.shape.segment.Y1 - shape2.shape.segment.Y0) / (double_equal(shape2.shape.segment.X1, shape2.shape.segment.X0) ? EQUAL_THRESHOLD : shape2.shape.segment.X1 - shape2.shape.segment.X0); }
    
    if (fabs(slope1 - slope2) < EQUAL_THRESHOLD) { return true; }
    else { return false; }
}

Shape scale(Circle circle, double scalar)
{
    Shape newCircleShape; Circle newCircle = circle;
    if (scalar < 0) { scalar = fabs(scalar); }
    if (!double_equal(scalar, 1)) { newCircle.radius *= scalar; }
    newCircleShape.shape.circle = newCircle; newCircleShape.type = CIRCLE;
    return newCircleShape;
}

double xy_intersect(Line *line, Segment *segment, int mode, int monotonity)
{
    // y - y1 = m(x-x1)
    // y-intercept: x = 0 -> y = y1 - m*x1
    // x-intercept: y = 0 -> x = -y1/m + x1
    
    double xIntercept = 0; double yIntercept = 0;

    if (line != NULL)
    {
        if (mode == 0) { yIntercept = line->Y0 - line->slope*line->X0; }
        else { xIntercept = -line->Y0/line->slope + line->X0; }
    }
    else
    {        
        if (mode == 0 && monotonity != INFINITE_SLOPE)
        {
            yIntercept = segment->Y0 - (segment->dirVector.y/segment->dirVector.x) * segment->X0;
        }
        else if (mode == 1 && monotonity != 0)
        {
            xIntercept = -1*segment->Y0/(segment->dirVector.y/segment->dirVector.x) + segment->X0;
        }
        else { return NO_INTERCEPT; }
    }

    return mode == 0 ? yIntercept : xIntercept;
}

VectorX twopoint_circle(Circle circle1, Circle circle2)
{
    VectorX intersectionList; intersectionList.intersectionCount = 0;

    double deltaX = circle2.origoX - circle1.origoX;
    double deltaY = circle2.origoY - circle1.origoY;
    double dist = sqrt(pow(deltaX, 2) + pow(deltaY, 2));
    double r1N = pow(circle1.radius, 2); double r2N = pow(circle2.radius, 2);

    double a = (r1N - r2N + pow(dist, 2)) / (2 * dist); // Derives from the Pithyagorean theorem - intersection segment splits dist into two parts
    double h = sqrt(r1N - pow(a, 2)); // Pythagorean theorem again - height of the triangles (distance from center line to intersection point)

    // Point to which +-h is added later (proportional to the distance between the centers)
    double cx = circle1.origoX + a * (deltaX / dist); double cy = circle1.origoY + a * (deltaY / dist);

    double mult = deltaX / dist;
    if (double_zero(deltaX)) { mult = 0; } // address the case of infinite
    if (double_zero(deltaY)) { mult = 1; } // address the case of 0 slope

    double intersectionX1 = cx + h * (deltaY / dist); double intersectionY1 = cy - h * (mult); // "upper"
    double intersectionX2 = cx - h * (deltaY / dist); double intersectionY2 = cy + h * (mult); // "lower"

    intersectionList.intersections[0].x = intersectionX1; intersectionList.intersections[1].x = intersectionX2;
    intersectionList.intersections[0].y = intersectionY1; intersectionList.intersections[1].y = intersectionY2;
    intersectionList.intersectionCount = (double_equal(intersectionX1, intersectionX2) && (double_equal(intersectionY1, intersectionY2))) ? 1 : 2;

    return intersectionList;
}

void process_new_shape(Shape originalShape, Shape newShapeElement, bool justModify)
{
    if (justModify)
    {
        if (originalShape.type == CIRCLE)
        {
            if (check_existing_circle(newShapeElement.shape.circle)) { printf("Az alakzat már létezik!\n"); back_to_menu(1000); return; }
            
            CircleListElement *next;
            for (next = get_shape_list()->firstCircle; next != NULL; next = next->next)
            {
                if (double_equal(next->circle.origoX, originalShape.shape.circle.origoX) && double_equal(next->circle.origoY, originalShape.shape.circle.origoY) && double_equal(next->circle.radius, originalShape.shape.circle.radius))
                {
                    next->circle = newShapeElement.shape.circle;
                    break;
                }
            }
        }
        else if (originalShape.type == LINE)
        {
            if (check_existing_line(newShapeElement.shape.line)) { printf("Az alakzat már létezik!\n"); back_to_menu(1000); return; }
            
            LineListElement *next;
            for(next = get_shape_list()->firstLine; next != NULL; next = next->next)
            {
                if (double_equal(next->line.X0, originalShape.shape.line.X0) && double_equal(next->line.Y0, originalShape.shape.line.Y0) && double_equal(next->line.slope, originalShape.shape.line.slope))
                {
                    next->line = newShapeElement.shape.line;
                    break;
                }
            }
        }
        else if (originalShape.type == SEGMENT)
        {
            if (check_existing_segment(newShapeElement.shape.segment)) { printf("Az alakzat már létezik!\n"); back_to_menu(1000); return; }
            
            SegmentListElement *iterator3;
            for (iterator3 = get_shape_list()->firstSegment; iterator3 != NULL; iterator3 = iterator3->next)
            {
                if (double_equal(iterator3->segment.X0, originalShape.shape.segment.X0) && double_equal(iterator3->segment.Y0, originalShape.shape.segment.Y0) && double_equal(iterator3->segment.X1, originalShape.shape.segment.X1) && double_equal(iterator3->segment.Y1, originalShape.shape.segment.Y1))
                {
                    iterator3->segment = newShapeElement.shape.segment;
                    break;
                }
            }
        }
    }
    else
    {
        if (newShapeElement.type == CIRCLE)
        {
            if (check_existing_circle(newShapeElement.shape.circle)) { printf("Az alakzat már létezik!\n"); back_to_menu(1000); return; }
            CircleListElement *newNode = create_circle_node(newShapeElement.shape.circle);
            add_circle_node_tolist(newNode);
        }
        else if (newShapeElement.type == LINE)
        {
            if (check_existing_line(newShapeElement.shape.line)) { printf("Az alakzat már létezik!\n"); back_to_menu(1000); return; }
            LineListElement *newNode = create_line_node(newShapeElement.shape.line);
            add_line_node_tolist(newNode);
        }
        else if (newShapeElement.type == SEGMENT)
        {
            if (check_existing_segment(newShapeElement.shape.segment)) { printf("Az alakzat már létezik!\n"); back_to_menu(1000); return; }
            SegmentListElement *newNode = create_segment_node(newShapeElement.shape.segment);
            add_segment_node_tolist(newNode);
        }
    }

    Shape nullShape; nullShape.type = UNDEFINED; resize_viewport(nullShape);
}

// ALAKZATOK_HELPER.C - VIEWPORT OPERATIONS -----------------------------------------------------------------------------

void resize_viewport(Shape shape)
{
    double viewportMult = 1;
    
    if (shape.type == LINE)
    {
        if (worksheet_data_request(SHAPE_COUNT) == worksheet_data_request(LINE_COUNT)) { worksheet_data_override(100, VIEWPORT_MAX); }
    }

    else if (shape.type == CIRCLE)
    {
        double xMax = fabs(shape.shape.circle.origoX) + shape.shape.circle.radius;
        double yMax = fabs(shape.shape.circle.origoY) + shape.shape.circle.radius;
        worksheet_data_override((yMax <= xMax ? xMax: yMax) * viewportMult, VIEWPORT_MAX);
    }

    else if (shape.type == SEGMENT)
    {
        double xMax = fabs(shape.shape.segment.X0) > fabs(shape.shape.segment.X1) ? fabs(shape.shape.segment.X0) : fabs(shape.shape.segment.X1);
        double yMax = fabs(shape.shape.segment.Y0) > fabs(shape.shape.segment.Y1) ? fabs(shape.shape.segment.Y0) : fabs(shape.shape.segment.Y1);
        worksheet_data_override((yMax <= xMax ? xMax: yMax) * viewportMult, VIEWPORT_MAX);
    }

    else if (shape.type == UNDEFINED)
    {
        double xyBiggest = 0;
        
        CircleListElement *iterator1 = get_shape_list()->firstCircle;
        while (iterator1 != NULL)
        {
            CircleListElement *next = iterator1->next;
            if (fabs(iterator1->circle.origoX) + iterator1->circle.radius > xyBiggest) { xyBiggest = fabs(iterator1->circle.origoX) + iterator1->circle.radius; }
            if (fabs(iterator1->circle.origoY) + iterator1->circle.radius > xyBiggest) { xyBiggest = fabs(iterator1->circle.origoY) + iterator1->circle.radius; }
            iterator1 = next;
        }

        SegmentListElement *iterator3 = get_shape_list()->firstSegment;
        while (iterator3 != NULL)
        {
            SegmentListElement *next = iterator3->next;
            if (fabs(iterator3->segment.X0) > xyBiggest) { xyBiggest = fabs(iterator3->segment.X0); }
            if (fabs(iterator3->segment.Y0) > xyBiggest) { xyBiggest = fabs(iterator3->segment.Y0); }
            if (fabs(iterator3->segment.X1) > xyBiggest) { xyBiggest = fabs(iterator3->segment.X1); }
            if (fabs(iterator3->segment.Y1) > xyBiggest) { xyBiggest = fabs(iterator3->segment.Y1); }
            iterator3 = next;
        }

        xyBiggest = fabs(xyBiggest) * viewportMult; worksheet_data_override(xyBiggest, VIEWPORT_MAX);
    }
    else { return; }
}

Vector2 DescartesToTopLeft(Vector2 input)
{
    double originalY = input.y;
    input.x += (worksheet_data_request(VIEWPORT_MAX));
    input.y = ((worksheet_data_request(VIEWPORT_MAX)) - originalY);

    return input;
}

Vector2 TopLeftToDescartes(Vector2 input)
{
    double originalY = input.y;
    input.x -= (worksheet_data_request(VIEWPORT_MAX));
    input.y = ((worksheet_data_request(VIEWPORT_MAX)) - originalY);

    return input;
}

LineTerminus line_terminus(Line line)
{
    LineTerminus terminus; Shape linear; linear.type = LINE; linear.shape.line = line;

    double x = -worksheet_data_request(VIEWPORT_MAX); double y = return_b(linear) + line.slope * (x);
    terminus.start = vector2(x, y);
    
    x = worksheet_data_request(VIEWPORT_MAX); y = return_b(linear) + line.slope * (x);
    terminus.end = vector2(x, y);

    return terminus;
}