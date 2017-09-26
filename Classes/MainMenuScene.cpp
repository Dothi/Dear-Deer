#include "MainMenuScene.h"
#include "GameScene.h"
#include "Definitions.h"

USING_NS_CC;

Scene* MainMenuScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = MainMenuScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool MainMenuScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto backgroundSprite = Sprite::create("Autumn.png");
	backgroundSprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

	this->addChild(backgroundSprite);

	auto titleSprite = Sprite::create("title.png");
	titleSprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height + origin.y - titleSprite->getContentSize().height));

	this->addChild(titleSprite);

	auto playButton = MenuItemImage::create("UI/play_button.png", "UI/play_button_pressed.png", CC_CALLBACK_1(MainMenuScene::GoToGameScene, this));
	
	playButton->setPosition(Point(visibleSize.width / 2 + origin.x, (visibleSize.height / 2 + origin.y) - 110));

	auto menu = Menu::create(playButton, NULL);
	menu->setPosition(Point::ZERO);

	this->addChild(menu);
	

	return true;
}

void MainMenuScene::GoToGameScene(cocos2d::Ref *sender)
{
	auto scene = GameScene::createScene();

	Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}
