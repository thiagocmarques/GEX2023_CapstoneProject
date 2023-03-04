
#include <SFML/Window/Keyboard.hpp>
#include "GameEngine.h"
#include "Scene_Play.h"
#include "SoundPlayer.h"
#include "MusicPlayer.h"
#include "Utilities.h"
#include "Entity.h"
#include "Enums.h"

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
	loadInitialTextures();
	registerActions();
	MusicPlayer::getInstance().play("SciFiTheme");

	//m_spawnPosition = sf::Vector2f(m_worldView.getSize().x / 2.f, m_worldBounds.height - m_worldView.getSize().y / 2.f);		// spawns at the bottom
	//m_spawnPosition = sf::Vector2f(m_worldBounds.width / 2.f,m_worldBounds.height / 2.f);										// spawns in the center of world

	m_spawnPosition = sf::Vector2f(m_worldBounds.width / 2.f, m_worldView.getSize().y / 2.f + 70.f);
	virtualLaneHeight = (m_worldBounds.height - MIN_Y_POSITION - virtualLaneGap) / (float)virtualLaneCount;
	restartGame();
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
	static float yVel{ 0.f };

	auto middleScreenPosY = m_worldView.getCenter().y;
	auto halfViewHeight = m_worldView.getSize().y / 2.f;

	auto viewTopPosLimit = middleScreenPosY - halfViewHeight;
	auto viewBottomPosLimit = middleScreenPosY + halfViewHeight + 20.f;

	if (m_isGameOver) {
		auto isOnBottom = (viewBottomPosLimit >= (m_worldBounds.height));
		auto isOnTop = (viewTopPosLimit <= 0.0f);

		if ((yVel > 0 && isOnBottom) || (yVel < 0 && isOnTop))
			yVel *= -1.f;

		m_worldView.move(0.f, yVel * dt.asSeconds());

	}
	else
	{



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

		if (m_player->getComponent<CState>().state == State::DEAD)
			yVel = pVel.y / 10.f;
	}
}

void Scene_Play::sPlayerMovement()
{
	// no movement if player is not on PLAYING State
	if (m_player->hasComponent<CState>() &&
		m_player->getComponent<CState>().state != State::PLAYING) {
		m_player->getComponent<CTransform>().vel = sf::Vector2f(0.f, 0.f);
		return;
	}


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
	if (pInput.shoot && canShoot())
		createBullet(m_player);
}

void Scene_Play::sAdjustPlayer()
{
	if (m_player->hasComponent<CTransform>()
		&& m_player->hasComponent<CCollision>()
		&& m_player->hasComponent<COxygen>()
		) {
		auto vb = getViewBounds();

		auto& pos = m_player->getComponent<CTransform>().pos;
		auto cr = m_player->getComponent<CCollision>().radius;
		float hLimit = 100.f;										// player cannot "touch" the screen horizontal borders

		pos.x = std::max(pos.x, vb.left + cr + hLimit);
		pos.x = std::min(pos.x, vb.left + vb.width - cr - hLimit);
		//pos.y = std::max(pos.y, vb.top + cr);
		pos.y = std::max(pos.y, MIN_Y_POSITION);					// 675.f is the height of waves texture
		pos.y = std::min(pos.y, vb.top + vb.height - cr);

		m_isOnSurface = (pos.y <= MIN_Y_POSITION);					// update isOnSurface attribute
	}
}

void Scene_Play::sAdjustPlayerTexture()
{
	if (m_player->hasComponent<CState>()
		&& m_player->hasComponent<CTransform>()) {

		auto playerVel = m_player->getComponent<CTransform>().vel;
		if (playerVel.x == 0.0f)
			return;

		// std::string newState = (pVel.x < 0.0f ? "left" : "right");
		bool isHeadingLeft = (playerVel.x < 0.0f ? true : false);
		auto playerState = m_player->getComponent<CState>();
		auto& tfmHeadingLeft = m_player->getComponent<CTransform>().headingLeft;

		if (playerState.state != State::DEAD) {
			if (isHeadingLeft != tfmHeadingLeft) { // change the texture only if the direction has changed 
				tfmHeadingLeft = isHeadingLeft;

				auto& pSprite = m_player->getComponent<CSprite>().sprite;
				std::string txture = "Sub01";
				if (tfmHeadingLeft)
					txture += "L";
				else
					txture += "R";

				pSprite.setTexture(m_game->assets().getTexture(txture));
			}
		}

	}
}

void Scene_Play::sAdjustSharkSwimming()
{
	for (auto& shark : m_entityManager.getEntities(EntityType::SHARK)) {
		auto currentLane = shark->getComponent<CTransform>().currentLane;
		auto initialYPos = MIN_Y_POSITION + currentLane * virtualLaneHeight;
		auto maxYSwimmingLength = shark->getComponent<CSprite>().sprite.getLocalBounds().height;

		auto& sharkYVel = shark->getComponent<CTransform>().vel.y;
		auto sharkYPos = shark->getComponent<CTransform>().pos.y;

		if ((sharkYPos < initialYPos - maxYSwimmingLength)
			|| (sharkYPos > initialYPos + maxYSwimmingLength))
			sharkYVel *= -1.f;
	}
}

