#pragma once

#include "Scene.h"

class Scene_Play : public Scene
{
private:
    std::shared_ptr<Entity>		                    m_player;

    sf::View                                        m_worldView;                // the view/window (the size is returned by calling getSize())
    sf::FloatRect                                   m_worldBounds;              // the size of the world

    float                                           m_scrollSpeed{80.f};
    float                                           m_playerSpeed{800.f};

    sf::Vector2f                                    m_spawnPosition;            
    bool						                    m_drawTextures{ true };
    bool						                    m_drawAABB{ false };
    bool				                            m_drawGrid{ false };
   
    
    //bool                                            isPaused{false};

    void                                            init();
    void                                            onEnd() override;

    // systems
    void                                            sMovement(sf::Time dt);
    void                                            sViewMovement(sf::Time dt);
    void                                            sPlayerMovement();
    void                                            sAdjustPlayer();
    void                                            sCheckPlayerState();
    void                                            sUpdateOxygenLevel(sf::Time dt);
    void                                            sDrawOxygenBar();
    void                                            sDrawScore();


public:

    Scene_Play(GameEngine* gameEngine);

    void		                                    update(sf::Time dt) override;
    void		                                    sDoAction(const Action& action) override;
    void		                                    sRender() override;

    void                                            drawAABB();


    sf::FloatRect                                   getViewBounds();
    void                                            registerActions();
    void                                            spawnPlayer();
    void                                            loadScenarios();
};
