#include "GameEngine.h"

#include "MusicPlayer.h"
#include "SoundPlayer.h"

#include <fstream>
#include <iostream>

const sf::Time GameEngine::TIME_PER_FRAME = sf::seconds((1.f / 60.f));

GameEngine::GameEngine(const std::string& configPath)
{
	init(configPath);

	//m_assets.loadFromFile(configPath);
	m_window.create(sf::VideoMode(m_windowSize.x, m_windowSize.y), "SeaQuest");

	//m_statisticsText.setFont(m_assets.getFont("Arial"));
	//m_statisticsText.setPosition(15.0f, 15.0f);
	//m_statisticsText.setCharacterSize(15);

	//createFactories();
	//createMenu();
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

		//sUserInput();

		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TIME_PER_FRAME) {
			timeSinceLastUpdate -= TIME_PER_FRAME;

			//sUserInput();
			//currentScene()->update(TIME_PER_FRAME);				// update world
		}

		//updateStatistics(elapsedTime);
		//currentScene()->sRender();

		m_window.setView(m_window.getDefaultView());
		//m_window.draw(m_statisticsText);
		m_window.display();
	}
}

bool GameEngine::isRunning()
{
	return (m_isRunning && m_window.isOpen());
}
