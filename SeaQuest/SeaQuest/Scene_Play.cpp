
#include <SFML/Window/Keyboard.hpp>
#include "GameEngine.h"
#include "Scene_Play.h"
#include "SoundPlayer.h"
#include "MusicPlayer.h"
#include "Utilities.h"
#include "Entity.h"

#include <random>
#include <sstream>

namespace {
	std::random_device rd;
	std::mt19937 rng(rd());
}

Scene_Play::Scene_Play(GameEngine* gameEngine)
	: Scene(gameEngine),
	m_worldView(gameEngine->getWindow().getDefaultView()),
	m_worldBounds({ 0, 0 }, { 0, 0 })
{
	init();

}

void Scene_Play::init()
{
	loadScenarios();
	MusicPlayer::getInstance().play("SciFiTheme");
	registerActions();

	//m_spawnPosition = sf::Vector2f(m_worldView.getSize().x / 2.f, m_worldBounds.height - m_worldView.getSize().y / 2.f);		// spawns at the bottom
	//m_spawnPosition = sf::Vector2f(m_worldBounds.width / 2.f,m_worldBounds.height / 2.f);										// spawns in the center of world
	m_spawnPosition = sf::Vector2f(m_worldBounds.width / 2.f, m_worldView.getSize().y / 2.f);
	
	
	m_worldView.setCenter(m_spawnPosition);

	spawnPlayer();
}

void Scene_Play::onEnd()
{
}

void Scene_Play::sMovement(sf::Time dt)
{
	sPlayerMovement();


	// move all objects
	for (auto e : m_entityManager.getEntities()) {
		if (e->hasComponent<CTransform>()) {
			auto& tfm = e->getComponent<CTransform>();

			tfm.pos += tfm.vel * dt.asSeconds();
			tfm.rot += tfm.rotVel * dt.asSeconds();

			//if (e->getTag() == "player" && tfm.pos.x > 0)
			//    std::cout << "playerPos: " << tfm.pos.x << ", " << tfm.pos.y << "\n";
		}
	}
}

void Scene_Play::sViewMovement(sf::Time dt)
{
	auto middleScreenPosY = m_worldView.getCenter().y;
	auto halfViewHeight = m_worldView.getSize().y / 2.f;

	auto viewTopPosLimit = middleScreenPosY - halfViewHeight;
	auto viewBottomPosLimit = middleScreenPosY + halfViewHeight + 20.f;
	
	auto pVel = m_player->getComponent<CTransform>().vel;
	auto pPos = m_player->getComponent<CTransform>().pos;


	if (pVel.y == 0.0f) return;															// if player not moving (vertical)

	bool isSubmerging = (pVel.y > 0.0f ? true : false);

	if (viewTopPosLimit <= 0.0f && !isSubmerging) return;                               // if view is on the top 
	if (viewBottomPosLimit >= (m_worldBounds.height) && isSubmerging) return;			// if view is on the bottom

	/*
		std::cout << "topPosLimit: " << viewTopPosLimit << " - bottomPosLimit: " << viewBottomPosLimit << "\n";
		std::cout << "wViewCenter: " << m_worldView.getCenter().x << "x" << m_worldView.getCenter().y << "\n";
		std::cout << "wBounds" << m_worldBounds.width << "x" << m_worldBounds.height << "\n";
		std::cout << "player Y: " << pPos.y << "\n";
	
	*/


	if (pPos.y >= middleScreenPosY && !isSubmerging) return;
	if (pPos.y <= middleScreenPosY && isSubmerging) return;

	auto viewNewY = pVel.y * dt.asSeconds();

	m_worldView.move(0.f, viewNewY);
	
}

void Scene_Play::sPlayerMovement()
{
	// no movement if player is dead
	if (m_player->hasComponent<CState>() && m_player->getComponent<CState>().state == State::DEAD)
		return;

	// player movement
	sf::Vector2f pv;
	auto& pInput = m_player->getComponent<CInput>();
	if (pInput.left) pv.x -= 1;
	if (pInput.right) pv.x += 1;
	if (pInput.up) pv.y -= 1;
	if (pInput.down) pv.y += 1;
	pv = normalize(pv);
	m_player->getComponent<CTransform>().vel = m_playerSpeed * pv;
	//auto xVel = m_player->getComponent<CTransform>().vel;
	//std::cout << "pv: " << pv.x << ", " << pv.y << "  vel: " << xVel.x << ", " << xVel.y << "\n";
}