void Scene_Play::sDrawOxygenBar(sf::Color innerBarColor)
{
	float outterBarWidth = m_worldView.getSize().x / 3.f;
	float outterBarHeight = m_worldView.getSize().y / 20.f;
	float innerBarWidth = outterBarWidth;
	float innerBarHeight = outterBarHeight;


	//sf::Color outterBarColor = sf::Color(0x0C486F);
	sf::Color outterBarColor = sf::Color(12, 72, 111, 100);
	sf::Color outterBarOutlineColor = sf::Color::White;
	//sf::Color innerBarColor = sf::Color(234, 87, 26);

	sf::RectangleShape outterBar;
	sf::RectangleShape innerBar;

	auto viewBounds = getViewBounds();
	auto posX = viewBounds.width / 2.f - outterBarWidth / 2.f;
	auto posY = viewBounds.top + outterBarHeight * 0.8f;				// 80% of bar Height

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

	// drawing the loaded diver count
	sf::Sprite diverIcon;
	diverIcon.setTexture(m_game->assets().getTexture("DiverMini"));
	auto diverCount = m_player->getComponent<CDivers>().diversCount;
	auto maxDivers = m_player->getComponent<CDivers>().MAX_DIVERS;

	centerOrigin(diverIcon);
	auto diverBounds = diverIcon.getLocalBounds();

	auto gapY = 13.f;
	auto gapX = (outterBarWidth - (maxDivers * diverBounds.width)) / (maxDivers - 1.f);

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
	auto textBounds = oxygenText.getLocalBounds();
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

	float scoreHeight = m_worldView.getSize().y / 20.f;
	std::stringstream scoreSstream;
	scoreSstream << gameScore;
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
	auto titlePosX = titleBounds.width / 2.f + 30.f;
	auto titlePosY = viewBounds.top + scoreHeight * 0.8f + titleBounds.height / 2.f;

	titleTxt.setPosition(titlePosX, titlePosY + titleBounds.height / 2.f);
	titleTxt.setFillColor(sf::Color::Black);
	titleTxt.setOutlineColor(sf::Color::White);
	titleTxt.setOutlineThickness(2.f);

	m_game->getWindow().draw(titleTxt);


	// drawing the life counter sub ico
	sf::Sprite subIcon;
	subIcon.setTexture(m_game->assets().getTexture("SubMini"));
	centerOrigin(subIcon);

	auto subPos = titleTxt.getPosition();
	subPos.x -= subIcon.getLocalBounds().width / 2.f;
	subPos.y += 50.f;

	subIcon.setPosition(subPos);
	m_game->getWindow().draw(subIcon);

	// drawing the life counter number
	std::stringstream extraLifeCounter;
	extraLifeCounter << "x " << m_extraLivesCount;
	sf::Text lifeCounterTxt(extraLifeCounter.str(), m_game->assets().getFont("SkyBridge"), scoreHeight * 0.5f);
	//scoreTxt.setLetterSpacing(4.f);
	centerOrigin(lifeCounterTxt);

	auto lifeCounterPos = subPos;
	lifeCounterPos.x += subIcon.getLocalBounds().width / 2.f + 25.f;

	lifeCounterTxt.setPosition(lifeCounterPos);
	lifeCounterTxt.setFillColor(sf::Color::White);
	lifeCounterTxt.setOutlineColor(sf::Color::Black);
	lifeCounterTxt.setOutlineThickness(1.f);

	m_game->getWindow().draw(lifeCounterTxt);



}

