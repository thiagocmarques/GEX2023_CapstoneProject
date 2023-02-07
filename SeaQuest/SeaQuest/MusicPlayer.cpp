#include "MusicPlayer.h"
#include <stdexcept>

MusicPlayer::MusicPlayer()
{
}

void MusicPlayer::play(String theme)
{

    if (!m_music.openFromFile(m_filenames[theme]))
        throw std::runtime_error("Music could not open file");

    m_music.setVolume(m_volume);
    m_music.setLoop(true);

    if (enabled)
        m_music.play();
}

void MusicPlayer::stop()
{
    m_music.stop();
}

void MusicPlayer::togglePause()
{
    if (isPaused())
        m_music.play();
    else
        m_music.pause();
}

void MusicPlayer::setVolume(float volume)
{
    m_volume = volume;
    m_music.setVolume(m_volume);
}

void MusicPlayer::loadMusicFilenames(String key, String path)
{
    m_filenames[key] = path;
}

bool MusicPlayer::isPaused()
{
    return m_music.getStatus() == m_music.Paused;
}

void MusicPlayer::toggleEnabled()
{
    enabled = !enabled;

    if (enabled)
        m_music.play();
    else
        m_music.stop();
}

bool MusicPlayer::isEnabled()
{
    return enabled;
}

MusicPlayer& MusicPlayer::getInstance()
{
    static MusicPlayer instance;
    return instance;
}