void Scene_Play::sAdjustPlayer()
{
	if (m_player->hasComponent<CTransform>() && m_player->hasComponent<CCollision>()) {
		auto vb = getViewBounds();

		auto& pos = m_player->getComponent<CTransform>().pos;
		auto cr = m_player->getComponent<CCollision>().radius;

		pos.x = std::max(pos.x, vb.left + cr);
		pos.x = std::min(pos.x, vb.left + vb.width - cr);
		//pos.y = std::max(pos.y, vb.top + cr);
		pos.y = std::max(pos.y, 675.f);								// 675.f is the height of waves texture
		pos.y = std::min(pos.y, vb.top + vb.height - cr);
		
	}
}

void Scene_Play::sCheckPlayerState()
{
	if (m_player->hasComponent<CState>()) {

		auto pVel = m_player->getComponent<CTransform>().vel;
		if (pVel.x == 0.0f)
			return;

		// std::string newState = (pVel.x < 0.0f ? "left" : "right");
		State newState = (pVel.x < 0.0f ? State::LEFT : State::RIGHT);

		//auto& state = m_player->getComponent<CState>().state;
		auto& state = m_player->getComponent<CState>().state;
		if (state != State::DEAD) {
			if (newState != state) { // only if the state has changed, change the texture
				state = newState;
				if (state == State::LEFT)
					m_player->getComponent<CSprite>().sprite.setTexture(m_game->assets().getTexture("Sub01L"));
				if (state == State::RIGHT)
					m_player->getComponent<CSprite>().sprite.setTexture(m_game->assets().getTexture("Sub01R"));
			}
		}

	}
}

void Scene_Play::sDrawOxygenBar()
{
	float outterBarWidth = m_worldView.getSize().x / 3.f;
	float outterBarHeight = m_worldView.getSize().y / 20.f;
	float innerBarWidth = outterBarWidth;
	float innerBarHeight = outterBarHeight;

	sf::Color outterBarColor = sf::Color::Yellow;
	sf::Color outterBarOutlineColor = sf::Color::Black;
	sf::Color innerBarColor = sf::Color::Cyan;

	sf::RectangleShape outterBar;
	sf::RectangleShape innerBar;

	auto viewBounds = getViewBounds();
	auto posX = viewBounds.width / 2.f  - outterBarWidth / 2.f;
	auto posY = viewBounds.top + outterBarHeight * 0.8f ;				// 80% of bar Height

	outterBar.setPosition(posX, posY);
	outterBar.setFillColor(outterBarColor);
	outterBar.setSize(sf::Vector2f(outterBarWidth, outterBarHeight));
	outterBar.setOutlineColor(outterBarOutlineColor);
	outterBar.setOutlineThickness(3.f);

	innerBar.setPosition(posX, posY);
	innerBar.setFillColor(innerBarColor);

	// 
	auto oxygenLevel = m_player->getComponent<COxygen>().oxygenLvl;
	innerBarWidth = oxygenLevel / 100.f * outterBarWidth;

	innerBar.setSize(sf::Vector2f(innerBarWidth, innerBarHeight));

	m_game->getWindow().draw(outterBar);
	m_game->getWindow().draw(innerBar);

	// drawing the divers
	sf::Sprite diverIcon;
	diverIcon.setTexture(m_game->assets().getTexture("DiverMini"));
	auto diverCount = m_player->getComponent<CDivers>().diversCount;

	centerOrigin(diverIcon);
	auto diverBounds = diverIcon.getLocalBounds();

	auto gapY = 13.f;
	auto gapX = (outterBarWidth - (6.f * diverBounds.width)) / 5.f;

	auto diverPosX = posX + diverBounds.width / 2.f;
	auto diverPosY = posY + outterBarHeight + diverBounds.height / 2.f + gapY;

	for (size_t i = 0; i < diverCount; i++) {
		diverIcon.setPosition(diverPosX, diverPosY);
		m_game->getWindow().draw(diverIcon);


		diverPosX += diverBounds.width + gapX;
	}



	// WORD "Oxygen" beside the oxygen Bar
	sf::Text oxygenText("Oxygen", m_game->assets().getFont("SkyBridge"), outterBarHeight * 0.7f);  // font size = % of bar Height
	centerOrigin(oxygenText);
	
	// as the text has center origin, I need to calculate its positions based on its half height and half width
	auto textBounds= oxygenText.getLocalBounds();
	posX += (-15.f - textBounds.width / 2.f);
	posY = viewBounds.top + outterBarHeight * 0.8f + textBounds.height / 2.f;
	oxygenText.setPosition(posX, posY);
	oxygenText.setFillColor(sf::Color::Black);
	oxygenText.setOutlineColor(sf::Color::White);
	oxygenText.setOutlineThickness(2.f);

	m_game->getWindow().draw(oxygenText);


}

