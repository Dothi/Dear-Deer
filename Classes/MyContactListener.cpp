#include "MyContactListener.h"

MyContactListener::MyContactListener() 
{
	numFootContacts = 0;
}

MyContactListener::~MyContactListener()
{

}

void MyContactListener::BeginContact(b2Contact* contact)
{
	//check if fixture A was the foot sensor
	void* fixtureUserData = contact->GetFixtureA()->GetUserData();
	b2Fixture* other = contact->GetFixtureB();
	if ((int)fixtureUserData == 3 && !other->IsSensor())
		numFootContacts++;
	//check if fixture B was the foot sensor
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	other = contact->GetFixtureA();
	if ((int)fixtureUserData == 3 && !other->IsSensor())
		numFootContacts++;

	if (contact->GetFixtureA() != nullptr && (int)contact->GetFixtureA()->GetUserData() != 3)
	{
		
		b2Fixture* a = contact->GetFixtureA();
		b2Fixture* b = contact->GetFixtureB();
		b2Body* aBody = a->GetBody();
		b2Body* bBody = b->GetBody();

		// CHECK COLLISION

		
		if (aBody != nullptr && aBody->GetType() != b2_staticBody && bBody != nullptr && bBody->GetType() != b2_staticBody)
		{
			Player* player = (Player*)aBody->GetUserData();
			Enemy* enemy = (Enemy*)bBody->GetUserData();
			void* jee = a->GetUserData();
			void* daa = b->GetUserData();

			if ((int)jee == 4 && (int)daa == 5)
			{
				player->handleContact(player, enemy);
			}
			else if ((int)daa == 4 && (int)jee == 5)
			{
				player->handleContact(player, enemy);
			}
			
		}
	}
}

void MyContactListener::EndContact(b2Contact* contact)
{
	//check if fixture A was the foot sensor
	void* fixtureUserData = contact->GetFixtureA()->GetUserData();
	b2Fixture* other = contact->GetFixtureB();
	if ((int)fixtureUserData == 3 && !other->IsSensor())
		numFootContacts--;
	//check if fixture B was the foot sensor
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	other = contact->GetFixtureA();
	if ((int)fixtureUserData == 3 && !other->IsSensor())
		numFootContacts--;
}


