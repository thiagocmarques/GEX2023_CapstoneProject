#pragma once

#include "Scene.h"

class Scene_Play : public Scene
{
private:
    std::shared_ptr<Entity>		                    m_player;

    sf::View                                        m_worldView;                        // the view/window (the size is returned by calling getSize())
    sf::FloatRect                                   m_worldBounds;                      // the size of the world

    float                                           m_scrollSpeed{80.f};
    float                                           m_playerSpeed{800.f};

    sf::Vector2f                                    m_spawnPosition;            
    bool						                    m_drawTextures{ true };
    bool						                    m_drawAABB{ false };
    bool				                            m_drawGrid{ false };

    bool                                            m_isOnSurface{ true };              // true if player is on surface
   
    const float                                     MIN_Y_POSITION{ 675.f };
    
    void                                            init();
    void                                            onEnd() override;

    // systems
    void                                            sMovement(sf::Time dt);             // all entities movements
    void                                            sPlayerMovement();                  // player movements according to input keys pressed
    void                                            sViewMovement(sf::Time dt);         // moves the world view accordingly to player movements
    void                                            sAdjustPlayer();                    // keeps player in bounds
    void                                            sAdjustPlayerTexture();             // change player texture to face left and right properly
    void                                            sUpdateOxygenLevel(sf::Time dt);    // lower or raises oxygen level accordingly to submerging or getting to surface
    void                                            sDrawOxygenBar();                   // draws Oxygen bar, diver count, and the game name on top left corner
    void                                            sDrawScore();                       // draws the score on the top right corner
    void                                            sStateMachine();                    // TODO

    // game helper functions
    void                                            diverLoad();                        // unload one diver from Sub
    void                                            diverUnload();                      // load one diver to Sub
    bool                                            isSubFullyLoaded();                 // true if sub is on its maximum capacity
    bool                                            isSubEmpty();                       // true if sub has no diver
    void                                            updateState();                      // update player state 
    void                                            passLevel(sf::Time dt);             // called when sub gets to surface fully loaded
    void                                            emptyOxygenTank();                  // give points for the remaining oxygen in tank when passing level
    void                                            bonusPerDiver();                    // give points for each diver when passing level
    void                                            checkIfDead();                      // checks state and do all stuff needed when player dies
    void                                            restartGame();                      // puts the game on initial state after passing level or die


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
