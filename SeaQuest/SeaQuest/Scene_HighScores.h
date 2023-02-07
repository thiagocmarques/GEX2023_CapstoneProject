#pragma once
#include "Scene.h"
#include <vector>
#include "json.h"

using json = nlohmann::json;

struct HighScore {
    std::string                                     playerInitials;
    unsigned long                                   score;
    size_t                                          maxLevel;
};

class Scene_HighScores : public Scene
{
private:
    std::vector<std::pair<SceneID, std::string>>	m_menuItems;
    int							                    m_menuIndex{ 0 };
    std::string					                    m_title;

    sf::Text					                    m_menuText;
    sf::Text					                    m_menuTitle;

    std::vector<HighScore>                          m_highScores;
    json                                            dataFromFile;
    void                                            loadScores();
    

    void                                            init();
    void                                            onEnd() override;

public:

    Scene_HighScores(GameEngine* gameEngine);

    void                                            registerItem(SceneID key, std::string item);
    void		                                    update(sf::Time dt) override;
    void		                                    sDoAction(const Action& action) override;
    void		                                    sRender() override;
};


