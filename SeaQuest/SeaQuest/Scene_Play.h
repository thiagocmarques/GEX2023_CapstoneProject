#pragma once

#include "Scene.h"

class Scene_Play : public Scene
{
private:
	std::shared_ptr<Entity>		                    m_player;

	sf::View                                        m_worldView;                        // the view/window (the size is returned by calling getSize())
	sf::FloatRect                                   m_worldBounds;                      // the size of the world

	float                                           m_scrollSpeed{ 80.f };
	float                                           m_playerSpeed{ 800.f };
	float											m_bulletSpeed{ 3000.f };
	float                                           m_diverSpeed{ 150.f };
	float                                           m_enemySubSpeed{ 250.f };

	sf::Vector2f                                    m_spawnPosition;
	bool						                    m_drawTextures{ true };
	bool						                    m_drawAABB{ false };
	bool				                            m_drawGrid{ false };

	bool                                            m_isOnSurface{ true };              // true if player is on surface
	bool                                            m_isGameOver{ false };              


	std::map<int, sf::Texture>						pDeadTextures;
	unsigned int									gameScore{ 0 };
	
	float											virtualLaneHeight{ 0.f };
	float											virtualLaneGap{ 60.f };
	size_t											virtualLaneCount{ 8 };

	size_t											m_extraLivesCount{ 3 };

	const float                                     MIN_Y_POSITION{ 675.f };
	const float										LOW_OXYGEN_LVL{ 35.f };
	const int										BULLET_COLLISION_RADIUS{ 15 };
	const float										UNLOAD_SOUND_LENGTH{ 0.35f };
	const float										DROP_OXGN_SOUND_LENGTH{ 0.43f };
	const float										PLR_DEAD_SOUND_LENGTH{ 0.65f };
	const float										LOW_OXGN_SOUND_LENGTH{ 0.52f };
	const float										DELAY_TO_RESTART{ 0.95f };
	const float										MAX_OXGN_LVL { 100.f };
	const float										MIN_OXGN_LVL { 0.f };
	const sf::Color									OXGN_BAR_COLOR{ sf::Color(234, 87, 26) };
	const sf::Color									PLAYER_SUB_COLOR{ sf::Color(255, 153, 64) };
	const int										POINTS_PER_ENEMY_SUB { 300 };
	const int                                       POINTS_PER_OXYGEN{ 99 };
	const int                                       POINTS_PER_DIVER{ 200 };

	void                                            init();
	void                                            onEnd() override;

	// systems
	void                                            sMovement(sf::Time dt);             // all entities movements
	void                                            sPlayerMovement();                  // player movements according to input keys pressed
	void                                            sViewMovement(sf::Time dt);         // moves the world view accordingly to player movements
	void                                            sAdjustPlayer();                    // keeps player in bounds
	void                                            sAdjustPlayerTexture();             // change player texture to face left and right properly
	void                                            sUpdateOxygenLevel(sf::Time dt);    // lower or raises oxygen level accordingly to submerging or getting to surface
	void                                            sDrawOxygenBar(sf::Color);          // draws Oxygen bar, diver count, and the game name on top left corner
	void                                            sDrawScore();                       // draws the score on the top right corner
	void                                            sCollisions();                    
	void											sRemoveEntitiesOutOfGame();			// destroy every entity out of the view

	// game helper functions
	void                                            diverLoad();                        // unload one diver from Sub
	void                                            diverUnload();                      // load one diver to Sub
	bool                                            isSubFullyLoaded();                 // true if sub is on its maximum capacity
	bool                                            isSubEmpty();                       // true if sub has no diver
	void                                            updateState();                      // update player state 
	void                                            updateScore();						// called when sub state is Pass_Lvl or SCORE
	void											createBullet(NttPtr ntt);			// create shots for entities
	bool											canShoot();							// player can shoot only if the last shoot left the screen
	void                                            lowOxygenWarning();                 // low oxygen warning
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
	void                                            spawnDivers();
	void                                            spawnEnemySubs();
	void											sGunUpdate(sf::Time dt);
	void                                            loadInitialTextures();
	void											drawVirtualLanes();
	bool											isNttInsideBounds(NttPtr ntt);
};
