#include <SFML/Window/Keyboard.hpp>
#include "Scene_Menu.h"
#include "GameEngine.h"
#include <memory>
#include <fstream>
#include <iostream>



Scene_Menu::Scene_Menu(GameEngine* gameEngine)
    : Scene(gameEngine)
{
    init();
}


void Scene_Menu::init()
{

    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::Up, "UP");

    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::Down, "DOWN");

    registerAction(sf::Keyboard::Enter, "PLAY");

    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::Q, "QUIT");

    m_title = "SeaQuest";


    //m_menuText.setFont(m_game->assets().getFont("Demiths")); 
    m_menuText.setFont(m_game->assets().getFont("Magneon"));
    m_menuTitle.setFont(m_game->assets().getFont("MonaShark"));

    const size_t CHAR_SIZE{ 64 };
    m_menuText.setCharacterSize(CHAR_SIZE);
    m_menuTitle.setCharacterSize(CHAR_SIZE * 3);

}


void Scene_Menu::registerItem(SceneID key, std::string item) {
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


    // background image
    sf::Sprite bkg = sf::Sprite{ };
    bkg.setTexture(m_game->assets().getTexture("MenuBkg"));

    auto bkgPosY = bkg.getLocalBounds().height - windowSize.y;
    bkg.setOrigin(0.f, bkgPosY);
    m_game->getWindow().draw(bkg);


    // Menu title
    m_menuTitle.setFillColor(normalColor);
    m_menuTitle.setString(m_title);

    auto titleSize = m_menuTitle.getLocalBounds();
    auto titlePosX = windowSize.x / 2.f - titleSize.width / 2.f;
    auto titlePosY = 128;
    m_menuTitle.setPosition(titlePosX, titlePosY);

    m_game->getWindow().draw(m_menuTitle);

    // Menu text
    
    int titleOffSetY = titlePosY + titleSize.height / 2.f;

    for (size_t i{ 0 }; i < m_menuItems.size(); ++i)
    {
        m_menuText.setString(m_menuItems.at(i).second);
        m_menuText.setFillColor((i == m_menuIndex ? selectedColor : normalColor));
        //m_menuText.setPosition(titlePosX + 128, (titleOffSetY + 128) + (i + 1) * 96);
        
        auto textSize = m_menuText.getLocalBounds();
        auto textPosX = windowSize.x / 2.f - textSize.width / 2.f;
        m_menuText.setPosition(textPosX, (titleOffSetY + 128) + (i + 1) * 96);

        m_game->getWindow().draw(m_menuText);
    }


    // footer

    sf::Text footer("UP: W    DOWN: S   SELECT: ENTER    QUIT: ESC",
        m_game->assets().getFont("Demiths"),
        20);
    footer.setFillColor(normalColor);

    auto footerSize = footer.getLocalBounds();
    auto footerPosX = windowSize.x / 2.f - footerSize.width / 2.f;
    

    footer.setPosition(footerPosX, windowSize.y - 128);

    m_game->getWindow().draw(footer);


    // nbcc logo image
    sf::Sprite nbcc;
    nbcc.setTexture(m_game->assets().getTexture("NBCC"));

    auto nbccPosY = windowSize.y - 96.f;
    nbcc.setPosition(10.f, nbccPosY);
    
    m_game->getWindow().draw(nbcc);

    sf::Text copyright("Gaming Experience Development\nComprehensive Development Project\nThiago Marques\n2023",
        m_game->assets().getFont("SpecialElite"),
        15);
    footer.setFillColor(normalColor);

    auto copyrightSize = copyright.getLocalBounds();
    auto copyrightPosX = nbcc.getLocalBounds().width + 10.f + 5.f;
    auto copyrightPosY = nbcc.getPosition().y + 10.f;


    copyright.setPosition(copyrightPosX, copyrightPosY);

    m_game->getWindow().draw(copyright);

}


void Scene_Menu::sDoAction(const Action& action) {
    if (action.getType() == "START")
    {
        if (action.getName() == "UP")
        {
            m_menuIndex = (m_menuIndex + m_menuItems.size() - 1) % m_menuItems.size();
        }
        else if (action.getName() == "DOWN")
        {
            m_menuIndex = (m_menuIndex + 1) % m_menuItems.size();
        }
        // TODO generalize
        else if (action.getName() == "PLAY")
        {
            if (m_menuItems.at(m_menuIndex).first == SceneID::QUIT)
                onEnd();
            else
                m_game->changeScene(m_menuItems.at(m_menuIndex).first);
        }
        else if (action.getName() == "QUIT")
        {
            onEnd();
        }
    }
}



void Scene_Menu::onEnd()
{
    m_game->getWindow().close();
}