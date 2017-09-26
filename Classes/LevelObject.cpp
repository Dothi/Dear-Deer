#include "LevelObject.h"

const float kPixelsPerMeter = 32.0f;
const float kGravity = -kPixelsPerMeter / 0.7f;
void LevelObject::addBodyToWorld(b2World* world)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(this->getPositionX() / kPixelsPerMeter, this->getPositionY() / kPixelsPerMeter);
	bodyDef.userData = this;
	bodyDef.fixedRotation = true;
	this->body = world->CreateBody(&bodyDef);
}
void LevelObject::addCircularFixtureToBody(float radius)
{
	b2CircleShape shape;
	shape.m_radius = radius * this->getScale();
	this->createFixture(&shape);
}

void LevelObject::addRectangularFixtureToBody(float width, float height)
{
	b2PolygonShape shape;
	shape.SetAsBox(
		width * this->getScale(),
		height * this->getScale()
		);
	this->createFixture(&shape);
}

enum
{
	kFilterCategoryLevel = 0x01,
	kFilterCategorySolidObject = 0x02,
	kFilterCategoryNonSolidObject = 0x04
};

void LevelObject::createFixture(b2Shape* shape)
{
	// note that friction, etc. can be modified later by looping
	// over the body's fixtures and calling fixture->SetFriction()
	b2FixtureDef fixtureDef;
	fixtureDef.shape = shape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.7f;
	fixtureDef.restitution = 0.1f;
	fixtureDef.filter.categoryBits = kFilterCategorySolidObject;
	fixtureDef.filter.maskBits = 0xffff;
	this->body->CreateFixture(&fixtureDef);
}