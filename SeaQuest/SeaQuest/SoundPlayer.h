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
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/System/Vector2.hpp>

#include <map>
#include <list>
#include <string>
#include <memory>


using SndBufPtr = std::unique_ptr<sf::SoundBuffer>;

class SoundPlayer
{
protected:
    SoundPlayer();

private:
    std::map<std::string, SndBufPtr>            m_soundBuffers;
    std::list<sf::Sound>                        m_sounds;
    bool                                        enabled = true;

public:
    void                                        play(std::string effect);
    void			                            play(std::string effect, sf::Vector2f position);
    void			                            removeStoppedSounds();
    void			                            setListnerPosition(sf::Vector2f position);
    sf::Vector2f	                            getListnerPosition() const;

    bool                                        isEmpty() const;
    void			                            loadBuffer(std::string id, const std::string path);
    void                                        stopAll();
    void                                        toggleEnabled();
    bool                                        isEnabled();

public:
    static SoundPlayer&                         getInstance();

    // deleting copy and move constructors for singleton
    SoundPlayer(const SoundPlayer&)             = delete;
    SoundPlayer(SoundPlayer&&)                  = delete;
    SoundPlayer& operator=(const SoundPlayer&)  = delete;
    SoundPlayer& operator=(SoundPlayer&&)       = delete;
};

