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
#include "Scene.h"
#include <vector>

class Scene_Settings : public Scene
{
private:
	std::vector<std::pair<MenuItem, std::string>>	m_menuItems;
    int							                    m_menuIndex{ 0 };
    std::string					                    m_title;

    sf::Text					                    m_menuText;
    sf::Text					                    m_menuTitle;

    ActionName                                      m_lastAction{ ActionName::NONE };
    int                                             m_repeatingActionDelayCount{ 0 };
    const int                                       REPEAT_ACTION_DEFAULT_DELAY{ 35 };

    void                                            init();
    void                                            onEnd() override;

public:

    Scene_Settings(GameEngine* gameEngine);

    void                                            registerItem(MenuItem key, std::string item);
    void		                                    update(sf::Time dt) override;
    void		                                    sDoAction(const Action& action) override;
    void		                                    sRender() override;
    void		                                    sPassTextEnteredEvent(sf::Event event) override;
    SceneID                                         getSceneInMenu(MenuItem m);
};