void Scene_Play::sCollisions()
{

	if (m_player->hasComponent<CCollision>()
		&& m_player->hasComponent<CTransform>()
		&& m_player->hasComponent<CState>()
		) {
		auto playerPos = m_player->getComponent<CTransform>().pos;
		auto playerCr = m_player->getComponent<CCollision>().radius;

		// collisions between player and divers
		if (!isSubFullyLoaded()) { // when sub is full, player doesn't collide with divers
			for (auto diver : m_entityManager.getEntities(EntityType::DIVER)) {
				if (diver->hasComponent<CTransform>()
					&& diver->hasComponent<CCollision>()) {

					auto diverPos = diver->getComponent<CTransform>().pos;
					auto diverCr = diver->getComponent<CCollision>().radius;

					if (dist(diverPos, playerPos) < (diverCr + playerCr)) {
						diverLoad();
						diver->destroy();
					}
				}
			}
		}
		// collisions between player and enemy_subs
		for (auto enemySub : m_entityManager.getEntities(EntityType::ENEMY_SUB)) {
			if (enemySub->hasComponent<CTransform>()
				&& enemySub->hasComponent<CCollision>()) {

				auto enemySubPos = enemySub->getComponent<CTransform>().pos;
				auto enemySubCr = enemySub->getComponent<CCollision>().radius;

				if (dist(enemySubPos, playerPos) < (enemySubCr + playerCr)) {
					m_player->getComponent<CState>().state = State::DEAD;
					enemySub->destroy();
				}
			}
		}

		// collisions between player and enemy_bullets
		for (auto enemyBullet : m_entityManager.getEntities(EntityType::ENEMY_BULLET)) {
			if (enemyBullet->hasComponent<CTransform>()
				&& enemyBullet->hasComponent<CCollision>()) {

				auto enemyBulPos = enemyBullet->getComponent<CTransform>().pos;
				auto enemyBulCr = enemyBullet->getComponent<CCollision>().radius;

				if (dist(enemyBulPos, playerPos) < (enemyBulCr + playerCr)) {
					m_player->getComponent<CState>().state = State::DEAD;
					enemyBullet->destroy();
				}
			}
		}

		// collisions between player and sharks
		for (auto shark : m_entityManager.getEntities(EntityType::SHARK)) {
			if (shark->hasComponent<CTransform>()
				&& shark->hasComponent<CCollision>()) {

				auto sharkPos = shark->getComponent<CTransform>().pos;
				auto sharkCr = shark->getComponent<CCollision>().radius;

				if (dist(sharkPos, playerPos) < (sharkCr + playerCr)) {
					m_player->getComponent<CState>().state = State::DEAD;
					shark->destroy();
				}
			}
		}

	}

	// collisions between player bullets and enemies
	for (auto pBullet : m_entityManager.getEntities(EntityType::PLAYER_BULLET)) {
		if (pBullet->hasComponent<CTransform>()
			&& pBullet->hasComponent<CCollision>()) {

			auto pBulletPos = pBullet->getComponent<CTransform>().pos;
			auto pBulletCr = pBullet->getComponent<CCollision>().radius;

			for (auto enemySub : m_entityManager.getEntities(EntityType::ENEMY_SUB)) {
				if (enemySub->hasComponent<CTransform>()
					&& enemySub->hasComponent<CCollision>()) {

					auto enemySubPos = enemySub->getComponent<CTransform>().pos;
					auto enemySubCr = enemySub->getComponent<CCollision>().radius;

					if (dist(enemySubPos, pBulletPos) < (enemySubCr + pBulletCr)) {
						gameScore += POINTS_PER_ENEMY_SUB;
						enemySub->destroy();
						pBullet->destroy();
						SoundPlayer::getInstance().play("KillEnemy");
					}
				}
			}

			for (auto shark : m_entityManager.getEntities(EntityType::SHARK)) {
				if (shark->hasComponent<CTransform>()
					&& shark->hasComponent<CCollision>()) {

					auto sharkPos = shark->getComponent<CTransform>().pos;
					auto sharkCr = shark->getComponent<CCollision>().radius;

					if (dist(sharkPos, pBulletPos) < (sharkCr + pBulletCr)) {
						gameScore += POINTS_PER_SHARK;
						shark->destroy();
						pBullet->destroy();
						SoundPlayer::getInstance().play("KillShark");
					}
				}
			}
		}
	}


}

void Scene_Play::sRemoveEntitiesOutOfGame()
{
	auto battlefield = m_worldBounds;
	float expandedArea = 200.f;
	battlefield.left -= expandedArea;
	battlefield.width += expandedArea * 2.f;
	float buffer = 100.f;

	for (auto e : m_entityManager.getEntities()) {
		if (e->hasComponent<CTransform>()) {
			auto pos = e->getComponent<CTransform>().pos;

			if (pos.x < (battlefield.left - buffer) ||
				pos.x >(battlefield.left + battlefield.width + buffer) ||
				pos.y < (battlefield.top - buffer) ||
				pos.y >(battlefield.top + battlefield.height + buffer)) {

				e->destroy();
			}
		}
	}
}

void Scene_Play::diverLoad()
{
	if (m_player->hasComponent<CDivers>()) {
		auto& diversCount = m_player->getComponent<CDivers>().diversCount;
		auto maxDivers = m_player->getComponent<CDivers>().MAX_DIVERS;

		if (diversCount < maxDivers) {
			diversCount++;
			if (diversCount == maxDivers)
				SoundPlayer::getInstance().play("LoadLastDiver");
			else
				SoundPlayer::getInstance().play("LoadDiver");

		}
	}
}

