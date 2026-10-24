#pragma once 

class Point {
private:
    double x;
    double y;

public:
    Point();
    Point(double x, double y);
    double getX() const;
    double getY() const;
    void setX(double x);
    void setY(double y);
};