//
// Author...: Thiago Marques
// Date.....: 2023-01-16
// 
// Based on the David Burchill's example
//

#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <map>
#include <string>
#include <functional>


class Scene;
enum class SceneID { NONE, MENU, PLAY };

using Sptr			= std::shared_ptr<Scene>;
using SceneMap		= std::map<SceneID, Sptr>;
using MusicMap		= std::map<SceneID, Sptr>;
using FactoryMap	= std::map<SceneID, std::function<Sptr()>>;


class GameEngine
{
private:
    sf::Vector2u            m_windowSize{ 1280, 768 };  
    sf::RenderWindow        m_window;
    //Assets                  m_assets;
    SceneID                 m_currentScene;

    // maps
    SceneMap                m_sceneMap;     
    SceneMap                m_musicMap;     
    FactoryMap              m_factories;

    bool                    m_isRunning{ true };
    const static sf::Time   TIME_PER_FRAME;


    void                    init(const std::string& path);

    

public:
    GameEngine(const std::string& configPath);  // CTOR

    void                    run();
    bool                    isRunning();
};

