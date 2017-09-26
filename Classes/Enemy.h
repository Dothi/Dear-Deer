#ifndef ENEMY_H
#define ENEMY_H

#include "cocos2d.h"
#include "Box2D\Box2D.h"


using namespace cocos2d;

class Enemy : public cocos2d::Sprite
{
public:
	Enemy();
	virtual ~Enemy();
	bool gaveHealth;
	bool dead;
	bool charging;
	bool scored;
	
	enum enemyState
	{
		IDLE,
		CHARGE,
		FEMALE,
		RANDOMNUM
	};

	enemyState eState;
	b2Body* body;
	
	void update(float dt);
	static Enemy* createWithTexture(Texture2D* texture, const Rect& rect);
	static Enemy* create(const std::string& filename);
private:
};

#endif