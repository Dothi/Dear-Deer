#ifndef PLAYER_H
#define PLAYER_H
#include "cocos2d.h"
#include "Box2D\Box2D.h"
#include "Enemy.h"
#include "SimpleAudioEngine.h"


using namespace cocos2d;

class Player : public cocos2d::Sprite
{

public:
	Player();
	virtual ~Player();
	
	int score;

	bool tookDamage;
	bool attacked;
	bool smallEnemyDmg;
	bool bigEnemyDmg;
	bool gotHealth;
	bool moving;
	bool jumping;
	bool charging;
	bool landed;
	bool diedToBig;
	bool diedToSmall;
	bool death;

	float timer;

	enum playerState
	{
		START,
		JUMP,
		IDLE,
		CHARGE,
		DEAD
	};

	static Player* createWithTexture(Texture2D* texture, const Rect& rect);
	static Player* create(void);

	b2Body* body;
	playerState pState;
	playerState oldState;

	Animate* idleAnimate;
	Animate* runAnimate;
	Animate* chargeAnimate;
	Animate* jumpAnimate;
	Animate* deathAnimate;
	
	void update(float);

	void run();
	void charge();
	void idle();
	void jump();
	void dead();

	void handleContact(Player*, Enemy*);
	void handleIdleContact(Enemy*);
	void handleChargeContact(Enemy*);
	void handleJumpContact(Enemy*);
	
	
	int getHealth(){ return health; }
	
private:
	int health;
	void initPlayer();

};

#endif