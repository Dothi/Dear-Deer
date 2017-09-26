#ifndef SPLASH_SCENE2_H
#define SPLASH_SCENE2_H
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"
#include "Definitions.h"

class SplashScene2 : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    CREATE_FUNC(SplashScene2);

private:
	void GoToMainMenuScene(float dt);

};

#endif 
