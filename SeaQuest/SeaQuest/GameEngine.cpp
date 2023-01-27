#include "GameEngine.h"

#include "MusicPlayer.h"
#include "SoundPlayer.h"

#include <fstream>
#include <iostream>

#include "Scene_Menu.h"

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
	m_currentScene = SceneID::MENU;
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
		else if (token == "Texture") {
			std::string name;
			std::string path;
			config >> name >> path;
			if ((name == "MenuBkg") or (name == "NBCC")) {
				m_assets.addTexture(name, path);
			}
;
		}
		config >> token;
	}

	config.close();
}


void GameEngine::run()
{
	//MusicPlayer::getInstance().play("menuTheme");

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

	m_currentScene = id;

	//changeMusic();
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

	//m_factories[SceneID::GEO] = std::function<Sptr()>(
	//	[this]() -> Sptr {
	//		return std::make_shared<Scene_Game>(this, "../assets/level1.txt");
	//	});
	//
	//m_factories[SceneID::FTR] = std::function<Sptr()>(
	//	[this]() -> Sptr {
	//		return  std::make_shared<Scene_GexFighter>(this, "../assets/gexFighters.txt");
	//	});
}

void GameEngine::createMenu()
{
	// create the menu_scene and put in sceneMap
	auto menuScene = std::make_shared<Scene_Menu>(this);
	m_sceneMap[SceneID::MENU] = menuScene;

	// add items to menu_scene
	menuScene->registerItem(SceneID::NONE, "PLAY");
	menuScene->registerItem(SceneID::NONE, "HIGH SCORES");
	menuScene->registerItem(SceneID::NONE, "SETTINGS");
	menuScene->registerItem(SceneID::QUIT, "QUIT");
}

bool GameEngine::isRunning()
{
	return (m_isRunning && m_window.isOpen());
}