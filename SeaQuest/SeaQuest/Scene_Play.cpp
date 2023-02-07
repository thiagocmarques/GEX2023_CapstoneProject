
#include <SFML/Window/Keyboard.hpp>
#include "GameEngine.h"
#include "Scene_Play.h"
#include "SoundPlayer.h"
#include "MusicPlayer.h"
#include "Utilities.h"
#include "Entity.h"

#include <random>

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
	loadBackground();
	MusicPlayer::getInstance().play("SciFiTheme");
	registerActions();

	m_spawnPosition = sf::Vector2f(m_worldView.getSize().x / 2.f,
		m_worldBounds.height - m_worldView.getSize().y / 2.f);

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
	auto worldViewTop = m_worldView.getCenter().y - m_worldView.getSize().y / 2.f;
	auto worldViewBottom = m_worldView.getCenter().y + m_worldView.getSize().y / 2.f;
	auto pVel = m_player->getComponent<CTransform>().vel;
	auto pPos = m_player->getComponent<CTransform>().pos;

	auto middleScreen = m_worldView.getCenter().y;

	if (pVel.y == 0.0f) return;                                 // if player not moving (vertical)

	bool isSubmerging = (pVel.y > 0.0f ? true : false);
	if (worldViewTop <= 0.0f && !isSubmerging) return;                                 // if view is on the top 
	if (worldViewBottom >= m_worldBounds.height && isSubmerging) return;     // if view is on the bottom


	//std::cout << "y: " << pPos.y << "  middleScreen: " << middleScreen << "  worldViewTop: " << worldViewTop << "  worldViewBottom: " << worldViewBottom << "\n";

	if (pPos.y >= middleScreen && !isSubmerging) return;
	if (pPos.y <= middleScreen && isSubmerging) return;

	m_worldView.move(0.f, pVel.y * dt.asSeconds());
	
	/*if (isSubmerging)
		m_worldView.move(0.f, pVel.y * dt.asSeconds());
	else
		m_worldView.move(0.f, pVel.y * dt.asSeconds());*/

	//if (getViewBounds().top > 0)
		//m_worldView.move(0.f, m_scrollSpeed * dt.asSeconds() * -1);
}

void Scene_Play::sPlayerMovement()
{
	// no movement if player is dead
	if (m_player->hasComponent<CState>() && m_player->getComponent<CState>().state == "dead")
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

void Scene_Play::adjustPlayer()
{
	if (m_player->hasComponent<CTransform>() && m_player->hasComponent<CCollision>()) {
		auto vb = getViewBounds();

		auto& pos = m_player->getComponent<CTransform>().pos;
		auto cr = m_player->getComponent<CCollision>().radius;

		pos.x = std::max(pos.x, vb.left + cr);
		pos.x = std::min(pos.x, vb.left + vb.width - cr);
		//pos.y = std::max(pos.y, vb.top + cr);
		pos.y = std::max(pos.y, 696.f);
		pos.y = std::min(pos.y, vb.top + vb.height - cr);
		
	}
}

void Scene_Play::checkPlayerState()
{
	if (m_player->hasComponent<CState>()) {

		auto xVel = m_player->getComponent<CTransform>().vel.x;
		if (xVel == 0.0f)
			return;

		std::string newState = (xVel < 0.0f ? "left" : "right");

		auto& state = m_player->getComponent<CState>().state;
		if (state != "dead") {
			if (newState != state) { // only if the state has changed, change the animation
				state = newState;
				if (state == "left")
					m_player->getComponent<CSprite>().sprite.setTexture(m_game->assets().getTexture("Sub01L"));
				if (state == "right")
					m_player->getComponent<CSprite>().sprite.setTexture(m_game->assets().getTexture("Sub01R"));
			}
		}
	}
}



void Scene_Play::update(sf::Time dt)
{
	if (m_isPaused)
		return;

	m_entityManager.update();

	sViewMovement(dt);

	adjustPlayer();
	sMovement(dt);
	checkPlayerState();
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
	if (action.getType() == "START") {

		if (action.getName() == "PAUSE") { setPaused(!m_isPaused); }
		else if (action.getName() == "QUIT") { m_game->quitLevel(); }
		else if (action.getName() == "BACK") { m_game->backLevel(); }

		else if (action.getName() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.getName() == "TOGGLE_COLLISION") { m_drawAABB = !m_drawAABB; }
		else if (action.getName() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }

		// Player control
		else if (action.getName() == "LEFT") { m_player->getComponent<CInput>().left = true; }
		else if (action.getName() == "RIGHT") { m_player->getComponent<CInput>().right = true; }
		else if (action.getName() == "UP") { m_player->getComponent<CInput>().up = true; }
		else if (action.getName() == "DOWN") { m_player->getComponent<CInput>().down = true; }

		// firing weapons
		else if (action.getName() == "FIRE") { m_player->getComponent<CInput>().shoot = true; }

	}

	// on Key Release
	else if (action.getType() == "END") {
		if (action.getName() == "LEFT") { m_player->getComponent<CInput>().left = false; }
		else if (action.getName() == "RIGHT") { m_player->getComponent<CInput>().right = false; }
		else if (action.getName() == "UP") { m_player->getComponent<CInput>().up = false; }
		else if (action.getName() == "DOWN") { m_player->getComponent<CInput>().down = false; }
		else if (action.getName() == "FIRE") { m_player->getComponent<CInput>().shoot = false; }
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
	for (auto e : m_entityManager.getEntities("bkg")) {
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
		if (e->getComponent<CSprite>().has) {
			auto& tfm = e->getComponent<CTransform>();
			auto& sprite = e->getComponent<CSprite>().sprite;
			sprite.setPosition(tfm.pos);
			sprite.setRotation(tfm.rot);
			m_game->getWindow().draw(sprite);
		}
	}

	if (m_isPaused) {
		sf::Text paused("PAUSED", m_game->assets().getFont("Demiths"), 128);
		centerOrigin(paused);
		auto bounds = getViewBounds();
		paused.setPosition(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
		m_game->getWindow().draw(paused);
	}

	// draw bounding boxes
	if (m_drawAABB) {
		drawAABB();
	}

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
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");

	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");

	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::Left, "LEFT");

	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Right, "RIGHT");

	registerAction(sf::Keyboard::Space, "FIRE");
	registerAction(sf::Keyboard::LControl, "FIRE");

	registerAction(sf::Keyboard::Escape, "PAUSE");
	registerAction(sf::Keyboard::P, "PAUSE");

	registerAction(sf::Keyboard::Q, "QUIT");


	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");
}

void Scene_Play::spawnPlayer()
{
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CTransform>(
		m_spawnPosition,
		sf::Vector2f(0.f, 0.f),
		0, 0);

	m_player->addComponent<CSprite>(m_game->assets().getTexture("Sub01L"));
	m_player->addComponent<CCollision>(30);
	m_player->addComponent<CInput>();
	//m_player->addComponent<CMissiles>();
	//m_player->addComponent<CHealth>().hp = (int)config_playerMaxHP;
	//auto& gun = m_player->addComponent<CGun>();
	m_player->addComponent<CState>("left");
}

void Scene_Play::loadBackground()
{
	auto e = m_entityManager.addEntity("bkg");

	auto& sprite
		= e->addComponent<CSprite>(m_game->assets().getTexture("Sea")).sprite;

	sprite.setOrigin(0.f, 0.f);

	m_worldBounds.width = sprite.getLocalBounds().width;
	m_worldBounds.height = sprite.getLocalBounds().height;
}