void Scene_Play::sDrawScore()
{
	if (m_player->hasComponent<CScore>()) {
		float scoreHeight = m_worldView.getSize().y / 20.f;
		std::stringstream scoreSstream;
		scoreSstream << m_player->getComponent<CScore>().score;
		auto score = scoreSstream.str();

		int zero_padding = 9 - score.length();
		for (int i = 0; i < zero_padding; i++) {
			score = '0' + score;
		}
		
		
		sf::Text scoreTxt(score, m_game->assets().getFont("SkyBridge"), scoreHeight * 0.9f);  // font size = 90% of scoreHeight
		scoreTxt.setLetterSpacing(4.f);
		centerOrigin(scoreTxt);

		// as the text has center origin, I need to calculate its positions based on its half height and half width
		auto textBounds = scoreTxt.getLocalBounds();
		auto viewBounds = getViewBounds();
		auto posX = m_worldView.getSize().x - textBounds.width / 2.f - 30.f;
		auto posY = viewBounds.top + scoreHeight * 0.8f + textBounds.height / 2.f;

		scoreTxt.setPosition(posX, posY);
		scoreTxt.setFillColor(sf::Color::Black);
		scoreTxt.setOutlineColor(sf::Color::White);
		scoreTxt.setOutlineThickness(2.f);

		m_game->getWindow().draw(scoreTxt);




		sf::Text titleTxt("SEAQUEST", m_game->assets().getFont("SkyBridge"), scoreHeight * 0.4f);  // font size = 90% of scoreHeight
		//scoreTxt.setLetterSpacing(4.f);
		centerOrigin(titleTxt);

		// as the text has center origin, I need to calculate its positions based on its half height and half width
		auto titleBounds = titleTxt.getLocalBounds();
		posX = titleBounds.width / 2.f + 30.f;
		posY = viewBounds.top + scoreHeight * 0.8f + titleBounds.height / 2.f;

		titleTxt.setPosition(posX, posY + titleBounds.height / 2.f);
		titleTxt.setFillColor(sf::Color::Black);
		titleTxt.setOutlineColor(sf::Color::White);
		titleTxt.setOutlineThickness(2.f);

		m_game->getWindow().draw(titleTxt);
	}
}

void Scene_Play::sUpdateOxygenLevel(sf::Time dt)
{
	if (m_player->hasComponent<COxygen>() && 
		m_player->hasComponent<CTransform>() && 
		m_player->hasComponent<CState>()) {


		auto pPosY = m_player->getComponent<CTransform>().pos.y;

		auto& cOxygen = m_player->getComponent<COxygen>();
		auto& isSubmerged = cOxygen.isSubmerged;
		auto& oxgnLvl = cOxygen.oxygenLvl;


		// checking if player is submerged
		isSubmerged = (pPosY <= 675.f ? false : true);

		if (isSubmerged)
			oxgnLvl -= cOxygen.drainRate * dt.asSeconds();
		else
			oxgnLvl += cOxygen.fillingRate * dt.asSeconds();

		oxgnLvl = std::min(oxgnLvl, 100.f);
		oxgnLvl = std::max(oxgnLvl, 0.f);
		
		if (oxgnLvl <= 0.f) {
			auto& playerState = m_player->getComponent<CState>().state;
			playerState = State::DEAD;
		}
	}
}



void Scene_Play::update(sf::Time dt)
{
	if (m_isPaused)
		return;

	m_entityManager.update();

	sViewMovement(dt);

	sAdjustPlayer();
	sMovement(dt);
	sCheckPlayerState();
	sUpdateOxygenLevel(dt);
	// sCollisions();
	// sGunUpdate(dt);
	// sAnimation(dt);
	// sGuideMissiles(dt);
	// sAutoPilot(dt);
	// spawnEnemies();
	// sRemoveEntitiesOutOfGame();
	// checkGameOver();
	SoundPlayer::getInstance().removeStoppedSounds();
}

