#include "Vec2.h"
#include <math.h>

Vec2::Vec2()
{

}

Vec2::Vec2(float xin, float yin)
    :x(xin), y(yin)
{

}

//bools
bool Vec2::operator == (const Vec2& rhs) const //reevaluate float equality eventually
{

    return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator != (const Vec2 & rhs) const
{
    return (x != rhs.x || y != rhs.y);
}

//math that returns Vec2
Vec2 Vec2::operator + (const Vec2& rhs) const
{
    return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2 & rhs) const
{
    return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator / (const float val) const
{
    return Vec2(x / val, y / val);
}

Vec2 Vec2::operator * (const float val) const
{
    return Vec2(x * val, y * val);
}

//math on this Vec2
void Vec2::operator += (const Vec2 &rhs)
{
    x += rhs.x;
    y += rhs.y;
}
void Vec2::operator -= (const Vec2 &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
}
void Vec2::operator *= (const float val)
{
    x *= val;
    y *= val;
}
void Vec2::operator /= (const float val)
{
    x /= val;
    y /= val;
}

float Vec2::dist(const Vec2 &rhs) const
{
    return (sqrt(((rhs.x-x)*(rhs.x-x))+((rhs.y-y)*(rhs.y-y))));
}

float Vec2::length() const
{
    return (sqrt((x*x)+(y*y)));
}

float Vec2::distsquared(const Vec2 &rhs) const
{
    return (((rhs.x-x)*(rhs.x-x))+((rhs.y-y)*(rhs.y-y)));
}

Vec2 Vec2::rotate(float angle_deg)
{
    float rad = angle_deg * (M_PI / 180);
    return Vec2(x * cos(rad) - y * sin(rad), x * sin(rad) + y * cos(rad));
}
