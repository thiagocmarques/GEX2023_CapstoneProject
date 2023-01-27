#pragma once
#include "Scene.h"
#include <vector>

class Scene_Settings : public Scene
{
private:
	std::vector<std::pair<SceneID, std::string>>	m_menuItems;
    int							                    m_menuIndex{ 0 };
    std::string					                    m_title;

    sf::Text					                    m_menuText;
    sf::Text					                    m_menuTitle;

    void                                            init();
    void                                            onEnd() override;

public:

    Scene_Settings(GameEngine* gameEngine);

    void                                            registerItem(SceneID key, std::string item);
    void		                                    update(sf::Time dt) override;
    void		                                    sDoAction(const Action& action) override;
    void		                                    sRender() override;
};