void Scene_Play::sDoAction(const Action& action)
{
	// On Key Press
	if (action.getType() == ActionType::KEY_PRESSED) {

		if (action.getName() == ActionName::PAUSE) { setPaused(!m_isPaused); }
		else if (action.getName() == ActionName::QUIT) { m_game->quitLevel(); }
		else if (action.getName() == ActionName::BACK) { m_game->backLevel(); }

		else if (action.getName() == ActionName::TOGGLE_TEXTURE) { m_drawTextures = !m_drawTextures; }
		else if (action.getName() == ActionName::TOGGLE_COLLISION) { m_drawAABB = !m_drawAABB; }
		else if (action.getName() == ActionName::TOGGLE_GRID) { m_drawGrid = !m_drawGrid; }

		// Player control
		else if (action.getName() == ActionName::LEFT) { m_player->getComponent<CInput>().left = true; }
		else if (action.getName() == ActionName::RIGHT) { m_player->getComponent<CInput>().right = true; }
		else if (action.getName() == ActionName::UP) { m_player->getComponent<CInput>().up = true; }
		else if (action.getName() == ActionName::DOWN) { m_player->getComponent<CInput>().down = true; }

		// firing weapons
		else if (action.getName() == ActionName::FIRE) { m_player->getComponent<CInput>().shoot = true; }

		// testing
		else if (action.getName() == ActionName::TEST_DIVER_UP) { m_player->getComponent<CDivers>().diversCount += 1; }
		else if (action.getName() == ActionName::TEST_DIVER_DOWN) { m_player->getComponent<CDivers>().diversCount -= 1; }
		else if (action.getName() == ActionName::TEST_SCORE_UP) { m_player->getComponent<CScore>().score += 100; }
		else if (action.getName() == ActionName::TEST_SCORE_DOWN) { m_player->getComponent<CScore>().score -= 1; }

	}

	// on Key Release
	else if (action.getType() == ActionType::KEY_RELEASED) {
		if (action.getName() == ActionName::LEFT) { m_player->getComponent<CInput>().left = false; }
		else if (action.getName() == ActionName::RIGHT) { m_player->getComponent<CInput>().right = false; }
		else if (action.getName() == ActionName::UP) { m_player->getComponent<CInput>().up = false; }
		else if (action.getName() == ActionName::DOWN) { m_player->getComponent<CInput>().down = false; }
		else if (action.getName() == ActionName::FIRE) { m_player->getComponent<CInput>().shoot = false; }
	}
}

