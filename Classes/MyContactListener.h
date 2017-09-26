#ifndef MY_CONTACT_LISTENER_H
#define MY_CONTACT_LISTENER_H

#include "Box2D\Box2D.h"
#include "cocos2d.h"
#include "Player.h"
#include "Enemy.h"

class MyContactListener : public b2ContactListener
{
public:
	int numFootContacts;

	MyContactListener();
	~MyContactListener();
	virtual void BeginContact(b2Contact* contact);
	virtual void EndContact(b2Contact* contact);
private:
	
};

#endif