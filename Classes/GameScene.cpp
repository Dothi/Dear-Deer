#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "Definitions.h"
#include <iostream>
#include <vector>



static bool started;
const char *HIGH_SCORE = "key1";
const char *MUTED = "key2";
Scene* GameScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::createWithPhysics();
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	// 'layer' is an autorelease object
	auto layer = GameScene::create();
	//layer->SetPhysicsWorld(scene->getPhysicsWorld());
	// add layer as a child to scene
	scene->addChild(layer);
	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
	/*if (!Layer::init())
	{
	return false;
	}*/

	def = CCUserDefault::sharedUserDefault();

	this->scheduleUpdate();

	bestScore = def->getIntegerForKey(HIGH_SCORE);
	score = 0;
	timer = 0.0;
	breathTimer = 0.0f;
	bgSpeed = 2.0f;
	trainSpeed = 2.0f;
	difficulty = 0;
	scroller = 0;
	randomed = false;
	damaged = false;
	randomBG = 0;
	srand(time(0));
	randomBG = rand() % 7;
	paused = false;
	isScrolling = false;
	loseSound = false;
	saved = false;
	speed = false;
	scored = false;
	soundMuted = def->getBoolForKey(MUTED);
	visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	audio->preload("sounds/BigDeer_AttackCrash_low.mp3");
	audio->preload("sounds/SmallDeer_AttackCrash_low.mp3");
	audio->preload("sounds/ProtagonistDeer_Jump2_low_volume.mp3");
	audio->preload("sounds/ProtagonistDeer_Land2.mp3");
	audio->preload("sounds/ProtagonistDeer_AttackCrash_low.mp3");
	audio->preload("sounds/FemaleDeer_Love_low.mp3");
	audio->preload("sounds/Loss2_low.mp3");

	
	initEnemyAnims();

	setTouchEvents();
	setWorldAndDebugLayer();
	setBackgrounds(randomBG);
	
	player = Player::create();
	player->setPosition(300, 120);
	this->addChild(player, 1);
	definePlayer();
	createPools();
	setUI();
	initEffects();
	this->schedule(schedule_selector(GameScene::resetEnemy), 3.5f);

	//physical ground
	addWall(visibleSize.width, 10, visibleSize.width / 2, 0 + 10);
	player->pState = Player::START;

	title = CCSprite::create("title.png");
	title->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height + origin.y - title->getContentSize().height));
	this->addChild(title);

	scoreBoard = CCSprite::create("Scoreboard.png");
	scoreBoard->setPosition(ccp(880, 500));
	scoreBoard->setVisible(false);
	this->addChild(scoreBoard);

	if (randomBG == 5)
	{
		scoreSprite = CCSprite::create("Score_white.png");
		scoreSprite->setPosition(ccp(860, 530));
		scoreSprite->setVisible(false);
		this->addChild(scoreSprite);
	}
	else
	{
		scoreSprite = CCSprite::create("Score.png");
		scoreSprite->setPosition(ccp(860, 530));
		scoreSprite->setVisible(false);
		this->addChild(scoreSprite);
	}
	

	gameOverSprite = CCSprite::create("GameOver.png");
	gameOverSprite->setPosition(ccp((visibleSize.width + origin.x) / 2, (visibleSize.height + origin.y) / 2 + 100));
	gameOverSprite->setVisible(false);
	this->addChild(gameOverSprite);

	tapToPlay = CCSprite::create("TaptoPlay.png");
	tapToPlay->setPosition(ccp(visibleSize.width / 2 + origin.x, (visibleSize.height / 2 + origin.y) - 100));
	this->addChild(tapToPlay);

	pausedText = CCSprite::create("UI/Paused.png");
	pausedText->setPosition(ccp(visibleSize.width / 2 + origin.x, (visibleSize.height / 2 + origin.y) + 100));
	pausedText->setVisible(false);
	this->addChild(pausedText);

	setMenu();

	scoreInfo = Label::createWithTTF("0", "fonts/PrStart.ttf", 28);
	if (randomBG == 5)
	{
		scoreInfo->setColor(cocos2d::Color3B(255, 255, 255));
	}
	else
	{
		scoreInfo->setColor(cocos2d::Color3B(0, 0, 0));
	}
	
	scoreInfo->setPosition(ccp(944, 528));
	scoreInfo->setVisible(false);
	
	this->addChild(scoreInfo);

	bestScoreInfo = Label::createWithTTF("Best Score: " + bestScore, "fonts/PrStart.ttf", 28);

	std::stringstream ss;

	ss << bestScore;

	std::string tp = ss.str();
	GameScene::bestScoreInfo->setString(tp);

	bestScoreInfo->setColor(cocos2d::Color3B(0, 0, 0));
	bestScoreInfo->setPosition(ccp(940, 475));
	bestScoreInfo->setVisible(false);
	this->addChild(bestScoreInfo);

	gameOverTip = Label::createWithTTF("Jump over the big ones!", "fonts/PrStart.ttf", 26);
	gameOverTip->setColor(cocos2d::Color3B(0, 0, 0));
	gameOverTip->setPosition(ccp(visibleSize.width / 2 + origin.x, (visibleSize.height / 2 + origin.y) - 175));
	gameOverTip->setVisible(false);
	gameOverTip->setZOrder(10);
	this->addChild(gameOverTip);

	heart = Sprite::create();
	heart->setPosition(player->getPositionX(), 130);
	heart->setLocalZOrder(8);
	this->addChild(heart);

	scoreEff = Sprite::create();
	scoreEff->setPosition(player->getPositionX(), 200);
	scoreEff->setLocalZOrder(9);
	this->addChild(scoreEff);

	landingDust = Sprite::create();
	landingDust->setPosition(player->getPositionX() - 10, player->getPositionY() - 100);
	landingDust->setLocalZOrder(8);
	this->addChild(landingDust);

	bigImpactSprite = Sprite::create();
	bigImpactSprite->setPosition(player->getPositionX() + 30, player->getPositionY());
	bigImpactSprite->setLocalZOrder(8);
	this->addChild(bigImpactSprite);

	alpha = 0;

	return true;
}
void GameScene::initEffects()
{
	char str[100] = { 0 };
	Vector<SpriteFrame*> heartAnimFrames(15);

	for (int i = 1; i <= 15; i++)
	{
		sprintf(str, "effects/heartEffect_%i.png", i);
		auto frame = SpriteFrame::create(str, Rect(0, 0, 192, 288));
		heartAnimFrames.pushBack(frame);
	}

	auto heartAnimation = Animation::createWithSpriteFrames(heartAnimFrames, 0.1f);
	heartEffect = Animate::create(heartAnimation);
	heartEffect->retain();

	Vector<SpriteFrame*> scoreAnimFrames(14);

	for (int i = 1; i <= 14; i++)
	{
		sprintf(str, "effects/scoreplus_%i.png", i);
		auto frame = SpriteFrame::create(str, Rect(0, 0, 256, 640));
		scoreAnimFrames.pushBack(frame);
	}

	auto scoreAnimation = Animation::createWithSpriteFrames(scoreAnimFrames, 0.065f);
	scoreEffect = Animate::create(scoreAnimation);
	scoreEffect->retain();

	Vector<SpriteFrame*> landingAnimFrames(6);

	for (int i = 1; i <= 6; i++)
	{
		sprintf(str, "effects/landing_dust_%i.png", i);
		auto frame = SpriteFrame::create(str, Rect(0, 0, 192, 192));
		landingAnimFrames.pushBack(frame);
	}

	auto landingAnimation = Animation::createWithSpriteFrames(landingAnimFrames, 0.1f);
	landingEffect = Animate::create(landingAnimation);
	landingEffect->retain();

	Vector<SpriteFrame*> bigImpactFrames(6);

	for (int i = 1; i <= 6; i++)
	{
		sprintf(str, "effects/hitEffect1_%i.png", i);
		auto frame = SpriteFrame::create(str, Rect(0, 0, 128, 128));
		bigImpactFrames.pushBack(frame);
	}
	
	auto bigImpactAnimation = Animation::createWithSpriteFrames(bigImpactFrames, 0.1f);
	bigImpact = Animate::create(bigImpactAnimation);
	bigImpact->retain();

	Vector<SpriteFrame*> smallImpactFrames(5);

	for (int i = 1; i <= 5; i++)
	{
		sprintf(str, "effects/hitEffect2_%i.png", i);
		auto frame = SpriteFrame::create(str, Rect(0, 0, 128, 128));
		smallImpactFrames.pushBack(frame);
	}

	auto smallImpactAnimation = Animation::createWithSpriteFrames(smallImpactFrames, 0.1f);
	smallImpact = Animate::create(smallImpactAnimation);
	smallImpact->retain();

	Vector<SpriteFrame*> playerImpactFrames(5);

	for (int i = 1; i <= 5; i++)
	{
		sprintf(str, "effects/hitEffect3_%i.png", i);
		auto frame = SpriteFrame::create(str, Rect(0, 0, 128, 128));
		playerImpactFrames.pushBack(frame);
	}

	auto playerImpactAnimation = Animation::createWithSpriteFrames(playerImpactFrames, 0.1f);
	playerImpact = Animate::create(playerImpactAnimation);
	playerImpact->retain();
}
void GameScene::initEnemyAnims()
{
	char str[100] = { 0 };

	Vector<SpriteFrame*> femAnimFrames(12);

	for (int i = 1; i <= 12; i++)
	{
		sprintf(str, "female/femaledeer_%i.png", i);
		auto frame = SpriteFrame::create(str, Rect(0, 0, 768, 256));
		femAnimFrames.pushBack(frame);
	}

	auto femAnimation = Animation::createWithSpriteFrames(femAnimFrames, 0.1f);
	femaleAnimate = Animate::create(femAnimation);
	femaleAnimate->retain();

	Vector<SpriteFrame*> enemyAnimFrames(6);

	for (int i = 1; i <= 6; i++)
	{
		sprintf(str, "small/enemy_small_%i.png", i);
		auto frame = SpriteFrame::create(str, Rect(0, 0, 192, 128));
		enemyAnimFrames.pushBack(frame);
	}

	auto moveAnimation = Animation::createWithSpriteFrames(enemyAnimFrames, 0.1f);
	enemyAnimate = Animate::create(moveAnimation);
	//enemyAnimate->reverse();
	enemyAnimate->retain();
}
void GameScene::update(float dt)
{
	if (!paused)
	{
		if (randomBG == 5)
		{
			GameScene::moveTrain();
		}
		else if (randomBG == 6)
		{
			GameScene::moveBoat();
		}
		
		
		if (landingDust->numberOfRunningActions() == 0)
		{
			landingDust->setVisible(false);
		}
		if (bigImpactSprite->numberOfRunningActions() == 0)
		{
			bigImpactSprite->setVisible(false);
		}
	}
	if (!started)
	{
		audio->setMaxAudioInstance(1);
		if (player->idleAnimate->getCurrentFrameIndex() == 20)
		{
			
			audio->play2d("sounds/ProtagonistDeer_IdleBreath.mp3");
		}
	}
	else
	{
		audio->setMaxAudioInstance(3);
		if (!paused)
		{
			if (scored)
			{
				if (scoreEff->numberOfRunningActions() == 0)
				{
					
					scoreEff->setTexture("effects/scoreplus_1.png");
					scoreEff->setPosition(player->getPositionX() + 20, player->getPositionY() + 210);

					scoreEff->runAction(scoreEffect);
					scoreEff->setVisible(true);

					scored = false;
				}
			}
			if (scoreEff->numberOfRunningActions() == 0)
			{

				scoreEff->cleanup();
				scoreEff->setVisible(false);


			}
		}
		
	}
	
	GameScene::startScreen();

	if (player->pState == Player::DEAD)
	{
		if (!paused)
		{
			playButton->setVisible(true);
		}
		else
		{
			playButton->setVisible(false);
		}
	}

	if (speed)
	{
		bgSpeed = 6.0f;
		trainSpeed = 2.0f;
	}
	else
	{
		bgSpeed = 4.0f;
		trainSpeed = 4.0f;
	}

	if (player->pState == Player::CHARGE && contactListener->numFootContacts >= 1)
	{
		speed = true;
	}
	else if (player->oldState == Player::CHARGE && contactListener->numFootContacts <= 0)
	{
		speed = true;
	}
	else
	{
		speed = false;

	}

	if (player->smallEnemyDmg)
	{
		bigImpactSprite->setTexture("effects/hitEffect2_1.png");
		bigImpactSprite->setPosition(ccp(player->getPositionX() + 30, player->getPositionY() + 30));
		bigImpactSprite->setVisible(true);
		bigImpactSprite->runAction(smallImpact);
		if (!soundMuted)
		{
			audio->play2d("sounds/SmallDeer_AttackCrash_low.mp3");
		}

		FShake* shake = FShake::actionWithDuration(.25f, 3.0f);
		this->runAction(shake);
		//CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("sounds/SmallDeer_AttackCrash_low.wav");
		player->smallEnemyDmg = false;
	}
	else if (player->bigEnemyDmg)
	{
		bigImpactSprite->setTexture("effects/hitEffect1_1.png");
		bigImpactSprite->setPosition(ccp(player->getPositionX() + 30, player->getPositionY() + 20));
		bigImpactSprite->setVisible(true);
		bigImpactSprite->runAction(bigImpact);
		if (!soundMuted)
		{
			audio->play2d("sounds/BigDeer_AttackCrash_low.mp3");
		}
		FShake* shake = FShake::actionWithDuration(.25f, 6.0f);
		this->runAction(shake);
		//CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("sounds/BigDeer_AttackCrash_low.wav");
		player->bigEnemyDmg = false;
	}
	GameScene::handlePause();

	if (player->pState != Player::DEAD)
	{
		if (damaged && !paused)
		{
			player->moving = false;

			timer += 1;


			player->stopAllActions();
			player->setTexture("Protagonist_Deer_Turpaanotto.png");

			if (timer >= 40)
			{
				damaged = false;
				timer = 0.0;
				player->run();
			}
			else if (player->pState == Player::CHARGE && contactListener->numFootContacts < 1)
			{
				damaged = false;
				timer = 0.0;
				//player->run();
			}
		}
	}


	if (player->gotHealth)
	{
		heart->setTexture("effects/heartEffect_1.png");
		heart->setVisible(true);
		heart->runAction(heartEffect);
		if (!soundMuted)
		{
			audio->play2d("sounds/FemaleDeer_Love_low.mp3");
		}
		player->gotHealth = false;
	}
	if (heart->numberOfRunningActions() == 0)
	{
		heart->setVisible(false);
	}
	if (score != player->score)
	{

		scored = true;
		score++;

		std::stringstream ss;

		ss << score;

		std::string tp = ss.str();
		GameScene::scoreInfo->setString(tp);
	}
	if (!paused)
	{
		GameScene::handleDifficulty();
		float deltaTime = dt;
		GameScene::updatePlayerAndEnemy(deltaTime);
		GameScene::updateUI();
		if (player->pState != Player::DEAD)
		{
			GameScene::updatePhysics(deltaTime);
		}

		GameScene::handlePlayerStates();

	}
	if (enemyBodies.size() > 0)
	{
		moveEnemies();
	}

	// Back button quit
	auto touchListener = EventListenerKeyboard::create();
	touchListener->onKeyReleased = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
	{
		if (keyCode == EventKeyboard::KeyCode::KEY_BACK)
		{
			GameScene::pause();

			//Director::getInstance()->end();
		}
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}
void GameScene::startScreen()
{
	if (tapToPlay->isVisible())
	{
		if (increaseAlpha)
		{
			alpha += 2.5;
			tapToPlay->setOpacity(alpha);
			if (alpha >= 255.0)
			{
				increaseAlpha = false;
				//alpha = 100.0;
			}
		}
		else
		{
			alpha -= 2.5;
			tapToPlay->setOpacity(alpha);
			if (alpha <= 0.0)
			{
				increaseAlpha = true;

			}
		}

	}
}
void GameScene::updatePlayerAndEnemy(float dt)
{
	player->update(dt);

	for (std::vector<Enemy*>::iterator it = enemyVec.begin(); it != enemyVec.end(); it++)
	{
		Enemy* e = *it;
		e->update(dt);
	}
}
void GameScene::updatePhysics(float dt)
{
	float deltaTime = dt;
	int positionIterations = 10;
	int velocityIterations = 10;

	deltaTime = deltaTime;
	world->Step(deltaTime, velocityIterations, positionIterations);

	if (!bodiesToDestroy.empty())
	{
		int length = 0;
		for (std::vector<b2Body*>::iterator it = bodiesToDestroy.begin(); it != bodiesToDestroy.end(); it++)
		{
			b2Body* b = *it;
			Enemy* e = (Enemy*)b->GetUserData();
			e->dead = false;
			e->charging = false;
			e->removeFromParentAndCleanup(true);
			e->setVisible(false);
			e->setPosition(ccp(visibleSize.width + 200, 0));
			e->stopAllActions();
			world->DestroyBody(*it);
			length++;
		}
		enemyBodies.erase(enemyBodies.begin(), enemyBodies.begin() + length);
		bodiesToDestroy.clear();
		length = 0;
	}

	for (b2Body *body = world->GetBodyList(); body != NULL; body = body->GetNext())
		if (body->GetUserData())
		{
		CCSprite *sprite = (CCSprite*)body->GetUserData();
		sprite->setPosition(ccp(body->GetPosition().x * SCALE_RATIO, body->GetPosition().y * SCALE_RATIO));
		sprite->setRotation(-1 * CC_RADIANS_TO_DEGREES(body->GetAngle()));
		}
	world->ClearForces();
	//world->DrawDebugData();
}
void GameScene::handlePause()
{
	if (started)
	{
		pauseButton->setVisible(true);
		title->setVisible(false);
		if (paused)
		{
			heart->pause();
			bigImpactSprite->pause();
			scoreEff->pause();
			landingDust->pause();

			quitButton->setVisible(true);
			resumeButton->setVisible(true);
			pausedText->setVisible(true);
			muteButton->setVisible(true);


			gameOverTip->setVisible(false);

			if (player->pState == Player::DEAD)
			{
				pausedText->setVisible(false);
				pauseButton->setVisible(false);

			}

			if (isScrolling)
			{
				
					this->unschedule(schedule_selector(GameScene::moveBg));
					scroller = 0;
				
				isScrolling = false;
			}

			player->pause();
			for (std::vector<Enemy*>::iterator it = enemyVec.begin(); it != enemyVec.end(); ++it)
			{
				Enemy* temp = *it;
				temp->pause();
				temp = nullptr;
			}
		}
		else
		{
			quitButton->setVisible(false);
			resumeButton->setVisible(false);
			pausedText->setVisible(false);
			muteButton->setVisible(false);

			heart->resume();
			bigImpactSprite->resume();
			scoreEff->resume();
			landingDust->resume();

			if (!paused)
			{
				if (!isScrolling);
				{
					if (scroller < 1)
					{
						this->schedule(schedule_selector(GameScene::moveBg), 0.01667);
						isScrolling = true;
						scroller = 1;
					}
				}
			}
			player->resume();
			for (std::vector<Enemy*>::iterator it = enemyVec.begin(); it != enemyVec.end(); ++it)
			{
				Enemy* temp = *it;
				temp->resume();
				temp = nullptr;
			}
		}
	}
	else
	{
		pauseButton->setVisible(false);
	}
}
void GameScene::setBackgrounds(int _random)
{
	if (_random == 0)
	{
		// Autumn 6 layers
		layer1 = Sprite::create("Parallax/Autumn/Layer1_Ground.png");
		layer2 = Sprite::create("Parallax/Autumn/Layer2_SecondGround.png");
		layer3 = Sprite::create("Parallax/Autumn/Layer3_Trees.png");
		layer4 = Sprite::create("Parallax/Autumn/Layer4_Mountains.png");
		layer5 = Sprite::create("Parallax/Autumn/Layer5_Clouds.png");
		layer6 = Sprite::create("Parallax/Autumn/Layer6_Sky.png");
		copyLayer1 = Sprite::create("Parallax/Autumn/Layer1_Ground.png");
		copyLayer2 = Sprite::create("Parallax/Autumn/Layer2_SecondGround.png");
		copyLayer3 = Sprite::create("Parallax/Autumn/Layer3_Trees.png");
		copyLayer4 = Sprite::create("Parallax/Autumn/Layer4_Mountains.png");
		copyLayer5 = Sprite::create("Parallax/Autumn/Layer5_Clouds.png");
	}
	else if (_random == 1)
	{
		// Summer 5 layers
		layer1 = Sprite::create("Parallax/Summer/Layer1_Ground.png");
		layer2 = Sprite::create("Parallax/Summer/Layer2_Trees.png");
		layer2->setVisible(false);
		layer3 = Sprite::create("Parallax/Summer/Layer2_Trees.png");
		layer4 = Sprite::create("Parallax/Summer/Layer3_Ocean.png");
		layer5 = Sprite::create("Parallax/Summer/Layer4_Clouds.png");
		layer6 = Sprite::create("Parallax/Summer/Layer5_Sky.png");
		copyLayer1 = Sprite::create("Parallax/Summer/Layer1_Ground.png");
		copyLayer2 = Sprite::create("Parallax/Summer/Layer2_Trees.png");
		copyLayer2->setVisible(false);
		copyLayer3 = Sprite::create("Parallax/Summer/Layer2_Trees.png");
		copyLayer4 = Sprite::create("Parallax/Summer/Layer3_Ocean.png");
		copyLayer5 = Sprite::create("Parallax/Summer/Layer4_Clouds.png");
	}
	else if (_random == 2)
	{
		// Winter
		layer1 = Sprite::create("Parallax/Winter/Layer1_Ground.png");
		layer2 = Sprite::create("Parallax/Winter/Layer2_Snow.png");
		layer3 = Sprite::create("Parallax/Winter/Layer3_Trees.png");
		layer4 = Sprite::create("Parallax/Winter/Layer4_Trees.png");
		layer5 = Sprite::create("Parallax/Winter/Layer5_Clouds.png");
		layer6 = Sprite::create("Parallax/Winter/Layer6_Sky.png");
		copyLayer1 = Sprite::create("Parallax/Winter/Layer1_Ground.png");
		copyLayer2 = Sprite::create("Parallax/Winter/Layer2_Snow.png");
		copyLayer3 = Sprite::create("Parallax/Winter/Layer3_Trees.png");
		copyLayer4 = Sprite::create("Parallax/Winter/Layer4_Trees.png");
		copyLayer5 = Sprite::create("Parallax/Winter/Layer5_Clouds.png");
	}
	else if (_random == 3)
	{
		// Swamp
		layer1 = Sprite::create("Parallax/Swamp/Layer1_Ground.png");
		layer2 = Sprite::create("Parallax/Swamp/Layer2_SecondGround.png");
		layer3 = Sprite::create("Parallax/Swamp/Layer3_Tree1.png");
		layer4 = Sprite::create("Parallax/Swamp/Layer4_TreeLine.png");
		layer5 = Sprite::create("Parallax/Swamp/Layer5_SecondTreeLine.png");
		layer6 = Sprite::create("Parallax/Swamp/Layer6_Sky.png");
		copyLayer1 = Sprite::create("Parallax/Swamp/Layer1_Ground.png");
		copyLayer2 = Sprite::create("Parallax/Swamp/Layer2_SecondGround.png");
		copyLayer3 = Sprite::create("Parallax/Swamp/Layer3_Tree2.png");
		copyLayer4 = Sprite::create("Parallax/Swamp/Layer4_TreeLine.png");
		copyLayer5 = Sprite::create("Parallax/Swamp/Layer5_SecondTreeLine.png");
	}
	else if (_random == 4)
	{
		// Funky
		layer1 = Sprite::create("Parallax/Funky/Layer_1.png");
		layer2 = Sprite::create("Parallax/Funky/Layer_2.png");
		layer3 = Sprite::create("Parallax/Funky/Layer_3.png");
		layer4 = Sprite::create("Parallax/Funky/Layer_4.png");
		layer5 = Sprite::create("Parallax/Funky/Layer_5_stars.png");
		layer6 = Sprite::create("Parallax/Funky/Layer_6_BG.png");
		copyLayer1 = Sprite::create("Parallax/Funky/Layer_1.png");
		copyLayer2 = Sprite::create("Parallax/Funky/Layer_2.png");
		copyLayer3 = Sprite::create("Parallax/Funky/Layer_3.png");
		copyLayer4 = Sprite::create("Parallax/Funky/Layer_4.png");
		copyLayer5 = Sprite::create("Parallax/Funky/Layer_5_stars.png");
	}
	else if (_random == 5)
	{
		// City
		layer1 = Sprite::create("Parallax/City/Layer1_Ground.png");
		layer2 = Sprite::create("Parallax/City/Layer2_Bushes.png");
		layer3 = Sprite::create("Parallax/City/Layer3_Lightpole.png");
		layer4 = Sprite::create("Parallax/City/Layer4_Road.png");
		layer5 = Sprite::create("Parallax/City/Layer5_City.png");
		layer6 = Sprite::create("Parallax/City/Layer6_Train.png");
		layer7 = Sprite::create("Parallax/City/Layer7_Trainroad.png");
		layer8 = Sprite::create("Parallax/City/Layer8_Sea.png");
		layer9 = Sprite::create("Parallax/City/Layer9_Moon.png");
		layer10 = Sprite::create("Parallax/City/Layer10_Stars.png");
		layer11 = Sprite::create("Parallax/City/Layer11_Sky.png");
		copyLayer1 = Sprite::create("Parallax/City/Layer1_Ground.png");
		copyLayer2 = Sprite::create("Parallax/City/Layer2_Bushes.png");
		copyLayer3 = Sprite::create("Parallax/City/Layer3_Lightpole.png");
		copyLayer4 = Sprite::create("Parallax/City/Layer4_Road.png");
		copyLayer5 = Sprite::create("Parallax/City/Layer5_City.png");
		copyLayer6 = Sprite::create("Parallax/City/Layer6_Train.png");
		layer6->setVisible(false);
		copyLayer7 = Sprite::create("Parallax/City/Layer7_Trainroad.png");
		copyLayer8 = Sprite::create("Parallax/City/Layer8_Sea.png");
		copyLayer9 = Sprite::create("Parallax/City/Layer9_Moon.png");
		copyLayer9->setVisible(false);
		copyLayer10 = Sprite::create("Parallax/City/Layer10_Stars.png");
	}
	else if (_random == 6)
	{
		// RiverCity
		layer1 = Sprite::create("Parallax/River/Layer1_Grass.png");
		layer2 = Sprite::create("Parallax/River/Layer2_Light_shafts.png");
		layer3 = Sprite::create("Parallax/River/Layer3_Treeline_front.png");
		layer4 = Sprite::create("Parallax/River/Layer4_Treeline_back.png");
		layer5 = Sprite::create("Parallax/River/Layer5_Rowboat.png");
		layer6 = Sprite::create("Parallax/River/Layer6_Motorboat.png");
		layer7 = Sprite::create("Parallax/River/Layer7_Sailboat.png");
		layer7->setVisible(false);
		layer8 = Sprite::create("Parallax/River/Layer8_River.png");
		layer9 = Sprite::create("Parallax/River/Layer9_City_bushes.png");
		layer10 = Sprite::create("Parallax/River/Layer10_City.png");
		layer11 = Sprite::create("Parallax/River/Layer11_Clouds.png");
		layer12 = Sprite::create("Parallax/River/Layer12_Sky.png");
		copyLayer1 = Sprite::create("Parallax/River/Layer1_Grass.png");
		copyLayer2 = Sprite::create("Parallax/River/Layer2_Light_shafts.png");
		//copyLayer2->setVisible(false);
		//layer2->setVisible(false);
		copyLayer3 = Sprite::create("Parallax/River/Layer3_Treeline_front.png");
		copyLayer4 = Sprite::create("Parallax/River/Layer4_Treeline_back.png");
		copyLayer5 = Sprite::create("Parallax/River/Layer5_Rowboat.png");
		copyLayer5->setVisible(false);
		copyLayer6 = Sprite::create("Parallax/River/Layer6_Motorboat.png");
		copyLayer6->setVisible(false);
		copyLayer7 = Sprite::create("Parallax/River/Layer7_Sailboat.png");
		//copyLayer7->setVisible(false);
		copyLayer8 = Sprite::create("Parallax/River/Layer8_River.png");
		copyLayer9 = Sprite::create("Parallax/River/Layer9_City_bushes.png");
		copyLayer10 = Sprite::create("Parallax/River/Layer10_City.png");
		copyLayer11 = Sprite::create("Parallax/River/Layer11_Clouds.png");
	}

	/*layer1->setOpacity(50);
	layer2->setOpacity(50);
	layer3->setOpacity(50);
	layer4->setOpacity(50);
	layer5->setOpacity(50);
	layer6->setOpacity(50);
	copyLayer1->setOpacity(50);
	copyLayer2->setOpacity(50);
	copyLayer3->setOpacity(50);
	copyLayer4->setOpacity(50);
	copyLayer5->setOpacity(50);*/

	layer1->setPosition(ccp(0, 0));
	layer1->setAnchorPoint(ccp(0, 0));

	copyLayer1->setPosition(ccp(visibleSize.width - 1, 0));
	copyLayer1->setAnchorPoint(ccp(0, 0));

	if (_random != 1)
	{
		layer2->setPosition(ccp(0, 0));
		layer2->setAnchorPoint(ccp(0, 0));

		copyLayer2->setPosition(ccp(visibleSize.width - 1, 0));
		copyLayer2->setAnchorPoint(ccp(0, 0));
	}

	layer3->setPosition(ccp(0, 0));
	layer3->setAnchorPoint(ccp(0, 0));
	copyLayer3->setPosition(ccp(visibleSize.width - 1, 0));
	copyLayer3->setAnchorPoint(ccp(0, 0));

	layer4->setPosition(ccp(0, 0));
	layer4->setAnchorPoint(ccp(0, 0));

	copyLayer4->setPosition(ccp(visibleSize.width - 1, 0));
	copyLayer4->setAnchorPoint(ccp(0, 0));

	layer5->setPosition(ccp(0, 0));
	layer5->setAnchorPoint(ccp(0, 0));

	copyLayer5->setPosition(ccp(visibleSize.width - 1, 0));
	copyLayer5->setAnchorPoint(ccp(0, 0));

	layer6->setPosition(ccp(0, 0));
	layer6->setAnchorPoint(ccp(0, 0));

	this->addChild(layer1, -1);
	this->addChild(copyLayer1, -1);
	this->addChild(layer2, -2);
	this->addChild(copyLayer2, -2);
	this->addChild(layer3, -3);
	this->addChild(copyLayer3, -3);
	this->addChild(layer4, -4);
	this->addChild(copyLayer4, -4);
	this->addChild(layer5, -5);
	this->addChild(copyLayer5, -5);
	this->addChild(layer6, -6);

	if (_random == 5)
	{
		copyLayer6->setPosition(ccp(visibleSize.width - 1, 0));
		copyLayer6->setAnchorPoint(ccp(0, 0));

		layer7->setPosition(ccp(0, 0));
		layer7->setAnchorPoint(ccp(0, 0));

		copyLayer7->setPosition(ccp(visibleSize.width - 1, 0));
		copyLayer7->setAnchorPoint(ccp(0, 0));

		layer8->setPosition(ccp(0, 0));
		layer8->setAnchorPoint(ccp(0, 0));

		copyLayer8->setPosition(ccp(visibleSize.width - 1, 0));
		copyLayer8->setAnchorPoint(ccp(0, 0));

		layer9->setPosition(ccp(0, 0));
		layer9->setAnchorPoint(ccp(0, 0));

		copyLayer9->setPosition(ccp(visibleSize.width - 1, 0));
		copyLayer9->setAnchorPoint(ccp(0, 0));

		layer10->setPosition(ccp(0, 0));
		layer10->setAnchorPoint(ccp(0, 0));

		copyLayer10->setPosition(ccp(visibleSize.width - 1, 0));
		copyLayer10->setAnchorPoint(ccp(0, 0));

		layer11->setPosition(ccp(0, 0));
		layer11->setAnchorPoint(ccp(0, 0));

		this->addChild(copyLayer6, -6);
		this->addChild(layer7, -7);
		this->addChild(copyLayer7, -7);
		this->addChild(layer8, -8);
		this->addChild(copyLayer8, -8);
		this->addChild(layer9, -9);
		this->addChild(copyLayer9, -9);
		this->addChild(layer10, -10);
		this->addChild(copyLayer10, -10);
		this->addChild(layer11, -11);
	}
	else if (randomBG == 6)
	{
		copyLayer6->setPosition(ccp(visibleSize.width - 1, 0));
		copyLayer6->setAnchorPoint(ccp(0, 0));

		layer7->setPosition(ccp(0, 0));
		layer7->setAnchorPoint(ccp(0, 0));

		copyLayer7->setPosition(ccp(visibleSize.width - 1, 0));
		copyLayer7->setAnchorPoint(ccp(0, 0));

		layer8->setPosition(ccp(0, 0));
		layer8->setAnchorPoint(ccp(0, 0));

		copyLayer8->setPosition(ccp(visibleSize.width - 1, 0));
		copyLayer8->setAnchorPoint(ccp(0, 0));

		layer9->setPosition(ccp(0, 0));
		layer9->setAnchorPoint(ccp(0, 0));

		copyLayer9->setPosition(ccp(visibleSize.width - 1, 0));
		copyLayer9->setAnchorPoint(ccp(0, 0));

		layer10->setPosition(ccp(0, 0));
		layer10->setAnchorPoint(ccp(0, 0));

		copyLayer10->setPosition(ccp(visibleSize.width - 1, 0));
		copyLayer10->setAnchorPoint(ccp(0, 0));

		layer11->setPosition(ccp(0, 0));
		layer11->setAnchorPoint(ccp(0, 0));

		copyLayer11->setPosition(ccp(visibleSize.width - 1, 0));
		copyLayer11->setAnchorPoint(ccp(0, 0));

		layer12->setPosition(ccp(0, 0));
		layer12->setAnchorPoint(ccp(0, 0));

		this->addChild(copyLayer6, -6);
		this->addChild(layer7, -7);
		this->addChild(copyLayer7, -7);
		this->addChild(layer8, -8);
		this->addChild(copyLayer8, -8);
		this->addChild(layer9, -9);
		this->addChild(copyLayer9, -9);
		this->addChild(layer10, -10);
		this->addChild(copyLayer10, -10);
		this->addChild(layer11, -11);
		this->addChild(copyLayer11, -11);
		this->addChild(layer12, -12);
	}
	
}

void GameScene::setWorldAndDebugLayer()
{
	b2Vec2 gravity = b2Vec2(0.0f, -9.81f);
	world = new b2World(gravity);
	contactListener = new MyContactListener();
	world->SetContactListener(contactListener);

	/*debugDrawLayer = B2DebugDrawLayer::create(world, SCALE_RATIO);
	debugDrawLayer->setPosition(Vec2(0, 0));

	this->addChild(debugDrawLayer, 999);*/
}

void GameScene::setTouchEvents()
{
	visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto touchListener = EventListenerTouchOneByOne::create();

	touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);
	touchListener->onTouchCancelled = CC_CALLBACK_2(GameScene::onTouchCancelled, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void GameScene::addWall(float w, float h, float px, float py)
{
	b2PolygonShape floorShape;

	floorShape.SetAsBox(w / SCALE_RATIO, h / SCALE_RATIO);
	b2FixtureDef floorFixture;
	floorFixture.density = 0;
	floorFixture.friction = 10;
	floorFixture.restitution = 0;
	floorFixture.shape = &floorShape;
	b2BodyDef floorBodyDef;

	floorBodyDef.position.Set(px / SCALE_RATIO, py / SCALE_RATIO);
	b2Body *floorBody = world->CreateBody(&floorBodyDef);

	floorBody->CreateFixture(&floorFixture);
}

void GameScene::definePlayer()
{
	playerShape.SetAsBox(192 / SCALE_RATIO / 4, 192 / SCALE_RATIO / 4, b2Vec2(0, 0), 0);
	b2FixtureDef playerFixture;
	playerFixture.density = 10;
	playerFixture.friction = 0.8f;
	playerFixture.restitution = 0;
	playerFixture.shape = &playerShape;

	playerBodyDef.position.Set(300 / SCALE_RATIO, 120 / SCALE_RATIO);
	playerBodyDef.type = b2_dynamicBody;
	playerBodyDef.userData = player;
	playerBodyDef.fixedRotation = true;

	playerBody = world->CreateBody(&playerBodyDef);
	playerBody->CreateFixture(&playerFixture);
	playerBody->SetGravityScale(2);

	playerShape.SetAsBox(.15, .15, b2Vec2(0, -0.8), 0);
	playerFixture.isSensor = true;
	b2Fixture* footSensorFixture = playerBody->CreateFixture(&playerFixture);
	footSensorFixture->SetUserData((void*)3);
	player->body = playerBody;
	player->setTag(1);

	playerShape.SetAsBox(140 / SCALE_RATIO / 4, 75 / SCALE_RATIO / 4, b2Vec2(-0.1, 0), 0);
	playerFixture.isSensor = true;
	b2Fixture* hurtBox = playerBody->CreateFixture(&playerFixture);
	hurtBox->SetUserData((void*)4);

}

void GameScene::defineEnemy(Enemy* _enemy)
{
	Enemy* enemy = _enemy;
	if (enemy->eState == Enemy::CHARGE)
	{
		enemyShape.SetAsBox(60 / SCALE_RATIO / 4, 150 / SCALE_RATIO / 4, b2Vec2(-1.5, 0.2f), 0);
	}
	else if (enemy->eState == Enemy::FEMALE)
	{
		enemyShape.SetAsBox(160 / SCALE_RATIO / 4, 110 / SCALE_RATIO / 4, b2Vec2(-0.5, 0), 0);
	}
	else if (enemy->eState == Enemy::IDLE)
	{
		enemyShape.SetAsBox(80 / SCALE_RATIO / 4, 110 / SCALE_RATIO / 4, b2Vec2(-0.5, 0), 0);
	}
	b2FixtureDef enemyFixture;
	enemyFixture.density = 10;
	enemyFixture.friction = 0.8f;
	enemyFixture.restitution = 0;
	enemyFixture.shape = &enemyShape;
	enemyFixture.isSensor = true;

	if (enemy->eState == Enemy::IDLE)
	{
		enemyBodyDef.position.Set((visibleSize.width + 100) / SCALE_RATIO, 80 / SCALE_RATIO);
	}
	else if (enemy->eState == Enemy::FEMALE)
	{
		enemyBodyDef.position.Set((visibleSize.width + 100) / SCALE_RATIO, 68 / SCALE_RATIO);
	}
	else
	{
		enemyBodyDef.position.Set((visibleSize.width + 100) / SCALE_RATIO, 80 / SCALE_RATIO);
	}


	enemyBodyDef.type = b2_dynamicBody;
	enemyBodyDef.userData = enemy;
	enemyBodyDef.fixedRotation = true;

	enemyBody = world->CreateBody(&enemyBodyDef);
	enemyBody->CreateFixture(&enemyFixture);
	enemyBody->SetGravityScale(0);
	enemy->body = enemyBody;
	b2Fixture* enemyFix = enemyBody->CreateFixture(&enemyFixture);
	enemyFix->SetUserData((void*)5);
	enemyBodies.push_back(enemyBody);

	enemy->setTag(2);
}

void GameScene::createPools()
{
	enemyRun = SpriteBatchNode::create("big_enemy_run_antlers_sheet_big.png");
	enemyRun->setPosition(ccp(visibleSize.width + 200, 0));

	 
	
	enemyPool = CCArray::createWithCapacity(20);
	enemyPool->retain();

	enemyPoolIndex = 0;

	for (int i = 0; i < 20; i++)
	{
		Enemy* sprite = Enemy::createWithTexture(enemyRun->getTexture(), CCRectMake(0, 0, 0, 0));
		sprite->setPosition(ccp(visibleSize.width + 250, 0));
		sprite->setVisible(false);
		
		
		enemyPool->addObject(sprite);
		sprite = nullptr;
	}
	enemyRun = nullptr;

}
void GameScene::resetEnemy(float dt)
{
	if (!paused)
	{
		if (player->pState != Player::DEAD && started)
		{
			// TODO: Remember to return enemies when needed
			Enemy* enemy = (Enemy*)enemyPool->objectAtIndex(enemyPoolIndex);
			enemy->setVisible(false);
			int randum;


			if (player->getHealth() < 5)
			{
				randum = rand() % 6;
			}
			else
			{
				randum = rand() % 5;

			}

			CCLOG("Random: %i", randum);
			if (randum == 1 || randum == 3)
			{
				enemy->eState = Enemy::IDLE;
			}
			else if (randum == 2 || randum == 4 || randum == 0)
			{
				enemy->eState = Enemy::CHARGE;
			}
			else
			{
				enemy->eState = Enemy::FEMALE;
			}

			enemyPoolIndex++;
			CCLOG("Enemypool: %i", enemyPoolIndex);

			if (enemyPoolIndex == enemyPool->count())
			{
				enemyPoolIndex = 0;
			}

			int enemy_x = visibleSize.width + 200;
			enemy->setPosition(Vec2(enemy_x, 200));
			

			if (enemy->eState == Enemy::CHARGE)
			{
				enemyRun = SpriteBatchNode::create("big_enemy_run_antlers_sheet.png");
				enemyRun->setVisible(false);
				enemy->setVisible(false);
				enemy->setTexture(enemyRun->getTexture());
				enemyRun->addChild(enemy, 1);
				enemyRun->setVisible(true);

				Animation* runAnimation = Animation::create();
				runAnimation->setDelayPerUnit(.1);

				int row = 2;
				int col = 3;

				for (int i = 0; i < row; i++)
				{
					for (int j = 0; j < col; j++)
					{
						SpriteFrame *frame = SpriteFrame::createWithTexture(enemyRun->getTexture(), CCRectMake(j * 256, i * 256, 256, 256));
						runAnimation->addSpriteFrame(frame);
					}
				}

				Animate* animate = Animate::create(runAnimation);
				RepeatForever* runAnimAction = RepeatForever::create(animate);
				enemy->runAction(runAnimAction);
				
				this->addChild(enemyRun, 1);
			}
			else if (enemy->eState == Enemy::FEMALE)
			{
				//enemy->removeFromParent();
				//enemy->stopAllActions();
				enemy->setPosition(Vec2(enemy_x, 130));
				
				if (enemy->numberOfRunningActions() == 0)
				{
					enemy->runAction(RepeatForever::create(femaleAnimate));
				}
				this->addChild(enemy, 1);
			}
			else if (enemy->eState == Enemy::IDLE)
			{
				//enemy->removeFromParent();
				//enemy->stopAllActions();
				enemy->setPosition(Vec2(enemy_x, 130));
				
				if (enemy->numberOfRunningActions() == 0)
				{
					enemy->runAction(RepeatForever::create(enemyAnimate));
				}
				this->addChild(enemy, 1);
			}

			enemy->setVisible(true);
			defineEnemy(enemy);
			enemyVec.push_back(enemy);
			//enemy = nullptr;
		}
	}
}

void GameScene::moveBg(float dt)
{
	if (player->pState != Player::DEAD)
	{
		if (randomBG < 5)
		{
			//Layer 1
			layer1->setPosition(ccp(layer1->getPosition().x - bgSpeed, layer1->getPosition().y));
			copyLayer1->setPosition(ccp(copyLayer1->getPosition().x - bgSpeed, copyLayer1->getPosition().y));

			if (layer1->getPosition().x < -layer1->boundingBox().size.width)
			{
				layer1->setPosition(ccp(copyLayer1->getPosition().x + copyLayer1->boundingBox().size.width, layer1->getPosition().y));
			}
			if (copyLayer1->getPosition().x < -copyLayer1->boundingBox().size.width)
			{
				copyLayer1->setPosition(ccp(layer1->getPosition().x + layer1->boundingBox().size.width, copyLayer1->getPosition().y));
			}

			if (randomBG != 1)
			{
				//Layer 2
				layer2->setPosition(ccp(layer2->getPosition().x - (bgSpeed * 0.8), layer2->getPosition().y));
				copyLayer2->setPosition(ccp(copyLayer2->getPosition().x - (bgSpeed * 0.8), copyLayer2->getPosition().y));

				if (layer2->getPosition().x < -layer2->boundingBox().size.width)
				{
					layer2->setPosition(ccp(copyLayer2->getPosition().x + copyLayer2->boundingBox().size.width, layer2->getPosition().y));
				}
				if (copyLayer2->getPosition().x < -copyLayer2->boundingBox().size.width)
				{
					copyLayer2->setPosition(ccp(layer2->getPosition().x + layer2->boundingBox().size.width, copyLayer2->getPosition().y));
				}
			}


			//Layer 3
			layer3->setPosition(ccp(layer3->getPosition().x - (bgSpeed * 0.6), layer3->getPosition().y));
			copyLayer3->setPosition(ccp(copyLayer3->getPosition().x - (bgSpeed * 0.6), copyLayer3->getPosition().y));

			if (layer3->getPosition().x < -layer3->boundingBox().size.width)
			{
				layer3->setPosition(ccp(copyLayer3->getPosition().x + copyLayer3->boundingBox().size.width, layer3->getPosition().y));
			}
			if (copyLayer3->getPosition().x < -copyLayer3->boundingBox().size.width)
			{
				copyLayer3->setPosition(ccp(layer3->getPosition().x + layer3->boundingBox().size.width, copyLayer3->getPosition().y));
			}



			//Layer 4
			layer4->setPosition(ccp(layer4->getPosition().x - (bgSpeed * 0.4), layer4->getPosition().y));
			copyLayer4->setPosition(ccp(copyLayer4->getPosition().x - (bgSpeed * 0.4), copyLayer4->getPosition().y));

			if (layer4->getPosition().x < -layer4->boundingBox().size.width)
			{
				layer4->setPosition(ccp(copyLayer4->getPosition().x + copyLayer4->boundingBox().size.width, layer4->getPosition().y));
			}
			if (copyLayer4->getPosition().x < -copyLayer4->boundingBox().size.width)
			{
				copyLayer4->setPosition(ccp(layer4->getPosition().x + layer4->boundingBox().size.width, copyLayer4->getPosition().y));
			}

			//Layer 5
			layer5->setPosition(ccp(layer5->getPosition().x - (bgSpeed * 0.2), layer5->getPosition().y));
			copyLayer5->setPosition(ccp(copyLayer5->getPosition().x - (bgSpeed * 0.2), copyLayer5->getPosition().y));

			if (layer5->getPosition().x < -layer5->boundingBox().size.width)
			{
				layer5->setPosition(ccp(copyLayer5->getPosition().x + copyLayer5->boundingBox().size.width, layer5->getPosition().y));
			}
			if (copyLayer5->getPosition().x < -copyLayer5->boundingBox().size.width)
			{
				copyLayer5->setPosition(ccp(layer5->getPosition().x + layer5->boundingBox().size.width, copyLayer5->getPosition().y));
			}
		}


		else if (randomBG == 5)
		{
			//Layer 1
			layer1->setPosition(ccp(layer1->getPosition().x - bgSpeed, layer1->getPosition().y));
			copyLayer1->setPosition(ccp(copyLayer1->getPosition().x - bgSpeed, copyLayer1->getPosition().y));

			if (layer1->getPosition().x < -layer1->boundingBox().size.width)
			{
				layer1->setPosition(ccp(copyLayer1->getPosition().x + copyLayer1->boundingBox().size.width, layer1->getPosition().y));
			}
			if (copyLayer1->getPosition().x < -copyLayer1->boundingBox().size.width)
			{
				copyLayer1->setPosition(ccp(layer1->getPosition().x + layer1->boundingBox().size.width, copyLayer1->getPosition().y));
			}

			//Layer 2
			layer2->setPosition(ccp(layer2->getPosition().x - (bgSpeed * 0.9), layer2->getPosition().y));
			copyLayer2->setPosition(ccp(copyLayer2->getPosition().x - (bgSpeed * 0.9), copyLayer2->getPosition().y));

			if (layer2->getPosition().x < -layer2->boundingBox().size.width)
			{
				layer2->setPosition(ccp(copyLayer2->getPosition().x + copyLayer2->boundingBox().size.width, layer2->getPosition().y));
			}
			if (copyLayer2->getPosition().x < -copyLayer2->boundingBox().size.width)
			{
				copyLayer2->setPosition(ccp(layer2->getPosition().x + layer2->boundingBox().size.width, copyLayer2->getPosition().y));
			}


			//Layer 3
			layer3->setPosition(ccp(layer3->getPosition().x - (bgSpeed * 0.8), layer3->getPosition().y));
			copyLayer3->setPosition(ccp(copyLayer3->getPosition().x - (bgSpeed * 0.8), copyLayer3->getPosition().y));

			if (layer3->getPosition().x < -layer3->boundingBox().size.width)
			{
				layer3->setPosition(ccp(copyLayer3->getPosition().x + copyLayer3->boundingBox().size.width, layer3->getPosition().y));
			}
			if (copyLayer3->getPosition().x < -copyLayer3->boundingBox().size.width)
			{
				copyLayer3->setPosition(ccp(layer3->getPosition().x + layer3->boundingBox().size.width, copyLayer3->getPosition().y));
			}



			//Layer 4
			layer4->setPosition(ccp(layer4->getPosition().x - (bgSpeed * 0.7), layer4->getPosition().y));
			copyLayer4->setPosition(ccp(copyLayer4->getPosition().x - (bgSpeed * 0.7), copyLayer4->getPosition().y));

			if (layer4->getPosition().x < -layer4->boundingBox().size.width)
			{
				layer4->setPosition(ccp(copyLayer4->getPosition().x + copyLayer4->boundingBox().size.width, layer4->getPosition().y));
			}
			if (copyLayer4->getPosition().x < -copyLayer4->boundingBox().size.width)
			{
				copyLayer4->setPosition(ccp(layer4->getPosition().x + layer4->boundingBox().size.width, copyLayer4->getPosition().y));
			}

			//Layer 5
			layer5->setPosition(ccp(layer5->getPosition().x - (bgSpeed * 0.6), layer5->getPosition().y));
			copyLayer5->setPosition(ccp(copyLayer5->getPosition().x - (bgSpeed * 0.6), copyLayer5->getPosition().y));

			if (layer5->getPosition().x < -layer5->boundingBox().size.width)
			{
				layer5->setPosition(ccp(copyLayer5->getPosition().x + copyLayer5->boundingBox().size.width, layer5->getPosition().y));
			}
			if (copyLayer5->getPosition().x < -copyLayer5->boundingBox().size.width)
			{
				copyLayer5->setPosition(ccp(layer5->getPosition().x + layer5->boundingBox().size.width, copyLayer5->getPosition().y));
			}

			

			//Layer 7
			layer7->setPosition(ccp(layer7->getPosition().x - (bgSpeed * 0.4), layer7->getPosition().y));
			copyLayer7->setPosition(ccp(copyLayer7->getPosition().x - (bgSpeed * 0.4), copyLayer7->getPosition().y));

			if (layer7->getPosition().x < -layer7->boundingBox().size.width)
			{
				layer7->setPosition(ccp(copyLayer7->getPosition().x + copyLayer7->boundingBox().size.width, layer7->getPosition().y));
			}
			if (copyLayer7->getPosition().x < -copyLayer7->boundingBox().size.width)
			{
				copyLayer7->setPosition(ccp(layer7->getPosition().x + layer7->boundingBox().size.width, copyLayer7->getPosition().y));
			}

			//Layer 8
			layer8->setPosition(ccp(layer8->getPosition().x - (bgSpeed * 0.3), layer8->getPosition().y));
			copyLayer8->setPosition(ccp(copyLayer8->getPosition().x - (bgSpeed * 0.3), copyLayer8->getPosition().y));

			if (layer8->getPosition().x < -layer8->boundingBox().size.width)
			{
				layer8->setPosition(ccp(copyLayer8->getPosition().x + copyLayer8->boundingBox().size.width, layer8->getPosition().y));
			}
			if (copyLayer8->getPosition().x < -copyLayer8->boundingBox().size.width)
			{
				copyLayer8->setPosition(ccp(layer8->getPosition().x + layer8->boundingBox().size.width, copyLayer8->getPosition().y));
			}

			//Layer 9
			layer9->setPosition(ccp(layer9->getPosition().x - (bgSpeed * 0.05), layer9->getPosition().y));
			copyLayer9->setPosition(ccp(copyLayer9->getPosition().x - (bgSpeed * 0.05), copyLayer9->getPosition().y));

			if (layer9->getPosition().x < -layer9->boundingBox().size.width)
			{
				layer9->setPosition(ccp(copyLayer9->getPosition().x + copyLayer9->boundingBox().size.width, layer9->getPosition().y));
			}
			if (copyLayer9->getPosition().x < -copyLayer9->boundingBox().size.width)
			{
				copyLayer9->setPosition(ccp(layer9->getPosition().x + layer9->boundingBox().size.width, copyLayer9->getPosition().y));
			}

			//Layer 10
			layer10->setPosition(ccp(layer10->getPosition().x - (bgSpeed * 0.1), layer10->getPosition().y));
			copyLayer10->setPosition(ccp(copyLayer10->getPosition().x - (bgSpeed * 0.1), copyLayer10->getPosition().y));

			if (layer10->getPosition().x < -layer10->boundingBox().size.width)
			{
				layer10->setPosition(ccp(copyLayer10->getPosition().x + copyLayer10->boundingBox().size.width, layer10->getPosition().y));
			}
			if (copyLayer10->getPosition().x < -copyLayer10->boundingBox().size.width)
			{
				copyLayer10->setPosition(ccp(layer10->getPosition().x + layer10->boundingBox().size.width, copyLayer10->getPosition().y));
			}
		}
		else if (randomBG == 6)
		{
			//Layer 1
			layer1->setPosition(ccp(layer1->getPosition().x - bgSpeed, layer1->getPosition().y));
			copyLayer1->setPosition(ccp(copyLayer1->getPosition().x - bgSpeed, copyLayer1->getPosition().y));

			if (layer1->getPosition().x < -layer1->boundingBox().size.width)
			{
				layer1->setPosition(ccp(copyLayer1->getPosition().x + copyLayer1->boundingBox().size.width, layer1->getPosition().y));
			}
			if (copyLayer1->getPosition().x < -copyLayer1->boundingBox().size.width)
			{
				copyLayer1->setPosition(ccp(layer1->getPosition().x + layer1->boundingBox().size.width, copyLayer1->getPosition().y));
			}

			//Layer 2
			layer2->setPosition(ccp(layer2->getPosition().x - (bgSpeed * 0.45), layer2->getPosition().y));
			copyLayer2->setPosition(ccp(copyLayer2->getPosition().x - (bgSpeed * 0.45), copyLayer2->getPosition().y));

			if (layer2->getPosition().x < -layer2->boundingBox().size.width)
			{
				layer2->setPosition(ccp(copyLayer2->getPosition().x + copyLayer2->boundingBox().size.width, layer2->getPosition().y));
			}
			if (copyLayer2->getPosition().x < -copyLayer2->boundingBox().size.width)
			{
				copyLayer2->setPosition(ccp(layer2->getPosition().x + layer2->boundingBox().size.width, copyLayer2->getPosition().y));
			}


			//Layer 3
			layer3->setPosition(ccp(layer3->getPosition().x - (bgSpeed * 0.8), layer3->getPosition().y));
			copyLayer3->setPosition(ccp(copyLayer3->getPosition().x - (bgSpeed * 0.8), copyLayer3->getPosition().y));

			if (layer3->getPosition().x < -layer3->boundingBox().size.width)
			{
				layer3->setPosition(ccp(copyLayer3->getPosition().x + copyLayer3->boundingBox().size.width, layer3->getPosition().y));
			}
			if (copyLayer3->getPosition().x < -copyLayer3->boundingBox().size.width)
			{
				copyLayer3->setPosition(ccp(layer3->getPosition().x + layer3->boundingBox().size.width, copyLayer3->getPosition().y));
			}



			//Layer 4
			layer4->setPosition(ccp(layer4->getPosition().x - (bgSpeed * 0.6), layer4->getPosition().y));
			copyLayer4->setPosition(ccp(copyLayer4->getPosition().x - (bgSpeed * 0.6), copyLayer4->getPosition().y));

			if (layer4->getPosition().x < -layer4->boundingBox().size.width)
			{
				layer4->setPosition(ccp(copyLayer4->getPosition().x + copyLayer4->boundingBox().size.width, layer4->getPosition().y));
			}
			if (copyLayer4->getPosition().x < -copyLayer4->boundingBox().size.width)
			{
				copyLayer4->setPosition(ccp(layer4->getPosition().x + layer4->boundingBox().size.width, copyLayer4->getPosition().y));
			}

			

			//Layer 8
			layer8->setPosition(ccp(layer8->getPosition().x - (bgSpeed * 0.1), layer8->getPosition().y));
			copyLayer8->setPosition(ccp(copyLayer8->getPosition().x - (bgSpeed * 0.1), copyLayer8->getPosition().y));

			if (layer8->getPosition().x < -layer8->boundingBox().size.width)
			{
				layer8->setPosition(ccp(copyLayer8->getPosition().x + copyLayer8->boundingBox().size.width, layer8->getPosition().y));
			}
			if (copyLayer8->getPosition().x < -copyLayer8->boundingBox().size.width)
			{
				copyLayer8->setPosition(ccp(layer8->getPosition().x + layer8->boundingBox().size.width, copyLayer8->getPosition().y));
			}

			//Layer 9
			layer9->setPosition(ccp(layer9->getPosition().x - (bgSpeed * 0.15), layer9->getPosition().y));
			copyLayer9->setPosition(ccp(copyLayer9->getPosition().x - (bgSpeed * 0.15), copyLayer9->getPosition().y));

			if (layer9->getPosition().x < -layer9->boundingBox().size.width)
			{
				layer9->setPosition(ccp(copyLayer9->getPosition().x + copyLayer9->boundingBox().size.width, layer9->getPosition().y));
			}
			if (copyLayer9->getPosition().x < -copyLayer9->boundingBox().size.width)
			{
				copyLayer9->setPosition(ccp(layer9->getPosition().x + layer9->boundingBox().size.width, copyLayer9->getPosition().y));
			}

			//Layer 10
			layer10->setPosition(ccp(layer10->getPosition().x - (bgSpeed * 0.1), layer10->getPosition().y));
			copyLayer10->setPosition(ccp(copyLayer10->getPosition().x - (bgSpeed * 0.1), copyLayer10->getPosition().y));

			if (layer10->getPosition().x < -layer10->boundingBox().size.width)
			{
				layer10->setPosition(ccp(copyLayer10->getPosition().x + copyLayer10->boundingBox().size.width, layer10->getPosition().y));
			}
			if (copyLayer10->getPosition().x < -copyLayer10->boundingBox().size.width)
			{
				copyLayer10->setPosition(ccp(layer10->getPosition().x + layer10->boundingBox().size.width, copyLayer10->getPosition().y));
			}
		}
	}

}
void GameScene::moveTrain()
{
	//Layer 6
	layer6->setPosition(ccp(layer6->getPosition().x + (trainSpeed * 0.5), layer6->getPosition().y));
	copyLayer6->setPosition(ccp(copyLayer6->getPosition().x + (trainSpeed * 0.5), copyLayer6->getPosition().y));

	if (layer6->getPosition().x > layer6->boundingBox().size.width)
	{
		layer6->setPosition(ccp(copyLayer6->getPosition().x - copyLayer6->boundingBox().size.width, layer6->getPosition().y));
	}
	if (copyLayer6->getPosition().x > copyLayer6->boundingBox().size.width)
	{
		copyLayer6->setPosition(ccp(layer6->getPosition().x - layer6->boundingBox().size.width, copyLayer6->getPosition().y));
	}
}
void GameScene::moveBoat()
{
	//Layer 6
	layer6->setPosition(ccp(layer6->getPosition().x + (trainSpeed * 0.5), layer6->getPosition().y));
	copyLayer6->setPosition(ccp(copyLayer6->getPosition().x + (trainSpeed * 0.5), copyLayer6->getPosition().y));

	if (layer6->getPosition().x > layer6->boundingBox().size.width)
	{
		layer6->setPosition(ccp(copyLayer6->getPosition().x - copyLayer6->boundingBox().size.width, layer6->getPosition().y));
	}
	if (copyLayer6->getPosition().x > copyLayer6->boundingBox().size.width)
	{
		copyLayer6->setPosition(ccp(layer6->getPosition().x - layer6->boundingBox().size.width, copyLayer6->getPosition().y));
	}
	//Layer 5
	layer5->setPosition(ccp(layer5->getPosition().x - (bgSpeed * 0.15), layer5->getPosition().y));
	copyLayer5->setPosition(ccp(copyLayer5->getPosition().x - (bgSpeed * 0.15), copyLayer5->getPosition().y));

	if (layer5->getPosition().x < -layer5->boundingBox().size.width)
	{
		layer5->setPosition(ccp(copyLayer5->getPosition().x + copyLayer5->boundingBox().size.width, layer5->getPosition().y));
	}
	if (copyLayer5->getPosition().x < -copyLayer5->boundingBox().size.width)
	{
		copyLayer5->setPosition(ccp(layer5->getPosition().x + layer5->boundingBox().size.width, copyLayer5->getPosition().y));
	}
	//Layer 7
	layer7->setPosition(ccp(layer7->getPosition().x - (bgSpeed * 0.2), layer7->getPosition().y));
	copyLayer7->setPosition(ccp(copyLayer7->getPosition().x - (bgSpeed * 0.2), copyLayer7->getPosition().y));

	if (layer7->getPosition().x < -layer7->boundingBox().size.width)
	{
		layer7->setPosition(ccp(copyLayer7->getPosition().x + copyLayer7->boundingBox().size.width, layer7->getPosition().y));
	}
	if (copyLayer7->getPosition().x < -copyLayer7->boundingBox().size.width)
	{
		copyLayer7->setPosition(ccp(layer7->getPosition().x + layer7->boundingBox().size.width, copyLayer7->getPosition().y));
	}
}
bool GameScene::onTouchBegan(Touch* touch, Event* event)
{
	if (!started)
	{
		started = true;
		player->pState = Player::IDLE;
		tapToPlay->setVisible(false);
		scoreInfo->setVisible(true);
		scoreSprite->setVisible(true);
		heart1->setVisible(true);
		heart2->setVisible(true);
		heart3->setVisible(true);
	}
	else
	{
		if (player->pState != Player::DEAD)
		{
			if (touch->getLocation().x > visibleSize.width / 2 && contactListener->numFootContacts >= 1)
			{
				player->oldState = player->pState;
				player->pState = Player::JUMP;
			}
			else if (touch->getLocation().x < visibleSize.width / 2)
			{
				player->pState = Player::CHARGE;
			}
		}
	}
	return true;
}

void GameScene::onTouchEnded(Touch* touch, Event* event)
{

	if (player->pState != Player::IDLE && player->pState != Player::START && player->pState != Player::DEAD)
	{
		player->pState = Player::IDLE;
	}
}

void GameScene::onTouchMoved(Touch* touch, Event* event)
{

}

void GameScene::onTouchCancelled(Touch* touch, Event* event)
{

	if (player->pState != Player::IDLE && player->pState != Player::START && player->pState != Player::DEAD)
	{
		player->pState = Player::IDLE;
	}

}

void GameScene::jump()
{
	if (contactListener->numFootContacts > 0)
	{
		player->pState = Player::IDLE;
		b2Vec2 vel = playerBody->GetLinearVelocity();
		vel.y = 10;
		playerBody->SetLinearVelocity(vel);
		if (!soundMuted)
		{
			audio->play2d("sounds/ProtagonistDeer_Jump2_low_volume.mp3");
		}
	}
}

void GameScene::dash(Vec2& _pos)
{

}

void GameScene::updateUI()
{
	if (player->tookDamage)
	{
		player->tookDamage = false;
		damaged = true;


	}
	else if (player->attacked)
	{
		player->attacked = false;
		bigImpactSprite->setTexture("effects/hitEffect3_1.png");
		bigImpactSprite->setPosition(ccp(player->getPositionX() + 60, player->getPositionY()));
		bigImpactSprite->setVisible(true);
		bigImpactSprite->runAction(playerImpact);
		if (!soundMuted)
		{
			audio->play2d("sounds/ProtagonistDeer_AttackCrash_low.mp3");
		}
	}

	switch (player->getHealth())
	{
	case 6:
	{
		hearts.at(0)->setTexture("Heart_Full.png");
		hearts.at(1)->setTexture("Heart_Full.png");
		hearts.at(2)->setTexture("Heart_Full.png");
	}
		break;
	case 5:
	{
		hearts.at(0)->setTexture("Heart_Full.png");
		hearts.at(1)->setTexture("Heart_Full.png");
		hearts.at(2)->setTexture("Heart_Half.png");
	}
		break;
	case 4:
	{
		hearts.at(0)->setTexture("Heart_Full.png");
		hearts.at(1)->setTexture("Heart_Full.png");
		hearts.at(2)->setTexture("Heart_Empty.png");
	}
		break;
	case 3:
	{
		hearts.at(0)->setTexture("Heart_Full.png");
		hearts.at(1)->setTexture("Heart_Half.png");
		hearts.at(2)->setTexture("Heart_Empty.png");
	}
		break;
	case 2:
	{
		hearts.at(0)->setTexture("Heart_Full.png");
		hearts.at(1)->setTexture("Heart_Empty.png");
		hearts.at(2)->setTexture("Heart_Empty.png");
	}
		break;
	case 1:
	{
		hearts.at(0)->setTexture("Heart_Half.png");
		hearts.at(1)->setTexture("Heart_Empty.png");
		hearts.at(2)->setTexture("Heart_Empty.png");
	}
		break;
	case 0:
	{
		hearts.at(0)->setTexture("Heart_Empty.png");
		hearts.at(1)->setTexture("Heart_Empty.png");
		hearts.at(2)->setTexture("Heart_Empty.png");
	}
		break;
	}

	if (player->getHealth() < 0)
	{
		hearts.at(0)->setTexture("Heart_Empty.png");
		hearts.at(1)->setTexture("Heart_Empty.png");
		hearts.at(2)->setTexture("Heart_Empty.png");
	}

}

void GameScene::moveEnemies()
{
	if (player->pState != Player::DEAD)
	{
		for (std::vector<b2Body*>::iterator it = enemyBodies.begin(); it != enemyBodies.end(); it++)
		{
			if (*it != nullptr)
			{
				b2Body* b = *it;
				Enemy* e = (Enemy*)b->GetUserData();

				if (b->GetPosition().x < -100 / SCALE_RATIO)
				{
					bodiesToDestroy.push_back(b);
					e->charging = false;
					e->scored = false;
					e->gaveHealth = false;
					
				}
				else if (b->GetPosition().x > visibleSize.width / SCALE_RATIO && e->dead)
				{
					bodiesToDestroy.push_back(b);
					e->charging = false;
					e->scored = false;
					e->gaveHealth = false;
					CCLOG("body rip");
				}
				else if (b->GetPosition().x < (visibleSize.width - 250) / SCALE_RATIO && !e->dead && e->eState == Enemy::IDLE && !e->charging)
				{
					int index = enemyAnimate->getCurrentFrameIndex() + 1;
					if (index > 6)
					{
						index = 1;
					}
					else
					{
						index++;
					}
					

					char str[100] = { 0 };
					Vector<SpriteFrame*> frames;

					for (int i = 1; i <= 6; i++)
					{
						if (index > 6)
						{
							index = 1;
						}

						sprintf(str, "small/enemy_small_%i_ram.png", index);
						auto frame = SpriteFrame::create(str, Rect(0, 0, 192, 128));
						frames.pushBack(frame);

						index++;
					}

					auto moveAnimation = Animation::createWithSpriteFrames(frames, 0.07f);
					smallChargeAnimate = Animate::create(moveAnimation);
					smallChargeAnimate->retain();

					if (e->eState == Enemy::IDLE)
					{
						e->stopAllActions();
					}
					
					//enemyAnimate->reverse();
					e->runAction(RepeatForever::create(smallChargeAnimate));
					e->charging = true;
				}
				else if (b->GetPosition().x > visibleSize.width / 2 / SCALE_RATIO)
				{
					if (!e->dead)
					{
						if (!speed)
						{
							if (e->eState == Enemy::FEMALE || e->charging)
							{
								b->SetLinearVelocity(b2Vec2(-10, 0));
							}

							else
							{
								b->SetLinearVelocity(b2Vec2(-8, 0));
							}
						}

						else
						{
							if (e->eState == Enemy::FEMALE || e->charging)
							{
								b->SetLinearVelocity(b2Vec2(-15, 0));
							}

							else
							{
								b->SetLinearVelocity(b2Vec2(-12, 0));
							}
						}

					}
				}
				else if (e->eState == Enemy::CHARGE)
				{
					if (e->getPosition().x - 50 < player->getPosition().x && !e->scored)
					{
						player->score++;
						e->scored = true;

					}
				}
			}
		}
	}
}

void GameScene::setUI()
{
	heart1 = Sprite::create("Heart_Full.png");
	heart2 = Sprite::create("Heart_Full.png");
	heart3 = Sprite::create("Heart_Full.png");

	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	hearts.push_back(heart1);
	heart1->setPosition(ccp(((visibleSize.width + origin.x) / 2) - 60, 530));
	heart1->setScale(.8f);
	heart1->setVisible(false);
	this->addChild(heart1);

	hearts.push_back(heart2);
	heart2->setPosition(ccp(((visibleSize.width + origin.x) / 2), 530));
	heart2->setScale(.8f);
	heart2->setVisible(false);
	this->addChild(heart2);

	hearts.push_back(heart3);
	heart3->setPosition(ccp(((visibleSize.width + origin.x) / 2) + 60, 530));
	heart3->setScale(.8f);
	heart3->setVisible(false);
	this->addChild(heart3);


}
void GameScene::restart(cocos2d::Ref *sender)
{
	auto scene = GameScene::createScene();

	started = false;

	Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}
void GameScene::pauseGame(cocos2d::Ref *sender)
{
	if (!paused)
	{
		paused = true;
	}
	else
	{
		paused = false;
	}
}
void GameScene::pause()
{
	if (!paused)
	{
		paused = true;
	}
}
void GameScene::handlePlayerStates()
{
	switch (player->pState)
	{
	case Player::JUMP:
		GameScene::jump();
		break;
	case Player::CHARGE:
		// check if in ground
		if (contactListener->numFootContacts > 0)
		{
			if (!player->landed)
			{
				//Landing effect poof
				landingDust->setTexture("effects/landing_dust_1.png");
				landingDust->setVisible(true);
				landingDust->runAction(landingEffect);
				if (!soundMuted)
				{
					audio->play2d("sounds/ProtagonistDeer_Land2.mp3");
				}
				player->landed = true;
			}
		

			player->charge();
			if (damaged)
			{
				damaged = false;
			}

		}
		else
		{
			player->landed = false;
			player->stopAllActions();
			player->setTexture("Protagonist_Deer_Dash.png");
		}
		break;
	case Player::IDLE:
		if (contactListener->numFootContacts > 0)
		{
			if (!player->landed)
			{
				//Landing effect poof
				landingDust->setVisible(true);
				landingDust->runAction(landingEffect);
				if (!soundMuted)
				{
					audio->play2d("sounds/ProtagonistDeer_Land2.mp3");
				}
				player->landed = true;
				
			}
			
			if (!damaged)
			{
				player->run();
			}

		}
		else if (contactListener->numFootContacts == 0)
		{
			player->landed = false;
			if (!damaged)
			{
				player->jump();
			}
		}
		break;
	case Player::DEAD:
		// set replay button visible and stop player animation
		pauseButton->setVisible(false);
		scoreInfo->setColor(Color3B(0, 0, 0));
		if (player->diedToBig)
		{
			int r;
			if (!randomed)
			{
				r = rand() % 5;
				

				switch (r)
				{
				case 0:
					gameOverTip->setString("Jump over the big ones!");
					break;
				case 1:
					gameOverTip->setString("Those BIG antlers are deadly..");
					break;
				case 2:
					gameOverTip->setString("Tap right side of the screen to jump.");
					break;
				case 3:
					gameOverTip->setString("Female deers do not like violence!");
					break;
				case 4:
					gameOverTip->setString("Jump while charging,\n\nyou will land further!");
				}
				randomed = true;
			}
		}
		else if (player->diedToSmall)
		{
			int r;
			if (!randomed)
			{
				r = rand() % 3;
				
				switch (r)
				{
				case 0:
					gameOverTip->setString("Charge at the small ones!");
					break;
				case 1:
					gameOverTip->setString("Show small deers\n\nwho is the BOSS.");
					break;
				case 2:
					gameOverTip->setString("Hold left side of\n\nthe screen to charge.");
					break;
				}
				randomed = true;
			}

		}
		gameOverSprite->setVisible(true);

		if (player->numberOfRunningActions() == 0)
		{
			gameOverTip->setColor(Color3B(255, 255, 255));
			gameOverTip->setVisible(true);
		}

		layer1->setVisible(false);
		layer2->setVisible(false);
		layer3->setVisible(false);
		layer4->setVisible(false);
		layer5->setVisible(false);
		layer6->setVisible(false);
		
		
		copyLayer1->setVisible(false);
		copyLayer2->setVisible(false);
		copyLayer3->setVisible(false);
		copyLayer4->setVisible(false);
		copyLayer5->setVisible(false);
		if (randomBG >= 5)
		{
			copyLayer6->setVisible(false);
			copyLayer7->setVisible(false);
			copyLayer8->setVisible(false);
			copyLayer9->setVisible(false);
			copyLayer10->setVisible(false);
			

			layer7->setVisible(false);
			layer8->setVisible(false);
			layer9->setVisible(false);
			layer10->setVisible(false);
			layer11->setVisible(false);
			
			if (randomBG == 6)
			{
				copyLayer11->setVisible(false);
				layer12->setVisible(false);
			}
		}
		
		bigImpactSprite->setVisible(false);
		player->dead();

		if (score > bestScore)
		{
			def->setIntegerForKey(HIGH_SCORE, score);
			def->flush();
			bestScore = def->getIntegerForKey(HIGH_SCORE);
			std::stringstream ss;

			ss << bestScore;

			std::string tp = ss.str();
			GameScene::bestScoreInfo->setString(tp);
		}
		//scoreInfo->setPosition(ccp(915, 525));
		scoreSprite->setVisible(false);
		bestScoreInfo->setVisible(true);
		scoreBoard->setVisible(true);

		if (!loseSound)
		{
			if (!soundMuted)
			{
				audio->play2d("sounds/Loss2_low.mp3");
			}
			loseSound = true;
		}

		// Stop animation actions from current enemies
		for (std::vector<Enemy*>::iterator it = enemyVec.begin(); it != enemyVec.end(); ++it)
		{
			Enemy* temp = *it;
			temp->stopAllActions();
			temp->setVisible(false);
			temp = nullptr;
		}

		break;
	}
}
void GameScene::handleDifficulty()
{
	if (player->score > 4 && player->score < 10 && difficulty < 1)
	{
		difficulty = 1;
		if (difficulty == 1)
		{
			this->schedule(schedule_selector(GameScene::resetEnemy), 3.0f);
		}
	}
	else if (player->score > 9 && player->score < 20 && difficulty < 2)
	{
		difficulty = 2;
		if (difficulty == 2)
		{
			this->schedule(schedule_selector(GameScene::resetEnemy), 2.5f);
		}
	}
	else if (player->score > 19 && player->score < 30 && difficulty < 3)
	{
		difficulty = 3;
		if (difficulty == 3)
		{
			this->schedule(schedule_selector(GameScene::resetEnemy), 2.0f);
		}
	}
	else if (player->score > 29 && player->score < 40 && difficulty < 4)
	{
		difficulty = 4;
		if (difficulty == 4)
		{
			this->schedule(schedule_selector(GameScene::resetEnemy), 1.5f);
		}
	}
	else if (player->score > 39 && player->score < 50 && difficulty < 5)
	{
		difficulty = 5;
		if (difficulty == 5)
		{
			this->schedule(schedule_selector(GameScene::resetEnemy), 1.0f);
		}
	}
}
void GameScene::goToMainMenu(cocos2d::Ref *sender)
{
	started = false;
	Director::getInstance()->end();
}

void GameScene::saveData(int _score)
{
	std::string path = CCFileUtils::sharedFileUtils()->getWritablePath() + "data.dat";
	std::ofstream file;
	file.open(path);
	file.write((char*)&_score, sizeof(_score));
	file.close();

}
void GameScene::loadData(int _score)
{
	std::string path = CCFileUtils::sharedFileUtils()->getWritablePath() + "data.dat";
	std::ifstream file;
	file.open(path, std::ifstream::binary);
	file.read((char*)&_score, sizeof(_score));
	file.close();
}
void GameScene::muteSound(cocos2d::Ref *sender)
{
	if (!soundMuted)
	{
		soundMuted = true;
		muteButton->setNormalImage(Sprite::create("UI/Sounds_OFF.png"));
		muteButton->setSelectedImage(Sprite::create("UI/Sounds_OFF.png"));
		def->setBoolForKey(MUTED, true);
		def->flush();
	}
	else
	{
		soundMuted = false;
		muteButton->setNormalImage(Sprite::create("UI/Sounds_ON.png"));
		muteButton->setSelectedImage(Sprite::create("UI/Sounds_ON.png"));
		def->setBoolForKey(MUTED, false);
		def->flush();
	}
}
void GameScene::setMenu()
{
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	playButton = MenuItemImage::create("UI/play_button.png", "UI/play_button_pressed.png", CC_CALLBACK_1(GameScene::restart, this));
	playButton->setPosition(Point(visibleSize.width / 2 + origin.x, (visibleSize.height / 2 + origin.y) - 50));
	playButton->setVisible(false);
	playButton->setLocalZOrder(9);

	pauseButton = MenuItemImage::create("UI/Pause.png", "UI/Pause_pressed.png", CC_CALLBACK_1(GameScene::pauseGame, this));
	pauseButton->setPosition(ccp(40, 540));
	pauseButton->setLocalZOrder(9);
	pauseButton->setVisible(false);

	quitButton = MenuItemImage::create("UI/Quit_Button.png", "UI/Quit_Button_Pressed.png", CC_CALLBACK_1(GameScene::goToMainMenu, this));
	quitButton->setPosition(ccp(visibleSize.width / 2 + origin.x, (visibleSize.height / 2 + origin.y) - 180));
	quitButton->setVisible(false);
	quitButton->setLocalZOrder(9);

	resumeButton = MenuItemImage::create("UI/Resume_Button.png", "UI/Resume_Button_Pressed.png", CC_CALLBACK_1(GameScene::pauseGame, this));
	resumeButton->setPosition(ccp(visibleSize.width / 2 + origin.x, (visibleSize.height / 2 + origin.y) - 40));
	resumeButton->setVisible(false);
	resumeButton->setLocalZOrder(9);

	if (soundMuted)
	{
		muteButton = MenuItemImage::create("UI/Sounds_OFF.png", "UI/Sounds_OFF.png", CC_CALLBACK_1(GameScene::muteSound, this));
	}
	else
	{
		muteButton = MenuItemImage::create("UI/Sounds_ON.png", "UI/Sounds_ON.png", CC_CALLBACK_1(GameScene::muteSound, this));
	}

	muteButton->setPosition(ccp(970, 50));
	muteButton->setVisible(false);
	muteButton->setLocalZOrder(9);


	menu = Menu::create(playButton, pauseButton, quitButton, resumeButton, muteButton, NULL);
	menu->setPosition(Point::ZERO);
	menu->setLocalZOrder(9);
	this->addChild(menu);
}

GameScene::~GameScene()
{
	enemyPool->release();
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	TextureCache::getInstance()->removeUnusedTextures();

}

