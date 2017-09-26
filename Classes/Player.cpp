#include "Player.h"
#include "Definitions.h"



Player::Player() : health(6), tookDamage(false), moving(false), charging(false), score(0), landed(true),
diedToBig(false), diedToSmall(false), gotHealth(false), death(false), smallEnemyDmg(false), bigEnemyDmg(false), attacked(false), timer(0)
{

}
Player::~Player()
{
	// REMEMBER TO RETAIN IDLE AND CHARGE WHEN SET

	CC_SAFE_RELEASE(idleAnimate);
	CC_SAFE_RELEASE(runAnimate);
	CC_SAFE_RELEASE(chargeAnimate);
	CC_SAFE_RELEASE(deathAnimate);
	//CC_SAFE_RELEASE(jumpAnimate);
}

Player* Player::createWithTexture(Texture2D* _tex, const Rect& _rect)
{
	Player* player = new Player();

	if (player && player->initWithTexture(_tex, _rect))
	{
		player->autorelease();
		player->initPlayer();
		return player;
	}
	CC_SAFE_DELETE(player);
	return NULL;
}
Player* Player::create()
{
	Player * player = new Player();
	if (player && player->initWithFile("run/run_1.png"))
	{
		player->autorelease();
		player->initPlayer();
		return player;
	}

	CC_SAFE_DELETE(player);
	return NULL;
}

void Player::initPlayer()
{
	moving = false;

	char str[100] = { 0 };

	// Idle
	Vector<SpriteFrame*> idleAnimFrames(30);

	for (int i = 1; i <= 30; i++)
	{
		sprintf(str, "idle/idle_%i.png", i);
		auto frame = SpriteFrame::create(str, Rect(0, 0, 192, 192));
		idleAnimFrames.pushBack(frame);
	}

	auto idleAnimation = Animation::createWithSpriteFrames(idleAnimFrames, 0.15f);
	idleAnimate = Animate::create(idleAnimation);
	idleAnimate->retain();
	this->runAction(RepeatForever::create(idleAnimate)); // Starting animation

	// Run
	Vector<SpriteFrame*> runAnimFrames(6);

	for (int i = 1; i <= 6; i++)
	{
		sprintf(str, "run/run_%i.png", i);
		auto frame = SpriteFrame::create(str, Rect(0, 0, 192, 192));
		runAnimFrames.pushBack(frame);
	}

	auto moveAnimation = Animation::createWithSpriteFrames(runAnimFrames, 0.1f);
	runAnimate = Animate::create(moveAnimation);
	runAnimate->retain();

	// Charge
	Vector<SpriteFrame*> chargeAnimFrames(6);

	for (int i = 1; i <= 6; i++)
	{
		sprintf(str, "charge/charge_%i.png", i);
		auto frame = SpriteFrame::create(str, Rect(0, 0, 208, 208));
		chargeAnimFrames.pushBack(frame);
	}

	auto chargeAnimation = Animation::createWithSpriteFrames(chargeAnimFrames, 0.1f);
	chargeAnimate = Animate::create(chargeAnimation);
	chargeAnimate->retain();

	// Death
	Vector<SpriteFrame*> deathAnimFrames(6);

	for (int i = 1; i <= 8; i++)
	{
		sprintf(str, "death/death_white_%i.png", i);
		auto frame = SpriteFrame::create(str, Rect(0, 0, 192, 192));
		deathAnimFrames.pushBack(frame);
	}

	auto deathAnimation = Animation::createWithSpriteFrames(deathAnimFrames, 0.1f);
	deathAnimate = Animate::create(deathAnimation);
	deathAnimate->retain();

	// Jump
	/*Vector<SpriteFrame*> jumpFrame(1);

	sprintf(str, "jump_1.png", 1);
	auto frame = SpriteFrame::create(str, Rect(0, 0, 192, 192));
	jumpFrame.pushBack(frame); 
	auto jumpAnimation = Animation::createWithSpriteFrames(jumpFrame, 0.1f);
	jumpAnimate = Animate::create(jumpAnimation);
	jumpAnimate->retain();*/

}

void Player::update(float dt)
{
	if (health <= 0)
	{
		pState = Player::DEAD;
	}

}
void Player::run()
{
	if (!moving && pState != Player::DEAD)
	{
		int curr = chargeAnimate->getCurrentFrameIndex() + 1;


		if (curr > 6)
		{
			curr = 1;
		}
		else
		{
			curr++;
		}

		runAnimate->release();

		char str[100] = { 0 };

		Vector<SpriteFrame*> runFrames(6);

		for (int i = 1; i <= 6; i++)
		{
			if (curr > 6)
			{
				curr = 1;
			}
			sprintf(str, "run/run_%i.png", curr);
			auto frame = SpriteFrame::create(str, Rect(0, 0, 192, 192));
			runFrames.pushBack(frame);
			curr++;
		}

		auto moveAnimation = Animation::createWithSpriteFrames(runFrames, 0.1f);
		runAnimate = Animate::create(moveAnimation);
		runAnimate->retain();

		this->stopAllActions();
		this->runAction(RepeatForever::create(runAnimate));
		moving = true;
		charging = false;
	}
}

