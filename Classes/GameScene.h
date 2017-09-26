#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "Box2D\Box2D.h"
#include "AudioEngine.h"
#include "B2DebugDrawLayer.h"
#include "MyContactListener.h"
#include "Player.h"
#include "Enemy.h"
#include "FShake.h"
#include "AudioEngine.h"
#include "MainMenuScene.h"
#include <iostream>
#include <fstream>
#include <ostream>

using namespace cocos2d;

class GameScene : public cocos2d::Layer, public b2ContactListener
{
public:
	~GameScene();
    static cocos2d::Scene* createScene();
	CCUserDefault *def;
	experimental::AudioEngine* audio;
	int bestScore;
	int score;
	int randomBG;
	int difficulty;
	int scroller;
	cocos2d::Label* scoreInfo;
	cocos2d::Label* bestScoreInfo;
	cocos2d::Label* gameOverTip;
    virtual bool init();
	bool paused;
	bool isScrolling;
	bool randomed;
	bool loseSound;
	bool increaseAlpha;
	bool saved;
	bool damaged;
	bool speed;
	bool soundMuted;
	bool scored;
	float timer;
	float bgSpeed;
	float trainSpeed;
	float breathTimer;
	b2World* world;
	B2DebugDrawLayer* debugDrawLayer;
	MyContactListener* contactListener;
	float deltaTime;
	float alpha;
	cocos2d::Sprite* bk1;
	cocos2d::Sprite* bk2;
	Sprite* scoreSprite;
	Sprite* heart;
	Sprite* heart1;
	Sprite* heart2;
	Sprite* heart3;
	CCSprite* landingDust;
	CCSprite* scoreEff;
	CCSprite* bigImpactSprite;
	CCSprite* gameOverSprite;
	CCSprite* tapToPlay;
	CCSprite* pausedText;
	CCSprite* title;
	CCSprite* scoreBoard;

	CCSprite* layer1;
	CCSprite* copyLayer1;
	CCSprite* layer2;
	CCSprite* copyLayer2;
	CCSprite* layer3;
	CCSprite* copyLayer3;
	CCSprite* layer4;
	CCSprite* copyLayer4;
	CCSprite* layer5;
	CCSprite* copyLayer5;
	CCSprite* layer6;
	CCSprite* copyLayer6;
	CCSprite* layer7;
	CCSprite* copyLayer7;
	CCSprite* layer8;
	CCSprite* copyLayer8;
	CCSprite* layer9;
	CCSprite* copyLayer9;
	CCSprite* layer10;
	CCSprite* copyLayer10;
	CCSprite* layer11;
	CCSprite* copyLayer11;
	CCSprite* layer12;

	//CCSprite* copyLayer6;


	Player* player;
	Menu* menu;
	MenuItemImage* playButton;
	MenuItemImage* pauseButton;
	MenuItemImage* quitButton;
	MenuItemImage* resumeButton;
	MenuItemImage* muteButton;
	float playerX;
	float playerY;
	b2Body* playerBody;
	b2PolygonShape playerShape;
	b2BodyDef playerBodyDef;
	b2Body* enemyBody;
	b2PolygonShape enemyShape;
	b2BodyDef enemyBodyDef;
	RepeatForever* runAnimAction;
	std::vector<Sprite*> hearts;
	SpriteBatchNode* enemyRun;
	SpriteBatchNode* enemyIdle;
	SpriteBatchNode* enemyFemale;
	Animate* enemyAnimate;
	Animate* smallChargeAnimate;
	Animate* femaleAnimate;
	Animate* charAnimate;
	Animate* animate;
	Animate* heartEffect;
	Animate* scoreEffect;
	Animate* landingEffect;
	Animate* bigImpact;
	Animate* smallImpact;
	Animate* playerImpact;
	void startScreen();
	void setMenu();
	void definePlayer();
	void handlePlayerStates();
	void handleDifficulty();
	void updatePlayerAndEnemy(float dt);
	void handlePause();
	void initEffects();
	void initEnemyAnims();
	void saveData(int score);
	void loadData(int score);
	
	virtual bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
	virtual void onTouchEnded(cocos2d::Touch*, cocos2d::Event*);
	virtual void onTouchMoved(cocos2d::Touch*, cocos2d::Event*);
	virtual void onTouchCancelled(cocos2d::Touch*, cocos2d::Event*);

	void addWall(float w, float h, float px, float py);
	void setTouchEvents();
	void setWorldAndDebugLayer();
	void setBackgrounds(int random);
	void setPlayerRunAnim();
	void updatePhysics(float dt);
	void createPools();
	void resetEnemy(float dt);
	void defineEnemy(Enemy* _enemy);
	void setUI();
	void updateUI();
	
	void update(float dt);
	void pause();
	void restart(cocos2d::Ref *sender);
	void pauseGame(cocos2d::Ref *sender);
	void goToMainMenu(cocos2d::Ref *sender);
	void muteSound(cocos2d::Ref *sender);
	
    CREATE_FUNC(GameScene);

private:
	
	cocos2d::Size visibleSize;
	cocos2d::Label* labelTouchInfo;
	

	CCArray* enemyPool;
	CCArray* enemies;
	
	int enemyPoolIndex;
	std::vector<b2Body*> enemyBodies;
	std::vector<Enemy*> enemyVec;
	std::vector<b2Body*> bodiesToDestroy;
	void moveBg(float dt);
	void moveTrain();
	void moveBoat();
	void jump();
	void dash(cocos2d::Vec2& pos);
	void moveEnemies();
	
	

	

	
};

#endif 
