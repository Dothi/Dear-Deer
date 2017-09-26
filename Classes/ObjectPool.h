#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

#include "cocos2d.h"

using namespace cocos2d;

class ObjectPool
{
public:
	ObjectPool(int size, CCSprite* sprite, Layer* gameScene);
	virtual ~ObjectPool();
	
private:
	 
	 int size;
	 int poolIndex;
	 CCSprite* sprite;
	 Layer* gameScene;
	 CCArray* pool;
};

#endif