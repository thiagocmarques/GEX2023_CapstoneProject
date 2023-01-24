//
// Author...: Thiago Marques
// Date.....: 2023-01-16
// 
// Based on the David Burchill's example
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
    float							            m_volume{ 25 };

private:
    MusicPlayer();
    ~MusicPlayer()                              = default;

public:

    void							            play(String theme);
    void							            stop();
    void							            togglePause(bool paused);
    void							            setVolume(float volume);
    void                                        loadMusicFilenames(String key, String path);


public:
    static MusicPlayer&                         getInstance();

    // deleting copy and move constructors for singleton
    MusicPlayer(const MusicPlayer&)             = delete;
    MusicPlayer(MusicPlayer&&)                  = delete;
    MusicPlayer& operator=(const MusicPlayer&)  = delete;
    MusicPlayer& operator=(MusicPlayer&&)       = delete;




};

