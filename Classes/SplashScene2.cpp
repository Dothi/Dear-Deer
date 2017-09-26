#include "SplashScene2.h"


USING_NS_CC;

Scene* SplashScene2::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = SplashScene2::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool SplashScene2::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	/*CCLabelTTF* ttf = CCLabelTTF::create("SPLASH SCREEN", "Helvetica", 18, CCSizeMake(245, 32), kCCTextAlignmentCenter);
	ttf->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));*/

	this->scheduleOnce(schedule_selector(SplashScene2::GoToMainMenuScene), DISPLAY_TIME_SPLASH_SCENE);

	auto backgroundSprite = Sprite::create("UI/Splash_Screen.png");
	
	backgroundSprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

	this->addChild(backgroundSprite);
	//this->addChild(ttf);
    
    return true;
}

void SplashScene2::GoToMainMenuScene(float dt)
{
	auto scene = GameScene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}


