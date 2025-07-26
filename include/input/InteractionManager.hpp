
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

#pragma once

#include "cinder/app/Event.h"
#include "input/InputListeners.hpp"
#include "input/MouseRawListener.hpp"
#include "input/KeyRawListener.hpp"
#include "input/TouchRawListener.hpp"

//#include "canvas\canvas.hpp"

namespace act {
	namespace gui {
		class GuiManager;
	}
	namespace wdgt {
		class WidgetManager;
	}
	/**
	* @brief interaction related things, like the interpretation from raw-input (InputManager) to actions are performed here (InputManager) i.e. SelectionHelper
	*
	*/
	namespace ia {

		/**
		 * @brief The InteractionManager receives any input from the observers as a cinder event. 
		 * This 'raw' input is preprocessed by converting the screenspace to worldspace and 
		 * passing it on to the listeners when interacting on a widget. If not interacting on a widget, 
		 * zoom and pan are handled.
		 * 
		 */
		class InteractionManager : public input::InputListeners, public input::MouseRawListener, public input::KeyRawListener, public input::TouchRawListener, public input::MouseListener, public input::KeyListener, public input::TouchListener
		{
		public:
			InteractionManager(std::shared_ptr<act::gui::GuiManager> guiMgr, std::shared_ptr<act::wdgt::WidgetManager> wdgtMgr);
			~InteractionManager();

			//void setCanvas(act::canvas::CanvasRef canvas);

			void	pan(vec2 position);
			void	zoom(float factor);
			void	zoomAt(vec2 at, float factor);

			vec2	getPan() { return panning; };
			float	getZoom() { return zooming; };

			virtual input::MouseListener*		getMouseListener()		override;
			virtual input::KeyListener*			getKeyListener()		override;
			virtual input::TouchListener*		getTouchListener()		override;

			void keyRawDown(ci::app::KeyEvent event)					override;
			void keyRawUp(ci::app::KeyEvent event)						override;
			void mouseRawUp(ci::app::MouseEvent event)					override;
			void mouseRawDown(ci::app::MouseEvent event)				override;
			void mouseRawWheel(ci::app::MouseEvent event)				override;
			void mouseRawMove(ci::app::MouseEvent event)				override;
			void mouseRawDrag(ci::app::MouseEvent event)				override;
			void touchesRawBegan(ci::app::TouchEvent event)				override;
			void touchesRawMoved(ci::app::TouchEvent event)				override;
			void touchesRawEnded(ci::app::TouchEvent event)				override;

			vec2 toObjectSpace(vec2 point);
			vec2 toScreenSpace(vec2 point);

		private:
			//act::canvas::CanvasRef canvas;
			std::shared_ptr<act::gui::GuiManager> guiMgr;
			std::shared_ptr<act::wdgt::WidgetManager> wdgtMgr;

			vec2	panning;
			float	zooming;

			bool	isPanning;
			ivec2	dragStartPosition;
			ivec2	dragNewPosition;
			ivec2	dragPreviousPosition;
			bool	isInteracting;
			ivec2	startInteractionPosition;
			ivec2	lastInteractionPosition;
		};
		typedef	std::shared_ptr<InteractionManager> InteractionMgrRef;
	};
};