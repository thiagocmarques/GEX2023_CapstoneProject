#pragma once
#include "Scene.h"
#include <vector>

class Scene_Menu : public Scene
{
private:
    std::vector<std::pair<MenuItem, std::string>>	m_menuItems;
//    std::vector<std::string>	                    m_levelPaths;
    int							                    m_menuIndex{ 0 };
    std::string					                    m_title;

    sf::Text					                    m_menuText;
    sf::Text					                    m_menuTitle;



    void                                            init();
    void                                            onEnd() override;

public:

    Scene_Menu(GameEngine* gameEngine);

    void                                            registerItem(MenuItem key, std::string item);
    void		                                    update(sf::Time dt) override;
    void		                                    sDoAction(const Action& action) override;
    void		                                    sRender() override;

    SceneID                                         getSceneInMenu(MenuItem m);
};
