

#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/gl/Texture.h"

#include "cinder/audio/audio.h"

#include "interaction/InteractionManager.hpp"
#include "gui\GuiManager.hpp"
#include "widget/WidgetManager.hpp"



using namespace ci; 
using namespace ci::app;
using namespace std;

using namespace vad;
using namespace ia;

InteractionManager::InteractionManager(std::shared_ptr<vad::gui::GuiManager> guiMgr, std::shared_ptr<vad::wdgt::WidgetManager> wdgtMgr)
	: guiMgr(guiMgr), wdgtMgr(wdgtMgr)
{
	panning = vec2(1000, 700);
	zooming = 0.53f;

	isPanning = false;
	dragNewPosition = ivec2(0);
	dragPreviousPosition = ivec2(0);

	isInteracting = false;
	startInteractionPosition = ivec2(0);
}

InteractionManager::~InteractionManager()
{
}

void vad::ia::InteractionManager::setCanvas(vad::canvas::SpectralCanvasRef spectralCanvas)
{
	this->spectralCanvas = spectralCanvas;
}

void vad::ia::InteractionManager::pan(vec2 position)
{
	panning += position / zooming;
}

void vad::ia::InteractionManager::zoom(float factor)
{
	zooming *= factor;
}

void vad::ia::InteractionManager::zoomAt(vec2 at, float factor)
{
	pan(-at);
	zoom(factor);
	pan(at);
}

input::MouseListener*		InteractionManager::getMouseListener()		{ return (MouseListener*)		this; }
input::KeyListener*			InteractionManager::getKeyListener()		{ return (KeyListener*)			this; }
input::TouchListener*		InteractionManager::getTouchListener()		{ return (TouchListener*)		this; }

void vad::ia::InteractionManager::keyRawDown(KeyEvent event)
{
	if (event.getCode() == KeyEvent::KEY_SPACE) {
		spectralCanvas->togglePlay();
	}
	else if (event.getCode() == KeyEvent::KEY_DELETE) {
		if(!spectralCanvas->removeNode(spectralCanvas->getNodeAt(lastInteractionPosition)))
			onKeyDown(event);
	}
	else {
		onKeyDown(event);
	}
}

void vad::ia::InteractionManager::keyRawUp(KeyEvent event)
{
	onKeyUp(event);
}


void vad::ia::InteractionManager::mouseRawMove(MouseEvent event)
{
	if (guiMgr->hit(event.getPos())) {
		guiMgr->onHover(event.getPos());
	}
	else {
		vec2 position = toObjectSpace(event.getPos());
		lastInteractionPosition = position;
		event.setPos(position);
		onMouseMove(event);

		wdgtMgr->onMouseMove(position);
	}
}

void vad::ia::InteractionManager::mouseRawDown(MouseEvent event)
{
	//###REFACTOR### into GUIMgr
	if (guiMgr->hit(event.getPos())) {
		guiMgr->onPress(event.getPos());
	}
	else {
		vec2 position = toObjectSpace(event.getPos());

		if (wdgtMgr->hit(position)) {
			isInteracting = true;
			event.setPos(position);
			startInteractionPosition = position;

			onMouseDown(event);
		}
		else {
			dragPreviousPosition = event.getPos();
			isPanning = true;
		}

		/*
		if (position.y < 0 && position.y > -250) {
			spectralCanvas->setPlayPosition(position.x / (float)spectralCanvas->getSize().x);
		}
		else if (position.x > 0 && position.y > 0 && position.x < spectralCanvas->getSize().x && position.y < spectralCanvas->getSize().y) {

			isInteracting = true;
			event.setPos(position);
			startInteractionPosition = position;

			onMouseDown(event);
		}
		else {
			dragPreviousPosition = event.getPos();
			isPanning = true;
		}*/
	}
}

void vad::ia::InteractionManager::mouseRawDrag(MouseEvent event)
{
	if (isPanning) {
		pan(event.getPos() - dragPreviousPosition);
		dragPreviousPosition = event.getPos();
	}
	else {
		//###REFACTOR### into GUIMgr
		if (guiMgr->hit(event.getPos())) {
			guiMgr->onPress(event.getPos());
		}
		else {
			vec2 position = toObjectSpace(event.getPos());
			lastInteractionPosition = position;

			event.setPos(position);

			onMouseDrag(event);
			/* if (position.y < 0 && position.y > -250) {
				spectralCanvas->setPlayPosition(position.x / (float)spectralCanvas->getSize().x);
			}
			else if (position.x > 0 && position.y > 0 && position.x < spectralCanvas->getSize().x && position.y < spectralCanvas->getSize().y) {
				onMouseDrag(event);
			}*/
		}

	}
}

