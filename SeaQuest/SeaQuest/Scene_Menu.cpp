#include <SFML/Window/Keyboard.hpp>
#include "Scene_Menu.h"
#include "GameEngine.h"
#include <memory>
#include <fstream>
#include <iostream>
#include "SoundPlayer.h"
#include "MusicPlayer.h"
#include "Utilities.h"


Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();

}


void Scene_Menu::init()
{

	registerAction(sf::Keyboard::W, ActionName::UP);
	registerAction(sf::Keyboard::Up, ActionName::UP);

	registerAction(sf::Keyboard::S, ActionName::DOWN);
	registerAction(sf::Keyboard::Down, ActionName::DOWN);

	registerAction(sf::Keyboard::Enter, ActionName::ENTER);

	registerAction(sf::Keyboard::Escape, ActionName::QUIT);
	registerAction(sf::Keyboard::Q, ActionName::QUIT);




	//m_title = "SeaQuest";
	m_title = "S e a Q u e s t";


	m_menuTitle.setFont(m_game->assets().getFont("SkyBridge"));
	m_menuText.setFont(m_game->assets().getFont("Magneon"));
	//m_menuText.setFont(m_game->assets().getFont("Demiths")); 
	//m_menuText.setFont(m_game->assets().getFont("MonaShark"));

	const size_t CHAR_SIZE{ 64 };
	m_menuText.setCharacterSize(CHAR_SIZE);
	m_menuTitle.setCharacterSize(CHAR_SIZE * 3);

}


void Scene_Menu::registerItem(MenuItem key, std::string item) {
	m_menuItems.push_back(std::make_pair(key, item));
}


void Scene_Menu::update(sf::Time dt) {
	m_entityManager.update();
}


void Scene_Menu::sRender() {
	static const sf::Color selectedColor(255, 255, 255);
	static const sf::Color normalColor(0, 0, 0);
	static const sf::Color backgroundColor(100, 100, 255);

	m_game->getWindow().clear(backgroundColor);

	sf::View view = m_game->getWindow().getView();
	auto windowSize = m_game->getWindow().getSize();
	view.setCenter(windowSize.x / 2.f, windowSize.y / 2.f);
	m_game->getWindow().setView(view);


	// background image -------------------------------------------
	sf::Sprite bkg = sf::Sprite{ };
	bkg.setTexture(m_game->assets().getTexture("MenuBkg"));

	auto bkgPosY = bkg.getLocalBounds().height - windowSize.y;
	bkg.setOrigin(0.f, bkgPosY);
	m_game->getWindow().draw(bkg);
	// background image -------------------------------------------


	// Menu title -------------------------------------------------
	m_menuTitle.setFillColor(normalColor);
	m_menuTitle.setOutlineColor(selectedColor);
	m_menuTitle.setOutlineThickness(1.5f);
	m_menuTitle.setString(m_title);

	auto titleSize = m_menuTitle.getLocalBounds();
	auto titlePosX = windowSize.x / 2.f - titleSize.width / 2.f;
	auto titlePosY = 128;
	m_menuTitle.setPosition(titlePosX, titlePosY);

	m_game->getWindow().draw(m_menuTitle);
	// Menu title -------------------------------------------------


	// Menu text --------------------------------------------------
	int titleOffSetY = titlePosY + titleSize.height / 2.f;

	for (size_t i{ 0 }; i < m_menuItems.size(); ++i)
	{
		m_menuText.setString(m_menuItems.at(i).second);
		m_menuText.setFillColor((i == m_menuIndex ? selectedColor : normalColor));
		m_menuText.setOutlineColor((i != m_menuIndex ? selectedColor : normalColor));
		m_menuText.setOutlineThickness(1.0f);
		//m_menuText.setPosition(titlePosX + 128, (titleOffSetY + 128) + (i + 1) * 96);

		auto textSize = m_menuText.getLocalBounds();
		auto textPosX = windowSize.x / 2.f - textSize.width / 2.f;
		m_menuText.setPosition(textPosX, (titleOffSetY + 192) + (i + 1) * 96);

		m_game->getWindow().draw(m_menuText);
	}
	// Menu text --------------------------------------------------

	// Footer -----------------------------------------------------
	sf::Text footer("UP: W    DOWN: S   SELECT: ENTER    QUIT: ESC",
		m_game->assets().getFont("Demiths"),
		20);
	footer.setFillColor(normalColor);

	auto footerSize = footer.getLocalBounds();
	auto footerPosX = windowSize.x / 2.f - footerSize.width / 2.f;

	footer.setPosition(footerPosX, windowSize.y - 96);

	m_game->getWindow().draw(footer);
	// Footer -----------------------------------------------------



	// NBCC logo --------------------------------------------------
	sf::Sprite nbcc;
	nbcc.setTexture(m_game->assets().getTexture("NBCC"));

	auto nbccPosY = windowSize.y - 112.f;
	nbcc.setPosition(10.f, nbccPosY);

	m_game->getWindow().draw(nbcc);
	// NBCC logo --------------------------------------------------


	// Project info -----------------------------------------------
	sf::Text author("Gaming Experience Development\nComprehensive Development Project\nThiago Marques\n2023",
		m_game->assets().getFont("SpecialElite"),
		18);

	author.setFillColor(sf::Color{ 0, 117, 153 });  // same colour as NBCC text in logo

	auto authorSize = author.getLocalBounds();
	auto authorPosX = nbcc.getLocalBounds().width + 5.f;
	auto authorPosY = nbcc.getPosition().y + 10.f;

	author.setPosition(authorPosX, authorPosY);

	m_game->getWindow().draw(author);
	// Project info -----------------------------------------------

}