void Scene_Play::diverUnload()
{
	if (m_player->hasComponent<CDivers>()
		&& m_player->hasComponent<CState>()) {
		auto& diversCount = m_player->getComponent<CDivers>().diversCount;

		if (diversCount > 0) {
			diversCount--;
			//if (m_player->getComponent<CState>().state == State::UNLOAD)
			SoundPlayer::getInstance().play("UnloadDiver");
		}
	}
}

bool Scene_Play::isSubFullyLoaded()
{
	if (m_player->hasComponent<CDivers>()) {
		auto diversCount = m_player->getComponent<CDivers>().diversCount;
		auto maxDivers = m_player->getComponent<CDivers>().MAX_DIVERS;

		return diversCount == maxDivers;
	}
	return false;
}

bool Scene_Play::isSubEmpty()
{
	if (m_player->hasComponent<CDivers>()) {
		auto diversCount = m_player->getComponent<CDivers>().diversCount;

		return diversCount == 0;
	}
	return false;
}

void Scene_Play::sUpdateOxygenLevel(sf::Time dt)
{
	if (m_player->hasComponent<COxygen>()
		&& m_player->hasComponent<CState>()
		) {

		auto& playerState = m_player->getComponent<CState>().state;

		auto& cOxygen = m_player->getComponent<COxygen>();
		auto& oxgnLvl = cOxygen.oxygenLvl;

		if (playerState == State::PLAYING
			|| playerState == State::SPAWN
			|| playerState == State::REFILL) {

			if (m_isOnSurface)
				oxgnLvl += cOxygen.fillingRate * dt.asSeconds();
			else
				oxgnLvl -= cOxygen.drainRate * dt.asSeconds();

			oxgnLvl = std::min(oxgnLvl, MAX_OXGN_LVL);
			oxgnLvl = std::max(oxgnLvl, MIN_OXGN_LVL);

			if (oxgnLvl <= MIN_OXGN_LVL)
				playerState = State::DEAD;
		}
		else if (playerState == State::SCORE) {
			oxgnLvl -= cOxygen.fillingRate * dt.asSeconds();

			oxgnLvl = std::max(oxgnLvl, MIN_OXGN_LVL);

			if (oxgnLvl <= MIN_OXGN_LVL) {
				playerState = State::UNLOAD;
				SoundPlayer::getInstance().stopAll();
			}
		}
	}
}

void Scene_Play::updateState()
{
	if (m_player->hasComponent<COxygen>() &&
		m_player->hasComponent<CTransform>() &&
		m_player->hasComponent<CState>()) {

		// only update state if player is not DEAD
		if (m_player->getComponent<CState>().state != State::DEAD) {

			auto& pState = m_player->getComponent<CState>().state;
			auto  oxgnLvl = m_player->getComponent<COxygen>().oxygenLvl;

			// if player was draining oxygen then it was submerged;
			bool wasSubmerged = m_player->getComponent<COxygen>().isDrainingOxygen;

			// if player got to surface now
			if (m_isOnSurface && wasSubmerged) {
				if (isSubEmpty()) {
					pState = State::DEAD;
				}
				else {
					if (isSubFullyLoaded()) {
						//if (pState == State::PLAYING) 
						pState = State::PASS_LVL;
					}
					else {
						pState = State::REFILL;
					}
				}

				// TODO unload divers and give points per diver

				// play refill sound only when the player gets to the surface
				if (pState == State::REFILL)
					SoundPlayer::getInstance().play("OxygenRefill");
			}


			if ((pState == State::SPAWN)) {
				SoundPlayer::getInstance().play("OxygenRefill");
				pState = State::REFILL;
			}

			if (pState == State::REFILL) {
				if (oxgnLvl >= 100.f) {
					SoundPlayer::getInstance().stopAll();
					diverUnload();
					pState = State::PLAYING;
				}
				else if (oxgnLvl <= 1.f) {
					SoundPlayer::getInstance().play("OxygenRefill");
				}
			}

			// updating isDrainingOxygen attribute
			m_player->getComponent<COxygen>().isDrainingOxygen = !m_isOnSurface;

			if (pState == State::PASS_LVL || pState == State::SCORE || pState == State::UNLOAD)
				updateScore();
		}
	}
}

void Scene_Play::updateScore()
{
	if (m_player->hasComponent<CState>()
		&& m_player->hasComponent<CDivers>()
		&& m_player->hasComponent<COxygen>()
		) {

		static sf::Clock scoreClock;
		static sf::Time timeSinceLastRestart;

		auto& pState = m_player->getComponent<CState>().state;


		if (pState == State::PASS_LVL) {
			timeSinceLastRestart = sf::Time::Zero;
			pState = State::SCORE;
			scoreClock.restart();
			SoundPlayer::getInstance().play("DroppingOxygen");
		}
		else if (pState == State::SCORE) {
			sf::Time elapsedTime = scoreClock.restart();
			timeSinceLastRestart += elapsedTime;
			sf::Time soundLength = sf::seconds(DROP_OXGN_SOUND_LENGTH);

			if (timeSinceLastRestart > soundLength) {
				timeSinceLastRestart -= soundLength;
				SoundPlayer::getInstance().play("DroppingOxygen");
			}

			gameScore += POINTS_PER_OXYGEN;
		}
		else if (pState == State::UNLOAD) {
			sf::Time elapsedTime = scoreClock.restart();
			timeSinceLastRestart += elapsedTime;
			sf::Time soundLength = sf::seconds(UNLOAD_SOUND_LENGTH);

			if (timeSinceLastRestart > soundLength) {
				timeSinceLastRestart -= soundLength;
				diverUnload();
				gameScore += POINTS_PER_DIVER;
			}

			if (isSubEmpty())
				pState = State::REFILL;
		}
	}

}

