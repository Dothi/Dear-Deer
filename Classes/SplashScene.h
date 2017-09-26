#ifndef SPLASH_SCENE_H
#define SPLASH_SCENE_H
#include "cocos2d.h"
#include "SplashScene2.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"
#include "Definitions.h"


class SplashScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    CREATE_FUNC(SplashScene);

private:
	void GoToMainMenuScene(float dt);

};

#endif 
