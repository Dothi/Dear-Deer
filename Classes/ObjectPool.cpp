#include "ObjectPool.h"

ObjectPool::ObjectPool(int _size, CCSprite* _sprite, Layer* _gameScene)
{
	Layer* gameScene = _gameScene;
	size = _size;
	sprite = _sprite;
	pool = CCArray::createWithCapacity(size);

	pool->retain();
	poolIndex = 0;

	for (int i = 0; i < size; i++)
	{

		sprite->setScaleX(-1);
		sprite->setVisible(false);
		gameScene->addChild(sprite, 0);
		pool->addObject(sprite);
	}
}
ObjectPool::~ObjectPool()
{
	pool->release();
	CC_SAFE_RELEASE(gameScene);
	CC_SAFE_RELEASE(pool);
}