void Scene_Play::createBullet(NttPtr ntt)
{

	if (ntt->hasComponent<CTransform>()
		&& ntt->hasComponent<CSprite>()) {

		auto facingLeft = ntt->getComponent<CTransform>().headingLeft;
		auto nttPos = ntt->getComponent<CTransform>().pos;

		EntityType tag = ntt->getTag() ==
			EntityType::PLAYER ? EntityType::PLAYER_BULLET : EntityType::ENEMY_BULLET;

		float xGap;
		float speed;
		
		std::string textureName;

		// only allows player bullets creation if the player state is PLAYING
		if (tag == EntityType::PLAYER_BULLET
			&& m_player->hasComponent<CState>()
			&& m_player->getComponent<CState>().state == State::PLAYING) {

			xGap = 36.f;
			nttPos.y += 15.f;
			SoundPlayer::getInstance().play("Shoot");

			speed = facingLeft ? -m_bulletSpeed : m_bulletSpeed;
			textureName = facingLeft ? "BulletL" : "BulletR";
		}
		else
		{
			xGap = 60.f;
			nttPos.y += 10.f;
			speed = ntt->getComponent<CTransform>().vel.x * 1.75f;
			textureName = "EnemyBullet" + std::to_string(ntt->getComponent<CGun>().baseBulletType);
			textureName += facingLeft ? "L" : "R";
		}

		nttPos.x = facingLeft ? nttPos.x - xGap : nttPos.x + xGap;
		auto bullet = m_entityManager.addEntity(tag);
		bullet->addComponent<CTransform>(nttPos, sf::Vector2f(speed, 0.f));
		bullet->addComponent<CSprite>(m_game->assets().getTexture(textureName));
		bullet->addComponent<CCollision>(BULLET_COLLISION_RADIUS);
	}
}

bool Scene_Play::canShoot()
{
	if (m_isOnSurface)
		return false;

	auto& pBullets = m_entityManager.getEntities(EntityType::PLAYER_BULLET);
	for (auto bul : pBullets) {
		auto bulPos = bul->getComponent<CTransform>().pos;
		if (bulPos.x <= m_worldBounds.width && bulPos.x >= 0.f)
			return false;
	}
	return true;
}

void Scene_Play::lowOxygenWarning()
{
	if (m_player->hasComponent<COxygen>()
		&& m_player->hasComponent<CState>()
		&& m_player->getComponent<CState>().state == State::PLAYING
		) {

		static sf::Clock oxygenClock;
		static sf::Time timeSinceLastRestart{ sf::Time::Zero };

		auto& cOxygen = m_player->getComponent<COxygen>();
		auto wasLowOxygen = cOxygen.isLowOxygen;

		cOxygen.isLowOxygen = (cOxygen.oxygenLvl <= LOW_OXYGEN_LVL && !m_isOnSurface);

		if (cOxygen.isLowOxygen && !wasLowOxygen) {
			timeSinceLastRestart = sf::Time::Zero;
			oxygenClock.restart();
			SoundPlayer::getInstance().play("LowOxygenWarn");
		}
		else if (cOxygen.isLowOxygen) {

			sf::Time elapsedTime = oxygenClock.restart();
			timeSinceLastRestart += elapsedTime;
			sf::Time soundLength = sf::seconds(LOW_OXGN_SOUND_LENGTH);

			if (timeSinceLastRestart > soundLength) {
				timeSinceLastRestart -= soundLength;
				SoundPlayer::getInstance().play("LowOxygenWarn");
			}
		}
	}
}

