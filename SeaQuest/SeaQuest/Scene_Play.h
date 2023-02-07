#pragma once

#include "Scene.h"

class Scene_Play : public Scene
{
private:
    std::shared_ptr<Entity>		                    m_player;

    sf::View                                        m_worldView;
    sf::FloatRect                                   m_worldBounds;

    float                                           m_scrollSpeed{80.f};
    float                                           m_playerSpeed{450.f};

    sf::Vector2f                                    m_spawnPosition;
    bool						                    m_drawTextures{ true };
    bool						                    m_drawAABB{ false };
    bool				                            m_drawGrid{ false };
    
    bool                                            isPaused{false};

    void                                            init();
    void                                            onEnd() override;

    // systems
    void                                            sMovement(sf::Time dt);
    void                                            sViewMovement(sf::Time dt);
    void                                            sPlayerMovement();
    void                                            adjustPlayer();
    void                                            checkPlayerState();

public:

    Scene_Play(GameEngine* gameEngine);

    void		                                    update(sf::Time dt) override;
    void		                                    sDoAction(const Action& action) override;
    void		                                    sRender() override;

    void                                            drawAABB();


    sf::FloatRect                                   getViewBounds();
    void                                            registerActions();
    void                                            spawnPlayer();
    void                                            loadBackground();
};
