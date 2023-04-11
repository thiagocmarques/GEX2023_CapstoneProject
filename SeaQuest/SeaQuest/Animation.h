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
#include <SFML/Graphics.hpp>
#include <vector>

class Animation {
public:
    std::string                 m_name{ "none" };
    std::vector<sf::IntRect>    m_frames;
    size_t                      m_currentFrame{ 0 };
    sf::Time                    m_timePerFrame;
    sf::Time                    m_countDown{ sf::Time::Zero };
    bool                        m_isRepeating{ true };
    bool                        m_hasEnded{ false };
    sf::Sprite                  m_sprite;


public:
    Animation() = default;
    Animation(  const std::string& name, 
                const sf::Texture& t,
                std::vector<sf::IntRect> frames, 
                sf::Time tpf, 
                bool repeats = true
                );

    void                        update(sf::Time dt);
    bool                        hasEnded() const;
    const std::string&          getName() const;
    sf::Sprite&                 getSprite();
};

