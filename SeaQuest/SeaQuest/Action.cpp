#include "Action.h"

//Action::Action(const std::string& name, const std::string& type, sf::Vector2f pos)
Action::Action(const ActionName& name, const ActionType& type, sf::Vector2f pos)
	: m_name(name)
	, m_type(type)
	, m_pos(pos)
{
}

//const std::string& Action::getName() const
const ActionName& Action::getName() const
{
	return m_name;
}

//const std::string& Action::getType() const
const ActionType& Action::getType() const
{
	return m_type;
}

const sf::Vector2f& Action::getPos() const
{
	return m_pos;
}

std::string Action::toString() const
{
	std::string name;
	std::string type;
	switch (m_name) {
	case ActionName::PAUSE:
		name = "Pause";
		break;
	case ActionName::QUIT:
		name = "Quit";
		break;
	case ActionName::BACK:
		name = "Back";
		break;
	case ActionName::DOWN:
		name = "Down";
		break;
	case ActionName::FIRE:
		name = "Fire";
		break;
	case ActionName::LEFT:
		name = "Left";
		break;
	case ActionName::RIGHT:
		name = "Right";
		break;
	case ActionName::TOGGLE_COLLISION:
		name = "Toggle Collision";
		break;
	case ActionName::TOGGLE_GRID:
		name = "Toggle Grid";
		break;
	case ActionName::TOGGLE_TEXTURE:
		name = "Toggle Texture";
		break;
	case ActionName::UP:
		name = "Up";
		break;
	case ActionName::ENTER:
		name = "Enter";
		break;
	default:
		name = "None";
		break;
	}

	switch (m_type)
	{
	case ActionType::START:
		type = "Key Pressed";
		break;
	case ActionType::END:
		type = "Key Released";
		break;
	default:
		type = "None";
		break;
	}

	return name + ":" + type + "(" + std::to_string(m_pos.x) + ", " + std::to_string(m_pos.y) + ")";
}
