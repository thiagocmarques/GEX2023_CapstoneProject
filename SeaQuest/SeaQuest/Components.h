#pragma once
#include <SFML/Graphics.hpp>
#include "Utilities.h"
#include "Animation.h"
#include <array>
#include "Enums.h"


struct Component {
    bool has{ false };
    Component() = default;
};


struct CAutoPilot : public Component
{
    std::array<float, 5> bearings{ 45, 0, -45, 0, 45 };
    std::array<sf::Time, 5> lengths{
            sf::seconds(0.5),
            sf::seconds(1),
            sf::seconds(1),
            sf::seconds(1),
            sf::seconds(0.5)
    };

    size_t legs{ 5 };
    size_t currentLeg{ 0 };
    sf::Time countdown{ sf::Time::Zero };

    CAutoPilot() = default;
};


struct CDivers : public Component {
    size_t    diversCount{ 0 };
    size_t    MAX_DIVERS{ 6 };

    CDivers() = default;
};


struct CSprite : public Component {
    sf::Sprite sprite;

    CSprite() = default;


    CSprite(const sf::Texture& t)
        : sprite(t) {
        centerOrigin(sprite);
    }


    CSprite(const sf::Texture& t, sf::IntRect r)
        : sprite(t, r) {
        centerOrigin(sprite);
    }
};


struct CAnimation : public Component {
    Animation   animation;

    CAnimation() = default;
    CAnimation(const Animation& a) : animation(a) {}

};


struct COxygen : public Component {
    float           oxygenLvl{ 1.f };
    bool            isDrainingOxygen{ false };

    float           drainRate{ 5.f };
    float           fillingRate = drainRate * 9.f;

    COxygen() = default;
    COxygen(float oxygen) : oxygenLvl(oxygen) {}

    bool            isLowOxygen{ false };

};

struct CState : public Component {
    State               state{State::SPAWN};
    CState() = default;
    CState(const State& s) : state(s) {}

};

struct CTransform : public Component {
    sf::Vector2f    pos{ 0.f, 0.f };
    sf::Vector2f    vel{ 0.f, 0.f };
    sf::Vector2f    scale{ 1.f, 1.f };
    float           rot{ 0.f };
    float           rotVel{ 0.f };

    bool            headingLeft{ true };
    CTransform() = default;

    CTransform(sf::Vector2f p, sf::Vector2f v, float r = 0.f, float rs = 0.f)
        : pos(p), vel(v), rot(r), rotVel(rs) {}

    CTransform(sf::Vector2f p, sf::Vector2f v, bool headingLeft)
        : pos(p), vel(v), headingLeft(headingLeft) {}

};


struct CShape : public Component {
    sf::CircleShape shape;

    CShape() = default;


    CShape(float radius, sf::Color color = sf::Color::Red, size_t points = 30) {
        shape.setRadius(radius);
        shape.setFillColor(color);
        shape.setPointCount(points);
        centerOrigin(shape);
    }

    CShape(float radius, size_t points, sf::Color fillColor, sf::Color outColor, float thickness) {
        shape.setRadius(radius);
        shape.setFillColor(fillColor);
        shape.setOutlineColor(outColor);
        shape.setPointCount(points);
        shape.setOutlineThickness(thickness);
        centerOrigin(shape);
    }
};


struct CCollision : public Component {
    float radius{ 0.f };

    CCollision() = default;


    CCollision(float r) : radius(r) {}
};

struct CBoundingBox : public Component {
    sf::Vector2f size;
    sf::Vector2f halfSize;

    CBoundingBox() = default;


    CBoundingBox(const sf::Vector2f& s)
        : size(s), halfSize(s / 2.f) {}


    sf::FloatRect getRect(sf::Vector2f pos) {
        return sf::FloatRect(pos + halfSize, size);
    }
};

struct CInput : public Component {
    bool up{ false };
    bool left{ false };
    bool right{ false };
    bool down{ false };

    bool shoot{ false };
    sf::Vector2f    shootPos{ 0.f, 0.f };

    CInput() = default;
};

struct CGun : public Component {
    bool isFiring{ false };
    int fireRate{ 1 };


    sf::Time countdown{ sf::Time::Zero };


    CGun() = default;

    CGun(bool firing, int rate) 
        : isFiring(firing), fireRate(rate) {}

    CGun(int rate): fireRate(rate){}

};