void Player::charge()
{
	if (!charging && pState != Player::DEAD)
	{
		int curr = runAnimate->getCurrentFrameIndex() + 1;

		if (curr > 6)
		{
			curr = 1;
		}
		else
		{
			curr++;
		}

		chargeAnimate->release();

		char str[100] = { 0 };

		Vector<SpriteFrame*> chargeFrames(6);

		for (int i = 1; i <= 6; i++)
		{
			if (curr > 6)
			{
				curr = 1;
			}

			sprintf(str, "charge/charge_%i.png", curr);
			auto frame = SpriteFrame::create(str, Rect(0, 0, 208, 208));
			chargeFrames.pushBack(frame);

			curr++;
		}

		auto moveAnimation = Animation::createWithSpriteFrames(chargeFrames, 0.1f);
		chargeAnimate = Animate::create(moveAnimation);
		chargeAnimate->retain();
		this->stopAllActions();
		this->runAction(RepeatForever::create(chargeAnimate));
		moving = false;
		charging = true;
	}
}
void Player::dead()
{
	if (!death)
	{
		this->stopAllActions();
		this->runAction(deathAnimate);
		death = true;
	}
	else
	{
		if (this->numberOfRunningActions() == 0)
		{
			this->setVisible(false);
		}
	}
	
}
void Player::idle()
{

}
void Player::jump()
{
	this->stopAllActions();
	//this->runAction(RepeatForever::create(jumpAnimate));
	this->setTexture("Protagonist_Deer_Jump.png");
	if (moving)
	{
		moving = false;
	}
}
void Player::handleContact(Player* player, Enemy* enemy)
{
	if (player->pState == Player::IDLE)
	{
		Player::handleIdleContact(enemy);
	}
	else if (player->pState == Player::CHARGE)
	{
		Player::handleChargeContact(enemy);
	}
	else if (player->pState == Player::JUMP)
	{
		Player::handleJumpContact(enemy);
	}

	//body->SetLinearVelocity(b2Vec2(0, 15.0f));
}

void Player::handleIdleContact(Enemy* enemy)
{
	// Check enemy state
	if (enemy->eState == Enemy::FEMALE && !enemy->gaveHealth)
	{
		// GAIN 2 heart
		if (health < 5)
		{
			health += 2;
			gotHealth = true;
			CCLOG("Health: %i", health);
			enemy->gaveHealth = true;
		}
		else if (health > 4 && health < 6)
		{
			health++;
			gotHealth = true;
			CCLOG("Health: %i", health);
			enemy->gaveHealth = true;
		}
	}
	else if (enemy->eState == Enemy::CHARGE)
	{
		// LOSE 1 heart
		if (health > 0)
		{
			health -= 2;

			if (health <= 0)
			{
				diedToBig = true;
			}
			bigEnemyDmg = true;
			tookDamage = true;
			enemy->scored = true;
			CCLOG("Health: %i", health);
			
		}
		
	}
	else if (enemy->eState == Enemy::IDLE)
	{
		if (health > 0)
		{
			health--;
			smallEnemyDmg = true;
			if (health <= 0)
			{
				diedToSmall = true;
			}

			tookDamage = true;
			CCLOG("Health: %i", health);
			
		}
	}
}
void Player::handleChargeContact(Enemy* enemy)
{
	if (enemy->eState == Enemy::FEMALE)
	{
		
	}
	else if (enemy->eState == Enemy::CHARGE)
	{
		// LOSE 1 heart
		if (health > 0)
		{
			health -= 2;
			if (health <= 0)
			{
				diedToBig = true;
			}
			tookDamage = true;
			bigEnemyDmg = true;
			enemy->scored = true;
		}
	}
	else if (enemy->eState == Enemy::IDLE)
	{
		// kill enemy
		enemy->dead = true;
		score++;
		attacked = true;
		
	}
}
void Player::handleJumpContact(Enemy* enemy)
{
	if (enemy->eState == Enemy::FEMALE && enemy->gaveHealth)
	{
		// GAIN 1 heart
		if (health < 5)
		{
			health += 2;
			gotHealth = true;
			CCLOG("Health: %i", health);
			enemy->gaveHealth = true;
		}
		else if (health > 4 && health < 6)
		{
			health++;
			gotHealth = true;
			CCLOG("Health: %i", health);
			enemy->gaveHealth = true;
		}
		enemy->gaveHealth = true;
	}
	else if (enemy->eState == Enemy::CHARGE)
	{
		// LOSE 1 heart
		if (health > 0)
		{
			health -= 2;
			if (health <= 0)
			{
				diedToBig = true;
			}
			bigEnemyDmg = true;
			tookDamage = true;
			enemy->scored = true;
		}
	}
	else if (enemy->eState == Enemy::IDLE)
	{
		if (health > 0)
		{
			health--;
			smallEnemyDmg = true;
			if (health <= 0)
			{
				diedToSmall = true;
			}
			tookDamage = true;
		}
	}
}

