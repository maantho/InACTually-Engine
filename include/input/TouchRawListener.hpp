/*
	InACTually
	2021

	contributor:
	Lars Engeln - mail@lars-engeln.de

	interactive theater for actual acts
*/

#pragma once

#include "ListenerBase.hpp"
#pragma warning( push )
#pragma warning( disable : 4081)
namespace act {
	namespace input {
		class TouchRawListener : public ListenerBase<TouchRawListener>
		{
		public:
			eventCall(touchesRawBegan, TouchEvent)
			eventCall(touchesRawMoved, TouchEvent)
			eventCall(touchesRawEnded, TouchEvent)
		};

		class TouchListener : public ListenerBase<TouchListener>
		{
		public:
			eventCall(onTouchesBegin, TouchEvent)
			eventCall(onTouchesMove, TouchEvent)
			eventCall(onTouchesEnd, TouchEvent)
		};
	}
}
#pragma warning( pop )