#include "B2DebugDrawLayer.h"

using namespace cocos2d;

B2DebugDrawLayer* B2DebugDrawLayer::create(b2World* pB2World, float pPtmRatio)
{
	B2DebugDrawLayer *pRet = new B2DebugDrawLayer(pB2World, pPtmRatio);
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

B2DebugDrawLayer::B2DebugDrawLayer(b2World* pB2World, float pPtmRatio)
	: mB2World(pB2World), mPtmRatio(pPtmRatio)
{
}

bool B2DebugDrawLayer::init()
{
	//////////////////////////////
	// 1. super init first

	if (!CCLayer::init())
	{
		return false;
	}

	CCLOG("INITIALIZING B2DebugDrawLayer");

	mB2DebugDraw = new GLESDebugDraw(mPtmRatio);
	mB2World->SetDebugDraw(mB2DebugDraw);

	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	flags += b2Draw::e_jointBit;
	flags += b2Draw::e_aabbBit;
	flags += b2Draw::e_pairBit;
	flags += b2Draw::e_centerOfMassBit;
	mB2DebugDraw->SetFlags(flags);

	return true;
}



void B2DebugDrawLayer::draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	//
	// IMPORTANT:
	// This is only for debug purposes
	// It is recommend to disable it
	//

	Layer::draw(renderer, transform, flags);

	ccGLEnableVertexAttribs(kCCVertexAttribFlag_Position);

	kmGLPushMatrix();

	mB2World->DrawDebugData();

	kmGLPopMatrix();

	/*
	GL::enableVertexAttribs( cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION );
	Director* director = Director::getInstance();
	CCASSERT(nullptr != director, "Director is null when seting matrix stack");
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	_modelViewMV = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	_customCommand.init(_globalZOrder);
	_customCommand.func = CC_CALLBACK_0(B2DebugDrawLayer::onDraw, this);
	renderer->addCommand(&_customCommand);
	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	*/

}


void B2DebugDrawLayer::onDraw()
{
	CCLOG("ONDRAW");
	Director* director = Director::getInstance();
	CCASSERT(nullptr != director, "Director is null when seting matrix stack");

	Mat4 oldMV;
	oldMV = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewMV);
	mB2World->DrawDebugData();
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, oldMV);
}