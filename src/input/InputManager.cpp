
/*
	InACTually
	> interactive theater for actual acts
	> this file is part of the "InACTually Engine", a MediaServer for driving all technology

	Copyright (c) 2021–2025 Lars Engeln, Fabian Töpfer
	Copyright (c) 2025 InACTually Community
	Licensed under the MIT License.
	See LICENSE file in the project root for full license information.

	This file is created and substantially modified: 2021

	contributors:
	Lars Engeln - mail@lars-engeln.de
*/

#include "input\InputManager.hpp"

#include "input\TouchInput.hpp"
#include "input\MouseInput.hpp"
#include "input\KeyInput.hpp"

#include <cinder/Timer.h>


using namespace act;
using namespace input;

InputManager::InputManager(ia::InteractionMgrRef iaMgr)
	: interactionMgr(iaMgr)
{

	mouseInput		= std::make_shared<MouseInput>();
	keyInput		= std::make_shared<KeyInput>();
	touchInput		= std::make_shared<TouchInput>(); 

	((MouseListener*)	mouseInput.get())->addListener(this);
	((KeyListener*)		keyInput.get())->addListener(this);
	((TouchListener*)	touchInput.get())->addListener(this);

	//updateThread = std::thread(&InputManager::update, this);

}


InputManager::~InputManager()
{
	doUpdates = false;
	//updateThread.join();
}

MouseRawListener*	InputManager::getMouseRawListener() { return (MouseRawListener*)	mouseInput.get(); }
KeyRawListener*		InputManager::getKeyRawListener()	{ return (KeyRawListener*)		keyInput.get(); }
TouchRawListener*	InputManager::getTouchRawListener() { return (TouchRawListener*)	touchInput.get(); }

void InputManager::update()
{
	mouseInput->update();
	keyInput->update();
	touchInput->update();

	return;

	/*
	ci::Timer timer;
	timer.start();
	double time = timer.getSeconds();
	int count = 0;
	stringstream strstr;

	double updateTick = 1000.0 / getFrameRate();
	double updateTime = timer.getSeconds();

	while (doUpdates) {
		mouseInput->update();
		keyInput->update();
		touchInput->update();
		
		count++;
		if (timer.getSeconds() - time >= 1.0) {
			ups = count;
			//feedback->setUpS(ups); 
			count = 0;
			time = timer.getSeconds();
		}	
		if (timer.getSeconds() - updateTime < updateTick) {
			std::chrono::milliseconds timespan((int)(updateTick - timer.getSeconds() + updateTime)); // or whatever
			std::this_thread::sleep_for(timespan);
		}
		updateTime = timer.getSeconds();
	}
	*/
}

void InputManager::drawDebug()
{
	mouseInput->drawDebug();
	keyInput->drawDebug();
	touchInput->drawDebug();
}

void act::input::InputManager::onKeyDown(ci::app::KeyEvent event)			{ interactionMgr->keyRawDown(event); }
void act::input::InputManager::onKeyUp(ci::app::KeyEvent event)				{ interactionMgr->keyRawUp(event); }
void act::input::InputManager::onMouseUp(ci::app::MouseEvent event)			{ interactionMgr->mouseRawUp(event); }
void act::input::InputManager::onMouseDown(ci::app::MouseEvent event)		{ interactionMgr->mouseRawDown(event); }
void act::input::InputManager::onMouseWheel(ci::app::MouseEvent event)		{ interactionMgr->mouseRawWheel(event); }
void act::input::InputManager::onMouseMove(ci::app::MouseEvent event)		{ interactionMgr->mouseRawMove(event); }
void act::input::InputManager::onMouseDrag(ci::app::MouseEvent event)		{ interactionMgr->mouseRawDrag(event); }
void act::input::InputManager::onTouchesBegin(ci::app::TouchEvent event)	{ interactionMgr->touchesRawBegan(event); }
void act::input::InputManager::onTouchesMove(ci::app::TouchEvent event)		{ interactionMgr->touchesRawMoved(event); }
void act::input::InputManager::onTouchesEnd(ci::app::TouchEvent event)		{ interactionMgr->touchesRawEnded(event); }
