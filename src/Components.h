#pragma once

#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <iostream>

class CTransform
{
public:
    Vec2 pos = {0.0, 0.0};
    Vec2 velocity = {0.0, 0.0};
    float angle = 0;

    CTransform(const Vec2 &p, const Vec2 &v, float a)
        : pos(p), velocity(v), angle(a){}
};

class CShape
{
public:
    sf::CircleShape circle;

    CShape(float radius, int points, const sf::Color &fill, 
            const sf::Color &outline, float thickness)
        : circle(radius, points)
    {
        circle.setFillColor(fill);
        circle.setOutlineColor(outline);
        circle.setOutlineThickness(thickness);
        circle.setOrigin(radius, radius);
    }
};

class CCollision
{
public:
    float radius = 0;
    CCollision(float r)
        : radius(r) {}
};

class CInput
{
public:
    bool up = false;
    bool left = false;
    bool right = false;
    bool down = false;
    bool shoot = false;

    CInput() {}
};

class CScore
{
public:
    int score = 0;
    CScore(int s)
        : score(s) {}
};

class CLifespan
{
public:
    int remaining = 0;  //amount of lifespan remaining on the entity
    int total = 0;      //the total initial amount of lifespan
    CLifespan(int total)
        : remaining(total), total(total) {}
};

class CText
{
public:
    sf::Text text;
    CText(const sf::Font &font, int fontSize, sf::Color textColor)
        {
            text.setFont(font);
            text.setCharacterSize(fontSize);
            text.setFillColor(textColor);
            text.setString("test");
        }
};

class CGravity
{
public:
    Vec2 gravity = {0,0};
    CGravity(const Vec2 &p)
    : gravity(p){}
};