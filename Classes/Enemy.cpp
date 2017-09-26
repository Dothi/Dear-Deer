#include "Enemy.h"

Enemy::Enemy() : dead(false), gaveHealth(false), charging(false), scored(false)
{
	
}

Enemy::~Enemy()
{

}

void Enemy::update(float dt)
{
	if (dead && eState == Enemy::IDLE)
	{
		this->stopAllActions();
		this->setTexture("small/enemy_small_damage_taken.png");
		body->SetLinearVelocity(b2Vec2(12, 5));
		
	}
	
}

Enemy* Enemy::createWithTexture(Texture2D* _tex, const Rect& _rect)
{
	Enemy* enemy = new Enemy();

	if (enemy && enemy->initWithTexture(_tex, _rect))
	{
		enemy->autorelease();
		return enemy;
	}
	CC_SAFE_DELETE(enemy);
	return NULL;
}

Enemy* Enemy::create(const std::string& filename)
{
	Enemy* enemy = new Enemy();

	if (enemy && enemy->initWithFile(filename))
	{
		enemy->autorelease();
		return enemy;
	}
	CC_SAFE_DELETE(enemy);
	return NULL;
}

