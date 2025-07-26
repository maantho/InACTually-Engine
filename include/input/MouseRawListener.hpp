
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

#include "ListenerBase.hpp"

#pragma warning( push )
#pragma warning( disable : 4081)
namespace act {
	namespace input {
		class MouseRawListener : public ListenerBase<MouseRawListener>
		{
		public:
			eventCall(mouseRawMove, cinder::app::MouseEvent);
			eventCall(mouseRawDown, cinder::app::MouseEvent);
			eventCall(mouseRawDrag, cinder::app::MouseEvent);
			eventCall(mouseRawUp, cinder::app::MouseEvent);
			eventCall(mouseRawWheel, cinder::app::MouseEvent);
		};

		class MouseListener : public ListenerBase<MouseListener>
		{
		public:
			eventCall(onMouseMove, cinder::app::MouseEvent);
			eventCall(onMouseDown, cinder::app::MouseEvent);
			eventCall(onMouseDrag, cinder::app::MouseEvent);
			eventCall(onMouseUp, cinder::app::MouseEvent);
			eventCall(onMouseWheel, cinder::app::MouseEvent);
		};
	}
}
#pragma warning( pop )