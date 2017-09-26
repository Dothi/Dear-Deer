#include "SplashScene.h"


USING_NS_CC;

Scene* SplashScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = SplashScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool SplashScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	/*CCLabelTTF* ttf = CCLabelTTF::create("SPLASH SCREEN", "Helvetica", 18, CCSizeMake(245, 32), kCCTextAlignmentCenter);
	ttf->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));*/

	this->scheduleOnce(schedule_selector(SplashScene::GoToMainMenuScene), DISPLAY_TIME_SPLASH_SCENE);

	auto backgroundSprite = Sprite::create("UI/KajakGamesSplashScreen.png");
	
	backgroundSprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

	this->addChild(backgroundSprite);
	//this->addChild(ttf);
    
    return true;
}

void SplashScene::GoToMainMenuScene(float dt)
{
	auto scene = SplashScene2::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}


