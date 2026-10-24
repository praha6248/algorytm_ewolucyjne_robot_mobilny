#include "Point.h"
#include <cmath>

Point::Point() 
    : x(0.0), y(0.0) {}

Point::Point(double x, double y) 
    : x(x), y(y) {}

double Point::getX() const {
    return x;
}

double Point::getY() const {
    return y;
}

void Point::setX(double x) {
    x = x;
}

void Point::setY(double y) {
    y = y;
}