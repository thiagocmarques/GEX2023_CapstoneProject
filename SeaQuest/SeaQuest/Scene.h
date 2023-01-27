#pragma once

#include "EntityManager.h"
#include "GameEngine.h"
#include "Action.h"
#include <SFML/System/Time.hpp>
#include <map>
#include <string>
#include <memory>

using ActionMap             = std::map<int, std::string>;

// forward declaration
class GameEngine;

class Scene
{

protected:

    GameEngine*             m_game;
    EntityManager	        m_entityManager;
    ActionMap		        m_actions;
    bool			        m_isPaused{ false };
    bool			        m_hasEnded{ false };
    size_t			        m_currentFrame{ 0 };

    virtual void	        onEnd() = 0;
    void			        setPaused(bool paused);

public:
    explicit Scene(GameEngine* gameEngine);

    virtual void		    update(sf::Time dt) = 0;
    virtual void		    sDoAction(const Action& action) = 0;
    virtual void		    sRender() = 0;

    void				    doAction(Action);
    void				    registerAction(int, std::string);
    const ActionMap		    getActionMap() const;
};

