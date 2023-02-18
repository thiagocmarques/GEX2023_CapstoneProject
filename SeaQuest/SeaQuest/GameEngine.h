//
// Author...: Thiago Marques
// Date.....: 2023-01-16
// 
// Based on the David Burchill's example
//

#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "EntityManager.h"
#include "Assets.h"

#include <map>
#include <string>
#include <functional>


class Scene;

using Sptr			        = std::shared_ptr<Scene>;
using SceneMap		        = std::map<SceneID, Sptr>;
using MusicMap		        = std::map<SceneID, Sptr>;
using FactoryMap	        = std::map<SceneID, std::function<Sptr()>>;


class GameEngine
{
private:
    sf::Vector2u            m_windowSize{ 1280, 768 };  
    sf::RenderWindow        m_window;
    SceneID                 m_currentScene;
    SceneID                 m_lastScene;
    Assets                  m_assets;

    // maps
    SceneMap                m_sceneMap;     
    SceneMap                m_musicMap;     
    FactoryMap              m_factories;

    bool                    m_isRunning{ true };
    const static sf::Time   TIME_PER_FRAME;

    // stats
    sf::Text                m_statisticsText;
    sf::Time                m_statisticsUpdateTime{ sf::Time::Zero };
    unsigned int            m_statisticsNumFrames{ 0 };
    void                    updateStatistics(sf::Time dt);


    void                    init(const std::string& path);
    void                    sUserInput();
    Sptr                    currentScene();
    void                    createFactories();
    void                    createMenu();
    void                    createMenuSettings();
    void                    createMenuHighScores();
    void                    changeMusic();

    

public:
    GameEngine(const std::string& configPath);  // CTOR

    void                    run();
    bool                    isRunning();
    void                    quitGame();


    void                    changeScene(SceneID id, bool endCurrentScene = false);
    sf::RenderWindow&       getWindow();
    void                    quitLevel();
    void                    backLevel();
    const Assets&           assets() const;


};

