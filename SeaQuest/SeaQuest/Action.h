#pragma once
#include <string>
#include <SFML/System/Vector2.hpp>

class Action {

public:

    std::string             m_name{ "none" };
    std::string             m_type{ "none" };
    sf::Vector2f            m_pos{ 0.f, 0.f };
    
    Action() = default;
    Action( const std::string& name, 
            const std::string& type, 
            sf::Vector2f pos = sf::Vector2f(0.f, 0.f)
    );

    const std::string&      getName() const;
    const std::string&      getType() const;
    const sf::Vector2f&     getPos() const;

    std::string             toString() const;
};

