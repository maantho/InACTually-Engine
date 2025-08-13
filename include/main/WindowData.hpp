
/*
	InACTually
	> interactive theater for actual acts
	> this file is part of the "InACTually Engine", a MediaServer for driving all technology

	Copyright (c) 2025 InACTually Community
	Licensed under the MIT License.
	See LICENSE file in the project root for full license information.

	This file is created and substantially modified: 2025

	contributors:
	Lars Engeln - mail@lars-engeln.de
*/

#pragma once

#include "cinder/app/App.h"
#include "cinder/app/Window.h"
#include "cinder/Json.h"

#include "Design.hpp"
#include "UniqueIDBase.hpp"

#include <algorithm>
#include <functional>


using namespace ci; 
using namespace ci::app;

namespace act {
	
	class WindowData : public UniqueIDBase {
	public:
		
		WindowData() {};
		~WindowData() {};

		static app::WindowRef createWindow(std::string title,
			const ci::ivec2& size = ci::ivec2(800, 600),
			bool fullScreen = false,
			bool borderless = false) {

			app::WindowRef window = ci::app::App::get()->createWindow(Window::Format().size(size));
			
			window->setTitle(title);
			window->setUserData(new WindowData());

			window->setFullScreen(fullScreen);
			window->setBorderless(borderless);

			return window;
		};

		virtual void cleanup() {
		};

		virtual void update() {};

		virtual void draw() {
			gl::clear(util::Design::backgroundColor());
			gl::color(Color::white());
		};

		virtual void fileDrop(FileDropEvent event) {};
		virtual void resize() const {};
		

	private:
		
	};

}