void Scene_Menu::sReceiveEvent(sf::Event event)
{
}

SceneID Scene_Menu::getSceneInMenu(MenuItem m)
{
	switch (m) {
	case MenuItem::PLAY:
		return SceneID::PLAY;
	case MenuItem::SETT:
		return SceneID::SETT;
	case MenuItem::HIGHSCR:
		return SceneID::HIGHSCR;
	default:
		return SceneID::MENU;
	}
}


void Scene_Menu::sDoAction(const Action& action) {
	SoundPlayer::getInstance().removeStoppedSounds();

	if (action.getType() == ActionType::START)
	{
		if (action.getName() == ActionName::UP)
		{
			m_menuIndex = (m_menuIndex + m_menuItems.size() - 1) % m_menuItems.size();
			SoundPlayer::getInstance().play("MenuClick");
		}
		else if (action.getName() == ActionName::DOWN)
		{
			m_menuIndex = (m_menuIndex + 1) % m_menuItems.size();
			SoundPlayer::getInstance().play("MenuClick");
		}
		// TODO generalize
		else if (action.getName() == ActionName::ENTER)
		{
			SoundPlayer::getInstance().play("ButtonClick");

			if (m_menuItems.at(m_menuIndex).first == MenuItem::QUIT)
				onEnd();
			else
				m_game->changeScene(getSceneInMenu(m_menuItems.at(m_menuIndex).first));



		}
		else if (action.getName() == ActionName::QUIT)
		{
			onEnd();
		}

		// joystick inputs
		else if (action.getName() == ActionName::JOYSTICK_FIRE) {
			auto newAction = Action(ActionName::ENTER, ActionType::START);
			sDoAction(newAction);
		}

		else if (action.getName() == ActionName::JOYSTICK_QUIT) {
			onEnd();
		}
		else if (action.getName() == ActionName::JOYSTICK_MOVE) {
			ActionName newActionName = ActionName::NONE;

			// if analog joystick is pointing down
			if (action.getPos().y > 0){
				// if in the previous action the analog was not pointing down
				if (m_lastAction != ActionName::DOWN) {
					newActionName = ActionName::DOWN;
					m_lastAction = ActionName::DOWN;
					m_repeatingActionDelayCount = 0;
				}
				// if it is a repeating analog movement, we force a delay before sending another DOWN action
				else if (++m_repeatingActionDelayCount > REPEAT_ACTION_DEFAULT_DELAY) {
					m_repeatingActionDelayCount = 0; 
					newActionName = ActionName::DOWN;
					m_lastAction = ActionName::DOWN;
				}
			}
			// if analog joystick is pointing UP
			else if (action.getPos().y < 0){
				// if in the previous action the analog was NOT pointing UP
				if (m_lastAction != ActionName::UP) {
					newActionName = ActionName::UP;
					m_lastAction = ActionName::UP;
					m_repeatingActionDelayCount = 0;
				}
				// if it is a repeating analog movement, we force a delay before sending another DOWN action
				else if (++m_repeatingActionDelayCount > REPEAT_ACTION_DEFAULT_DELAY) {

					m_repeatingActionDelayCount = 0;
					newActionName = ActionName::UP;
					m_lastAction = ActionName::UP;
				}
			}

			if (newActionName != ActionName::NONE) {
				auto newAction = Action(newActionName, ActionType::START);
				sDoAction(newAction);
			}
		}
	}
}



void Scene_Menu::onEnd()
{
	m_game->getWindow().close();
}