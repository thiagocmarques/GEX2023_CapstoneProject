#include "Action.h"

Action::Action(const std::string& name, const std::string& type, sf::Vector2f pos)
    : m_name(name)
    , m_type(type)
    , m_pos(pos)
{
}

const std::string& Action::getName() const
{
    return m_name;
}

const std::string& Action::getType() const
{
    return m_type;
}

const sf::Vector2f& Action::getPos() const
{
    return m_pos;
}

std::string Action::toString() const
{
    return m_name + ":" + m_type + "(" + std::to_string(m_pos.x) + ", " + std::to_string(m_pos.y) + ")";
}
