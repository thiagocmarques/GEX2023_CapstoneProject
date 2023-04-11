// New Brunswick Community College
// Gaming Experience Development
// -------------------------------
// Capstone Project - SeaQuest
// -------------------------------
// Instructor: David Burchill
// Student: Thiago Marques
// 
// April 2023
// 


#pragma once

#include <SFML/System.hpp>
#include <iostream>


float           length(const sf::Vector2f& v);
float           dist(const sf::Vector2f& a, const sf::Vector2f& b);

sf::Vector2f    normalize(sf::Vector2f v);
float           bearing(const sf::Vector2f& v);
sf::Vector2f    uVecFromBearing(float d);

float           radToDeg(float r);
float           degToRad(float d);

std::ostream& operator<<(std::ostream& os, sf::Vector2f v);


// center sprite origin
template <typename T>
void centerOrigin(T& s) {
    auto bounds = s.getLocalBounds();
    s.setOrigin(sf::Vector2f(bounds.width / 2.f, bounds.height / 2.f));
}

template <typename T>
void centerOrigin(T* s) {
    auto bounds = s->getLocalBounds();
    s->setOrigin(sf::Vector2f(bounds.width / 2.f, bounds.height / 2.f));
}

