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

#include <map>
#include <string>
#include <SFML/Audio/Music.hpp>


using String = std::string;
class MusicPlayer
{
private:
    sf::Music						            m_music;
    std::map<String, String>	                m_filenames;
    float							            m_volume{ 15 };
    bool                                        enabled = true;

private:
    MusicPlayer();
    ~MusicPlayer()                              = default;

public:

    void							            play(String theme);
    void							            stop();
    void							            togglePause();
    void							            setVolume(float volume);
    void                                        loadMusicFilenames(String key, String path);
    bool                                        isPaused();
    void                                        toggleEnabled();
    bool                                        isEnabled();

public:
    static MusicPlayer&                         getInstance();

    // deleting copy and move constructors for singleton
    MusicPlayer(const MusicPlayer&)             = delete;
    MusicPlayer(MusicPlayer&&)                  = delete;
    MusicPlayer& operator=(const MusicPlayer&)  = delete;
    MusicPlayer& operator=(MusicPlayer&&)       = delete;




};

