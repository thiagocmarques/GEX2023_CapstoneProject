// New Brunswick Community College
// Gaming Experience Development
// -------------------------------
// Capstone Project - SeaQuest
// -------------------------------
// Instructor: David Burchill
// Student: Thiago Marques
// 
// April 2023
// 


#include "Scene_HighScores.h"
#include <fstream>
#include "SoundPlayer.h"
#include <sstream>
#include <locale>
#include <iomanip>

Scene_HighScores::Scene_HighScores(GameEngine* gameEngine) : Scene(gameEngine)
{
	init();
}


void Scene_HighScores::init()
{
    //loadScores();
    registerAction(sf::Keyboard::W, ActionName::UP);
    registerAction(sf::Keyboard::Up, ActionName::UP);

    registerAction(sf::Keyboard::S, ActionName::DOWN);
    registerAction(sf::Keyboard::Down, ActionName::DOWN);

    registerAction(sf::Keyboard::Enter, ActionName::ENTER);

    registerAction(sf::Keyboard::Escape, ActionName::BACK);
    registerAction(sf::Keyboard::Q, ActionName::BACK);

    m_title = "H i g h  S c o r e s";

    m_menuTitle.setFont(m_game->assets().getFont("SkyBridge"));
    m_menuText.setFont(m_game->assets().getFont("Magneon"));

    const size_t CHAR_SIZE{ 64 };
    m_menuText.setCharacterSize(CHAR_SIZE);
    m_menuTitle.setCharacterSize(CHAR_SIZE * 2);

}

void Scene_HighScores::onEnd()
{

}

void Scene_HighScores::drawHighScores() const
{

    auto viewCenter = m_game->getWindow().getView().getCenter();

    auto leftX = viewCenter.x - 500.f;
    auto centerX = viewCenter.x;
    auto rightX = viewCenter.x + 500.f;
    float lineHeight = 55.f;
    
    sf::Text playerName;
    sf::Text score;
    sf::Text timeStamp;

    // thats the offset from center according to Y axis
    // the center is zero / the title is 4 / and the last line will be -7
    int lineCount = 4;

    // FONT
    playerName.setFont(m_game->assets().getFont("SpecialElite"));
    score.setFont(m_game->assets().getFont("SpecialElite"));
    timeStamp.setFont(m_game->assets().getFont("SpecialElite"));

    // SIZE
    playerName.setCharacterSize(45);
    score.setCharacterSize(45);
    timeStamp.setCharacterSize(45);

    // FILL COLOUR
    playerName.setFillColor(sf::Color::Black);
    score.setFillColor(sf::Color::Black);
    timeStamp.setFillColor(sf::Color::Black);

    // OUTLINE COLOUR
    playerName.setOutlineColor(sf::Color::White);
    score.setOutlineColor(sf::Color::White);
    timeStamp.setOutlineColor(sf::Color::White);


    playerName.setString("PLAYER NAME");
    score.setString("SCORE");
    timeStamp.setString("DATE ACHIEVED");

    centerOrigin(playerName);
    centerOrigin(score);
    centerOrigin(timeStamp);

    playerName.setPosition(leftX, viewCenter.y - (lineCount * lineHeight) - 10.f);
    score.setPosition(centerX, viewCenter.y - (lineCount * lineHeight) - 10.f);
    timeStamp.setPosition(rightX, viewCenter.y - (lineCount * lineHeight) - 10.f);

    m_game->getWindow().draw(playerName);
    m_game->getWindow().draw(score);
    m_game->getWindow().draw(timeStamp);



    playerName.setCharacterSize(37);
    score.setCharacterSize(37);
    timeStamp.setCharacterSize(37);
    // looping through the vector to display all scores
    for (auto hscore : m_highScoreList.getHighScores()) {
        lineCount--;

        std::ostringstream oss;
        oss.imbue(std::locale("en_US.UTF-8"));
        oss << std::fixed << std::setprecision(0) << hscore.score;

        auto scoreTxt = oss.str();

        playerName.setString(hscore.name);
        score.setString(scoreTxt);
        timeStamp.setString(hscore.timestamp);

        oss.str("");

        centerOrigin(playerName);
        centerOrigin(score);
        centerOrigin(timeStamp);

        playerName.setPosition(leftX, viewCenter.y - (lineCount * lineHeight));
        score.setPosition(centerX, viewCenter.y - (lineCount * lineHeight));
        timeStamp.setPosition(rightX, viewCenter.y - (lineCount * lineHeight));

        m_game->getWindow().draw(playerName);
        m_game->getWindow().draw(score);
        m_game->getWindow().draw(timeStamp);
    }




}


void Scene_HighScores::registerItem(MenuItem key, std::string item)
{
    m_menuItems.push_back(std::make_pair(key, item));
}

void Scene_HighScores::update(sf::Time dt)
{
    m_entityManager.update();
}

void Scene_HighScores::sDoAction(const Action& action)
{
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

            switch (m_menuItems.at(m_menuIndex).first) {
            case MenuItem::MENU:
                m_game->backLevel();
                break;

            case MenuItem::SETT:
                m_game->changeScene(getSceneInMenu(m_menuItems.at(m_menuIndex).first));
                break;
            }
        }

        // joystick inputs
        else if (action.getName() == ActionName::JOYSTICK_FIRE) {
            auto newAction = Action(ActionName::ENTER, ActionType::START);
            sDoAction(newAction);
        }
        else if (action.getName() == ActionName::JOYSTICK_BACK) {
            m_game->backLevel();
        }
        else if (action.getName() == ActionName::JOYSTICK_MOVE) {
            ActionName newActionName = ActionName::NONE;

            // if analog joystick is pointing down
            if (action.getPos().y > 0) {
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
            else if (action.getPos().y < 0) {
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

void Scene_HighScores::sRender()
{
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
    auto titlePosY = 88;
    m_menuTitle.setPosition(titlePosX, titlePosY);

    m_game->getWindow().draw(m_menuTitle);
    // Menu title -------------------------------------------------


    // High Scores ------------------------------------------------
    m_highScoreList.loadFromFile();
    drawHighScores();



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
        m_menuText.setPosition(textPosX, windowSize.y - 155.f);

        m_game->getWindow().draw(m_menuText);
    }
    // Menu text --------------------------------------------------



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

    author.setFillColor(sf::Color{ 0, 117, 153 }); // same colour as NBCC text in logo

    auto authorSize = author.getLocalBounds();
    auto authorPosX = nbcc.getLocalBounds().width + 5.f;
    auto authorPosY = nbcc.getPosition().y + 10.f;

    author.setPosition(authorPosX, authorPosY);

    m_game->getWindow().draw(author);
    // Project info -----------------------------------------------
}

void Scene_HighScores::sPassTextEnteredEvent(sf::Event event)
{
}

SceneID Scene_HighScores::getSceneInMenu(MenuItem m)
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
