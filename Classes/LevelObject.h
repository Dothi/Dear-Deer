#ifndef LEVELOBJECT_H
#define LEVELOBJECT_H
#include "cocos2d.h"
#include "Box2D\Box2D.h"

class LevelObject : public cocos2d::Node
{
private:
	typedef Node super;
	typedef LevelObject self;
protected:
	b2Body* body;
	cocos2d::Sprite* sprite;
public:
	LevelObject();
	virtual ~LevelObject();
	void addRectangularFixtureToBody(float width, float height);
	void addCircularFixtureToBody(float radius);
	void createFixture(b2Shape* shape);
	void addBodyToWorld(b2World* world);
};


#endif