void Scene_Play::sRender()
{
	m_game->getWindow().setView(m_worldView);

	// draw world
	auto bgColor = sf::Color(100, 100, 255);
	if (m_isPaused)
		bgColor = sf::Color(150, 50, 255);

	m_game->getWindow().clear(bgColor);

	// draw bkg first
	for (auto e : m_entityManager.getEntities(EntityEnum::BACKGROUND)) {
		if (e->getComponent<CSprite>().has) {
			auto& sprite = e->getComponent<CSprite>().sprite;
			m_game->getWindow().draw(sprite);
		}
	}


	for (auto e : m_entityManager.getEntities()) {

		// draw all entities with Animations textures
		if (e->getComponent<CAnimation>().has) {
			auto& tfm = e->getComponent<CTransform>();
			auto& anim = e->getComponent<CAnimation>().animation;
			anim.getSprite().setPosition(tfm.pos);
			anim.getSprite().setRotation(tfm.rot);
			m_game->getWindow().draw(anim.getSprite());
		}

		// draw all entities with Sprite textures
		if (e->getComponent<CSprite>().has && 
			e->getTag() != EntityEnum::BACKGROUND && 
			e->getTag() != EntityEnum::FOREGROUND) 
		{
			auto& tfm = e->getComponent<CTransform>();
			auto& sprite = e->getComponent<CSprite>().sprite;
			sprite.setPosition(tfm.pos);
			sprite.setRotation(tfm.rot);
			m_game->getWindow().draw(sprite);
		}
	}

	// draw foreground last
	for (auto e : m_entityManager.getEntities(EntityEnum::FOREGROUND)) {
		if (e->getComponent<CSprite>().has) {
			auto& sprite = e->getComponent<CSprite>().sprite;
			m_game->getWindow().draw(sprite);
		}
	}

	if (m_isPaused) {
		sf::Text paused("PAUSED", m_game->assets().getFont("Magneon"), 128);
		centerOrigin(paused);
		paused.setFillColor(sf::Color::Black);
		paused.setOutlineThickness(2.f);
		paused.setOutlineColor(sf::Color::White);
		auto bounds = getViewBounds();
		paused.setPosition(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
		m_game->getWindow().draw(paused);
	}

	// draw bounding boxes
	if (m_drawAABB) {
		drawAABB();
	}

	sDrawOxygenBar();
	sDrawScore();

}

void Scene_Play::drawAABB()
{
	for (auto e : m_entityManager.getEntities()) {

		// draw the world
		if (e->getComponent<CCollision>().has) {

			auto& tfm = e->getComponent<CTransform>();
			auto cr = e->getComponent<CCollision>().radius;

			sf::CircleShape cir(cr);
			cir.setOutlineThickness(2);
			cir.setOutlineColor(sf::Color::Red);
			cir.setFillColor(sf::Color::Transparent);

			cir.setPosition(tfm.pos);
			centerOrigin(cir);

			m_game->getWindow().draw(cir);
		}
	}
}

sf::FloatRect Scene_Play::getViewBounds()
{
	sf::FloatRect bounds;
	bounds.left = m_worldView.getCenter().x - m_worldView.getSize().x / 2.f;
	bounds.top = m_worldView.getCenter().y - m_worldView.getSize().y / 2.f;
	bounds.width = m_worldView.getSize().x;
	bounds.height = m_worldView.getSize().y;
	return bounds;
}

void Scene_Play::registerActions()
{
	registerAction(sf::Keyboard::W, ActionName::UP);
	registerAction(sf::Keyboard::Up, ActionName::UP);

	registerAction(sf::Keyboard::S, ActionName::DOWN);
	registerAction(sf::Keyboard::Down, ActionName::DOWN);

	registerAction(sf::Keyboard::A, ActionName::LEFT);
	registerAction(sf::Keyboard::Left, ActionName::LEFT);

	registerAction(sf::Keyboard::D, ActionName::RIGHT);
	registerAction(sf::Keyboard::Right, ActionName::RIGHT);

	registerAction(sf::Keyboard::Space, ActionName::FIRE);
	registerAction(sf::Keyboard::LControl, ActionName::FIRE);

	registerAction(sf::Keyboard::Escape, ActionName::PAUSE);
	registerAction(sf::Keyboard::P, ActionName::PAUSE);

	registerAction(sf::Keyboard::Q, ActionName::QUIT);


	registerAction(sf::Keyboard::T, ActionName::TOGGLE_TEXTURE);
	registerAction(sf::Keyboard::C, ActionName::TOGGLE_COLLISION);
	registerAction(sf::Keyboard::G, ActionName::TOGGLE_GRID);

	//testing
	registerAction(sf::Keyboard::I, ActionName::TEST_DIVER_UP);
	registerAction(sf::Keyboard::O, ActionName::TEST_DIVER_DOWN);
	registerAction(sf::Keyboard::K, ActionName::TEST_SCORE_UP);
	registerAction(sf::Keyboard::L, ActionName::TEST_SCORE_DOWN);

}

void Scene_Play::spawnPlayer()
{
	m_player = m_entityManager.addEntity(EntityEnum::PLAYER);
	m_player->addComponent<CTransform>(
		m_spawnPosition,
		sf::Vector2f(0.f, 0.f),
		0, 0);

	m_player->addComponent<CSprite>(m_game->assets().getTexture("Sub01L"));
	auto playerWidth = m_player->getComponent<CSprite>().sprite.getLocalBounds().width;
	m_player->addComponent<CCollision>(playerWidth / 2.f * 0.85f);  // 85% of player texture width
	m_player->addComponent<CInput>();
	m_player->addComponent<COxygen>().oxygenLvl = 100.f;
	m_player->addComponent<CState>(State::LEFT);
	m_player->addComponent<CScore>().score = 0;
	m_player->addComponent<CDivers>();

	//auto& gun = m_player->addComponent<CGun>();
}

void Scene_Play::loadScenarios()
{
	auto e = m_entityManager.addEntity(EntityEnum::BACKGROUND);

	auto& sprite
		= e->addComponent<CSprite>(m_game->assets().getTexture("Sea")).sprite;

	sprite.setOrigin(0.f, 0.f);

	m_worldBounds.width = sprite.getLocalBounds().width;
	m_worldBounds.height = sprite.getLocalBounds().height;


	auto waves = m_entityManager.addEntity(EntityEnum::FOREGROUND);
	auto& wavesSprite
		= waves->addComponent<CSprite>(m_game->assets().getTexture("Waves")).sprite;
	wavesSprite.setOrigin(0.f, 0.f);

}
	