void Scene_Play::checkIfDead()
{
	static sf::Clock deadClock;
	static sf::Time	 timeSinceLastRestart;

	if (m_player->hasComponent<CState>()
		&& m_player->hasComponent<CTransform>()
		&& m_player->hasComponent<CSprite>()
		&& m_player->isActive()
		) {
		auto& pState = m_player->getComponent<CState>();
		auto& pTfm = m_player->getComponent<CTransform>();
		auto& pSprite = m_player->getComponent<CSprite>().sprite;

		if (pState.state == State::DEAD) {
			if (timeSinceLastRestart == sf::Time::Zero)
				SoundPlayer::getInstance().play("PlayerDies");

			sf::Time elapsedTime = deadClock.restart();
			timeSinceLastRestart += elapsedTime;

			if (timeSinceLastRestart < sf::seconds(PLR_DEAD_SOUND_LENGTH)) {
				std::uniform_int_distribution left(0, 2); // first 3 indexes are sub facing-left textures
				std::uniform_int_distribution right(3, 5); // last 3 indexes are sub facing-right textures
				if (pTfm.headingLeft)
					pSprite.setTexture(deadSubTxtreMap.at(left(rng)));
				else
					pSprite.setTexture(deadSubTxtreMap.at(right(rng)));
			}
			else {
				m_player->destroy();
				//std::cout << "Player Destroyed\n";
				deadClock.restart();
				timeSinceLastRestart = sf::Time::Zero;
			}
		}
		else // pState not equal DEAD
		{
			deadClock.restart();
			timeSinceLastRestart = sf::Time::Zero;
		}
	}
	else  // there is no player, which means it was destroyed
	{
		sf::Time elapsedDestroyedTime = deadClock.restart();
		timeSinceLastRestart += elapsedDestroyedTime;

		// a little delay before restarting the game
		if (timeSinceLastRestart > sf::seconds(DELAY_TO_RESTART)) {
			if (m_extraLivesCount > 0) {
				m_extraLivesCount--;
				restartGame();
			}
			else
			{
				m_isGameOver = true;
			}
		}

	}
}

void Scene_Play::restartGame()
{
	for (auto ntt : m_entityManager.getEntities()) {
		if (ntt->getTag() != EntityType::BACKGROUND
			&& ntt->getTag() != EntityType::FOREGROUND)
			ntt->destroy();
	}
	m_worldView.setCenter(m_spawnPosition);
	spawnPlayer();
}



void Scene_Play::update(sf::Time dt)
{
	if (m_isPaused)
		return;

	m_entityManager.update();

	sViewMovement(dt);

	sAdjustPlayer();
	sMovement(dt);
	sAdjustPlayerTexture();
	sAdjustSharkSwimming();
	sUpdateOxygenLevel(dt);
	lowOxygenWarning();
	updateState();
	if (m_player->getComponent<CState>().state == State::PLAYING || m_isGameOver) {
		spawnDivers();
		spawnEnemySubs();
		spawnSharks();
	}
	sCollisions();
	sGunUpdate(dt);
	// sAnimation(dt);
	// sGuideMissiles(dt);
	// sAutoPilot(dt);

	sRemoveEntitiesOutOfGame();

	if (!m_isGameOver) checkIfDead();
	// checkGameOver();
	SoundPlayer::getInstance().removeStoppedSounds();
}