void vad::ia::InteractionManager::mouseRawUp(MouseEvent event)
{
	isPanning = false;
	vec2 position = toObjectSpace(event.getPos());

	if (guiMgr->hit(event.getPos())) {
		guiMgr->onUp(event.getPos());
	}
	else {
		/*if (position.x > 0 && position.y > 0 && position.x < spectralCanvas->getSize().x && position.y < spectralCanvas->getSize().y) {
			event.setPos(position);
			onMouseUp(event);
		}
		else*/ if (isInteracting) {
			isInteracting = false;

			position = lastInteractionPosition;

			event.setPos(position);
			onMouseUp(event);
		}
	}
}

void vad::ia::InteractionManager::mouseRawWheel(MouseEvent event)
{
	ivec2 wSize = event.getWindow()->getSize();
	

	float scale = 1.05f;

	if (event.getWheelIncrement() < 0 && zooming > 0.2f) {
		zoomAt(event.getPos(), 1.0f / scale);
	}
	else if (event.getWheelIncrement() > 0 && zooming < 2.f) {
		zoomAt(event.getPos(), scale);
	}

	onMouseWheel(event);
}


void vad::ia::InteractionManager::touchesRawBegan(ci::app::TouchEvent event)
{
	vec2 position = toObjectSpace(event.getTouches()[0].getPos());

	//###REFACTOR### into GUIMgr

	/*if (position.y < 0 && position.y > -250) {
		spectralCanvas->setPlayPosition(position.x / (float)spectralCanvas->getSize().x);
	}
	else if (position.x > 0 && position.y > 0 && position.x < spectralCanvas->getSize().x && position.y < spectralCanvas->getSize().y) {
*/	
	if (!isPanning && wdgtMgr->hit(position)) {
		isInteracting = true;
		for (auto&& touch : event.getTouches()) {
			touch.setPos(toObjectSpace(touch.getPos()));
		}
		startInteractionPosition = position;

		onTouchesBegin(event);
	}
	else {
		if (event.getTouches().size() == 1) {
			dragPreviousPosition = event.getTouches()[0].getPos();
		}
		else if (event.getTouches().size() == 2) {
			vec2 pinchPos = (event.getTouches()[0].getPos() + event.getTouches()[1].getPos()) * 0.5f;
			dragPreviousPosition = pinchPos;
		}
		isPanning = true;
	}

}

void vad::ia::InteractionManager::touchesRawMoved(ci::app::TouchEvent event)
{
	if (isPanning) {
		if (event.getTouches().size() == 1) {
			pan(event.getTouches()[0].getPos() - vec2(dragPreviousPosition));
			dragPreviousPosition = event.getTouches()[0].getPos();
		}
		else if (event.getTouches().size() == 2) {
			vec2 pinchPos = (event.getTouches()[0].getPos() + event.getTouches()[1].getPos()) * 0.5f;
			pan(pinchPos - vec2(dragPreviousPosition));
			dragPreviousPosition = pinchPos;
		}
	}
	else {
		vec2 position = toObjectSpace(event.getTouches()[0].getPos());
		for (auto&& touch : event.getTouches()) {
			touch.setPos(toObjectSpace(touch.getPos()));
		}

		//###REFACTOR### into GUIMgr
		/*if (position.y < 0 && position.y > -250) {
			spectralCanvas->setPlayPosition(position.x / (float)spectralCanvas->getSize().x);
		}
		else if (position.x > 0 && position.y > 0 && position.x < spectralCanvas->getSize().x && position.y < spectralCanvas->getSize().y) {
		*/
		onTouchesMove(event);
		//}
	}

}

void vad::ia::InteractionManager::touchesRawEnded(ci::app::TouchEvent event)
{
	for (auto&& touch : event.getTouches()) {
		touch.setPos(toObjectSpace(touch.getPos()));
	}

	onTouchesEnd(event);
}

vec2 vad::ia::InteractionManager::toObjectSpace(vec2 point)
{
	return point * (1.0f / zooming) - panning;
}

vec2 vad::ia::InteractionManager::toScreenSpace(vec2 point)
{
	return (point - panning) * (1.0f / zooming);
}