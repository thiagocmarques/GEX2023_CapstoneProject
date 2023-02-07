#include "GameEngine.h"

#include "MusicPlayer.h"
#include "SoundPlayer.h"

#include <fstream>
#include <iostream>

#include "Scene_Menu.h"
#include "Scene_Settings.h"
#include "Scene_HighScores.h"
#include "Scene_Play.h"

const sf::Time GameEngine::TIME_PER_FRAME = sf::seconds((1.f / 60.f));

GameEngine::GameEngine(const std::string& configPath)
{
	init(configPath);

	m_assets.loadFromFile(configPath);
	m_window.create(sf::VideoMode(m_windowSize.x, m_windowSize.y), "SeaQuest");

	//m_statisticsText.setFont(m_assets.getFont("Arial"));
	//m_statisticsText.setPosition(15.0f, 15.0f);
	//m_statisticsText.setCharacterSize(15);

	createFactories();
	createMenu();
	createMenuSettings();
	createMenuHighScores();
	m_currentScene = SceneID::MENU;
	MusicPlayer::getInstance().play("ChillMusic");
}


void GameEngine::init(const std::string& configPath)
{
	std::ifstream config(configPath);
	if (config.fail()) {
		std::cerr << "Open file " << configPath << " failed\n";
		config.close();
		exit(1);
	}

	std::string token{ "" };
	config >> token;
	while (config) {
		if (token == "#") {
			std::string tmp;
			std::getline(config, tmp);
			std::cout << tmp << "\n";
		}
		else if (token == "Window") {
			config >> m_windowSize.x >> m_windowSize.y;
		}
		else if (token == "Sound") {
			std::string key;
			std::string path;
			config >> key >> path;

			SoundPlayer::getInstance().loadBuffer(key, path);
		}
		else if (token == "Music") {
			std::string key;
			std::string path;
			config >> key >> path;

			MusicPlayer::getInstance().loadMusicFilenames(key, path);
		}
		config >> token;
	}

	config.close();
}


void GameEngine::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate{ sf::Time::Zero };

	while (isRunning()) {

		sUserInput();

		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TIME_PER_FRAME) {
			timeSinceLastUpdate -= TIME_PER_FRAME;

			sUserInput();
			currentScene()->update(TIME_PER_FRAME);				// update world
		}

		//updateStatistics(elapsedTime);
		currentScene()->sRender();

		m_window.setView(m_window.getDefaultView());
		//m_window.draw(m_statisticsText);
		m_window.display();
	}
}


void GameEngine::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			quitGame();

		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{
			if (currentScene()->getActionMap().contains(event.key.code))
			{
				std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
				currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
			}
		}
	}
}


void GameEngine::quitGame() {
	m_window.close();
}

void GameEngine::changeScene(SceneID id, bool endCurrentScene)
{
	if (id == SceneID::NONE)
		return;

	if (endCurrentScene) {
		m_sceneMap.erase(m_currentScene);
		SoundPlayer::getInstance().stopAll();
		SoundPlayer::getInstance().removeStoppedSounds();
	}

	if (!m_sceneMap.contains(id))
		m_sceneMap[id] = m_factories[id]();

	m_lastScene = m_currentScene;
	m_currentScene = id;

	changeMusic();
}

sf::RenderWindow& GameEngine::getWindow()
{
	return m_window;
}

void GameEngine::quitLevel()
{
	changeScene(SceneID::MENU, true);
}

void GameEngine::backLevel()
{
	changeScene(SceneID::MENU, false);
}

const Assets& GameEngine::assets() const
{
	return m_assets;
}

Sptr GameEngine::currentScene()
{
	return m_sceneMap.at(m_currentScene);
}

void GameEngine::createFactories()
{
	m_factories[SceneID::MENU] = std::function<Sptr()>(
		[this]() -> Sptr {
			return std::make_shared<Scene_Menu>(this);
		});

	m_factories[SceneID::SETT] = std::function<Sptr()>(
		[this]() -> Sptr {
			return std::make_shared<Scene_Settings>(this);
		});
	m_factories[SceneID::HIGHSCR] = std::function<Sptr()>(
		[this]() -> Sptr {
			return std::make_shared<Scene_HighScores>(this);
		});
	
	m_factories[SceneID::PLAY] = std::function<Sptr()>(
		[this]() -> Sptr {
			return  std::make_shared<Scene_Play>(this);
		});
}

void GameEngine::createMenu()
{
	// create the menu_scene and put in sceneMap
	auto menuScene = std::make_shared<Scene_Menu>(this);
	m_sceneMap[SceneID::MENU] = menuScene;

	// add items to menu_scene
	menuScene->registerItem(SceneID::PLAY, "PLAY");
	menuScene->registerItem(SceneID::HIGHSCR, "HIGH SCORES");
	menuScene->registerItem(SceneID::SETT, "SETTINGS");
	menuScene->registerItem(SceneID::QUIT, "QUIT");
}

void GameEngine::createMenuSettings()
{
	// create the scene_settings and put in sceneMap
	auto menuSettings = std::make_shared<Scene_Settings>(this);
	m_sceneMap[SceneID::SETT] = menuSettings;

	// add items to scene_settings
	menuSettings->registerItem(SceneID::SOUND, "Toggle Sound Effects: ");
	menuSettings->registerItem(SceneID::MUSIC, "Toggle Music: ");
	menuSettings->registerItem(SceneID::MENU, "Back");
}

void GameEngine::createMenuHighScores()
{
	// create the scene_settings and put in sceneMap
	auto menuHighScores = std::make_shared<Scene_HighScores>(this);
	m_sceneMap[SceneID::HIGHSCR] = menuHighScores;

	// add items to scene_settings
	menuHighScores->registerItem(SceneID::SETT, "Settings");
	menuHighScores->registerItem(SceneID::MENU, "Back");
}

void GameEngine::changeMusic()
{
	// all menus, including Settings and High Scores have the same background music

	// Going to the Play the Game FROM MENU
	if ((m_currentScene == SceneID::PLAY) && (m_lastScene != SceneID::PLAY)) {
		MusicPlayer::getInstance().stop();
		MusicPlayer::getInstance().play("SciFiTheme");
	}

	// Going back to MENU FROM the game play
	if ((m_currentScene != SceneID::PLAY) && (m_lastScene == SceneID::PLAY)) {
		MusicPlayer::getInstance().stop();
		MusicPlayer::getInstance().play("ChillMusic");
	}
}


bool GameEngine::isRunning()
{
	return (m_isRunning && m_window.isOpen());
}