void Scene_Play::sDoAction(const Action& action)
{
	// On Key Press
	if (action.getType() == ActionType::START) {

		if (action.getName() == ActionName::PAUSE) { if (!m_isGameOver) setPaused(!m_isPaused); }
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
		// else if (action.getName() == ActionName::TEST_DIVER_UP) { diverLoad(); }
		// else if (action.getName() == ActionName::TEST_DIVER_DOWN) { diverUnload(); }
		// else if (action.getName() == ActionName::TEST_SCORE_UP) { gameScore += 100; }
		// else if (action.getName() == ActionName::TEST_SCORE_DOWN) { gameScore -= 1; }

	}

	// on Key Release
	else if (action.getType() == ActionType::END) {
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
	auto bgColor = sf::Color(100, 100, 255, 255);
	if (m_isPaused)
		bgColor = sf::Color(150, 50, 255, 255);

	m_game->getWindow().clear(bgColor);

	// draw bkg first
	for (auto e : m_entityManager.getEntities(EntityType::BACKGROUND)) {
		if (e->getComponent<CSprite>().has) {
			auto& sprite = e->getComponent<CSprite>().sprite;
			m_game->getWindow().draw(sprite);
		}
	}


	for (auto e : m_entityManager.getEntities()) {

		// draw all entities with Animations textures
		if (e->hasComponent<CAnimation>()) {
			auto& tfm = e->getComponent<CTransform>();
			auto& anim = e->getComponent<CAnimation>().animation;
			anim.getSprite().setPosition(tfm.pos);
			anim.getSprite().setRotation(tfm.rot);
			m_game->getWindow().draw(anim.getSprite());
		}

		// draw all entities with Sprite textures
		if (e->hasComponent<CSprite>() &&
			e->getTag() != EntityType::BACKGROUND &&
			e->getTag() != EntityType::FOREGROUND)
		{
			auto& tfm = e->getComponent<CTransform>();
			auto& sprite = e->getComponent<CSprite>().sprite;
			sprite.setPosition(tfm.pos);
			sprite.setRotation(tfm.rot);
			m_game->getWindow().draw(sprite);
		}
		// draw all entities with Shape
		if (e->hasComponent<CShape>())
		{
			auto& tfm = e->getComponent<CTransform>();
			auto& shape = e->getComponent<CShape>().shape;
			shape.setPosition(tfm.pos);
			shape.setRotation(tfm.rot);
			m_game->getWindow().draw(shape);
		}
	}

	// draw foreground last
	for (auto e : m_entityManager.getEntities(EntityType::FOREGROUND)) {
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

	if (m_isGameOver) {
		sf::Text gameOver("Game Over", m_game->assets().getFont("Magneon"), 128);
		centerOrigin(gameOver);
		gameOver.setFillColor(sf::Color::Black);
		gameOver.setOutlineThickness(2.f);
		gameOver.setOutlineColor(sf::Color::White);
		auto bounds = getViewBounds();
		gameOver.setPosition(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
		m_game->getWindow().draw(gameOver);
	}

	// draw bounding boxes
	if (m_drawAABB) {
		drawAABB();
	}

	sf::Color innerOxBar = OXGN_BAR_COLOR;
	static sf::Color lastColor;
	if (m_player->getComponent<COxygen>().isLowOxygen) {

		if (lastColor == sf::Color::White) {
			innerOxBar = sf::Color::Red;
		}
		else {
			innerOxBar = sf::Color::White;
		}
		lastColor = innerOxBar;
	}

	sDrawOxygenBar(innerOxBar);
	sDrawScore();
	//	drawVirtualLanes();
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
	m_player = m_entityManager.addEntity(EntityType::PLAYER);
	m_player->addComponent<CTransform>(
		m_spawnPosition,
		sf::Vector2f(0.f, 0.f),
		0, 0);

	m_player->addComponent<CSprite>(m_game->assets().getTexture("Sub01L"));
	auto playerWidth = m_player->getComponent<CSprite>().sprite.getLocalBounds().width;
	m_player->addComponent<CCollision>(playerWidth / 2.f * 0.85f);	// 85% of player texture width
	m_player->addComponent<CInput>();
	m_player->addComponent<COxygen>().oxygenLvl = 1.f;
	m_player->addComponent<CState>(State::SPAWN);
	m_player->addComponent<CDivers>();
}

void Scene_Play::spawnDivers()
{
	std::uniform_int_distribution spawnTime(1, 1000);
	std::uniform_int_distribution direction(0, 1);
	std::uniform_int_distribution lane(1, virtualLaneCount);
	std::uniform_int_distribution texture(1, 2);

	auto doSpawn = spawnTime(rng) % 201 == 0;
	if (doSpawn) {
		auto headingLeft = direction(rng) == 0;
		auto xPos = headingLeft ? m_worldBounds.width + 100.f : -100.f;
		auto xSpeed = headingLeft ? -m_diverSpeed : m_diverSpeed;
		auto yPos = MIN_Y_POSITION + lane(rng) * virtualLaneHeight;

		auto vel = sf::Vector2f(xSpeed, 0.f);
		auto pos = sf::Vector2f(xPos, yPos);
		auto rot = 0.f;

		std::stringstream baseTextureName;
		baseTextureName << "Diver" << texture(rng);
		if (headingLeft) baseTextureName << 'L'; else baseTextureName << 'R';

		auto textureName = baseTextureName.str();

		auto diver = m_entityManager.addEntity(EntityType::DIVER);
		diver->addComponent<CTransform>(pos, vel, rot);
		diver->addComponent<CSprite>(m_game->assets().getTexture(textureName));
		diver->addComponent<CCollision>(50);
	}
}

void Scene_Play::spawnEnemySubs()
{
	std::uniform_int_distribution spawnTime(1, 1000);
	std::uniform_int_distribution direction(0, 1);
	std::uniform_int_distribution typeSub(1, 2);
	std::uniform_int_distribution typeBullet(1, 3);
	std::uniform_int_distribution lane(1, virtualLaneCount);

	auto doSpawn = spawnTime(rng) % 133 == 0;
	if (doSpawn) {
		auto headingLeft = direction(rng) == 0;
		auto xPos = headingLeft ? m_worldBounds.width + 100.f : -100.f;
		auto xSpeed = headingLeft ? -m_enemySubSpeed : m_enemySubSpeed;
		auto yPos = MIN_Y_POSITION + lane(rng) * virtualLaneHeight;

		auto vel = sf::Vector2f(xSpeed, 0.f);
		auto pos = sf::Vector2f(xPos, yPos);
		auto rot = 0.f;

		auto textureBaseName = "EnemySub";
		std::string textureName = textureBaseName + std::to_string(typeSub(rng));
		textureName += headingLeft ? "L" : "R";

		auto nmeSub = m_entityManager.addEntity(EntityType::ENEMY_SUB);
		nmeSub->addComponent<CTransform>(pos, vel, headingLeft);
		nmeSub->addComponent<CSprite>(m_game->assets().getTexture(textureName));
		nmeSub->addComponent<CCollision>(50);
		nmeSub->addComponent<CGun>(true);
		nmeSub->getComponent<CGun>().baseBulletType = typeBullet(rng);
	}
}

void Scene_Play::spawnSharks()
{
	std::uniform_int_distribution spawnTime(1, 1000);
	std::uniform_int_distribution direction(0, 1);
	std::uniform_int_distribution typeShark(1, 3);
	std::uniform_int_distribution lane(1, virtualLaneCount);

	auto doSpawn = spawnTime(rng) % 133 == 0;
	if (doSpawn) {
		auto headingLeft = direction(rng) == 0;
		auto xPos = headingLeft ? m_worldBounds.width + 100.f : -100.f;
		auto xSpeed = headingLeft ? -m_sharkSpeed : m_sharkSpeed;
		auto currentLane = lane(rng);
		auto yPos = MIN_Y_POSITION + currentLane * virtualLaneHeight;

		auto vel = sf::Vector2f(xSpeed, xSpeed / 3.f);
		auto pos = sf::Vector2f(xPos, yPos);
		auto rot = 0.f;

		auto textureBaseName = "Shark";
		std::string textureName = textureBaseName + std::to_string(typeShark(rng));
		textureName += headingLeft ? "L" : "R";

		auto shark = m_entityManager.addEntity(EntityType::SHARK);
		shark->addComponent<CTransform>(pos, vel, headingLeft);
		shark->addComponent<CSprite>(m_game->assets().getTexture(textureName));
		shark->addComponent<CCollision>(40);
		shark->getComponent<CTransform>().currentLane = currentLane;
	}
}

void Scene_Play::sGunUpdate(sf::Time dt)
{
	for (auto ntt : m_entityManager.getEntities(EntityType::ENEMY_SUB)) {
		if (ntt->isActive()) {
			auto& gun = ntt->getComponent<CGun>();

			gun.countdown -= dt;
			gun.isFiring = isNttInsideBounds(ntt);

			if (gun.isFiring && gun.countdown < sf::Time::Zero) {
				gun.isFiring = false;
				gun.countdown = sf::seconds(10.f) / (1.f + gun.fireRate);

				createBullet(ntt);
			}
		}
	}

}

void Scene_Play::loadInitialTextures()
{
	auto e = m_entityManager.addEntity(EntityType::BACKGROUND);

	auto& backgroundImage
		= e->addComponent<CSprite>(m_game->assets().getTexture("Sea")).sprite;

	backgroundImage.setOrigin(0.f, 0.f);

	m_worldBounds.width = backgroundImage.getLocalBounds().width;
	m_worldBounds.height = backgroundImage.getLocalBounds().height;


	auto waves = m_entityManager.addEntity(EntityType::FOREGROUND);
	auto& wavesSprite
		= waves->addComponent<CSprite>(m_game->assets().getTexture("Waves")).sprite;
	wavesSprite.setOrigin(0.f, 0.f);


	// loading player dead textures
	deadSubTxtreMap[0] = m_game->assets().getTexture("Sub01La");
	deadSubTxtreMap[1] = m_game->assets().getTexture("Sub01Lb");
	deadSubTxtreMap[2] = m_game->assets().getTexture("Sub01Lc");
	deadSubTxtreMap[3] = m_game->assets().getTexture("Sub01Ra");
	deadSubTxtreMap[4] = m_game->assets().getTexture("Sub01Rb");
	deadSubTxtreMap[5] = m_game->assets().getTexture("Sub01Rc");

}

void Scene_Play::drawVirtualLanes()
{
	for (size_t i = 1; i <= virtualLaneCount; i++) {
		auto newEntity = m_entityManager.addEntity(EntityType::NPC);

		newEntity->addComponent<CShape>(50);
		newEntity->addComponent<CTransform>(
			sf::Vector2f(200.f, MIN_Y_POSITION + virtualLaneHeight * i),
			sf::Vector2f(0.f, 0.f),
			0, 0);

	}
}

bool Scene_Play::isNttInsideBounds(NttPtr ntt)
{
	if (ntt->hasComponent<CTransform>()
		&& ntt->hasComponent<CSprite>()) {

		auto nttTfm = ntt->getComponent<CTransform>();
		auto nttSprite = ntt->getComponent<CSprite>().sprite;

		auto nttHalfWidth = nttSprite.getLocalBounds().width / 2.f;

		auto nttLeftBound = nttTfm.pos.x - nttHalfWidth;
		auto nttRightBound = nttTfm.pos.x + nttHalfWidth;

		return (nttLeftBound > 0.f && nttRightBound < m_worldBounds.width);
	}
	return false;
}
