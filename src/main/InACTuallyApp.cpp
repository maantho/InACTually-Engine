
/*
	InACTually
	2021

	participants:
	Lars Engeln - mail@lars-engeln.de

	interactive theater for actual acts
*/

#include "cinder/app/App.h"
#include "main/InACTually.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace act;

class InACTuallyApp : public App {
  public:
	  void setup() override;
	  void applyWidget();

	  void cleanup() override;
	  void update() override;
	  void draw() override;

	  shared_ptr<input::MouseRawListener> mouseRawListener = make_shared<input::MouseRawListener>();
	  virtual void	mouseUp(MouseEvent event)		override { mouseRawListener->mouseRawUp(event); }
	  virtual void	mouseDown(MouseEvent event)		override { mouseRawListener->mouseRawDown(event); }
	  virtual void	mouseWheel(MouseEvent event)	override { mouseRawListener->mouseRawWheel(event); }
	  virtual void	mouseMove(MouseEvent event)		override { mouseRawListener->mouseRawMove(event); }
	  virtual void	mouseDrag(MouseEvent event)		override { mouseRawListener->mouseRawDrag(event); }

	  shared_ptr<input::TouchRawListener> touchRawListener = make_shared<input::TouchRawListener>();
	  void			touchesBegan(TouchEvent event)	override { touchRawListener->touchesRawBegan(event); }
	  void			touchesMoved(TouchEvent event)	override { touchRawListener->touchesRawMoved(event); }
	  void			touchesEnded(TouchEvent event)	override { touchRawListener->touchesRawEnded(event); }

	  shared_ptr<input::KeyRawListener> keyRawListener = make_shared<input::KeyRawListener>();
	  virtual void	keyDown(KeyEvent event)			override { keyRawListener->keyRawDown(event); }
	  virtual void	keyUp(KeyEvent event)			override { keyRawListener->keyRawUp(event); }

	  void fileDrop(FileDropEvent event) override;
	  void resize() override;

	  shared_ptr<InACTuallyApp> inACTually;
};


#define RegisterModule(module) VisualAudioDesign::useModule(std::make_shared<module>());
#define RegisterVisual(visual) VisualAudioDesign::useVisual(std::make_shared<visual>());
#define RegisterWidget(widget) VisualAudioDesign::useWidget(std::make_shared<widget>());
#define RegisterEffect(effect) VisualAudioDesign::useEffect(std::make_shared<effect>());

#include "widget/WidgetRegistration.hpp"

#include "modules\TransformModule.hpp"
#include "modules\draw\DrawModule.hpp"
#include "modules\morph\MorphModule.hpp"
#include "modules\ManipulationExamplesModule.hpp"
#include "modules\LoadModule.hpp"
#include "modules\SaveModule.hpp"
#include "modules\StampModule.hpp"
#include "modules\FXModule.hpp"

void InACTuallyApp::setup()
{
	server = net::OSCServer::create(2999);
}   

void InACTuallyApp::draw()
{
	gl::clear();

}

CINDER_APP( InACTuallyApp, RendererGl )
