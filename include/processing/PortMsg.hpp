
/*
	InACTually
	> interactive theater for actual acts
	> this file is part of the "InACTually Engine", a MediaServer for driving all technology

	Copyright (c) 2021–2025 Lars Engeln, Fabian Töpfer
	Copyright (c) 2025 InACTually Community
	Licensed under the MIT License.
	See LICENSE file in the project root for full license information.

	This file is created and substantially modified: 2023

	contributors:
	Lars Engeln - mail@lars-engeln.de
*/


#pragma once

#include "IDBase.hpp"
#include "UniqueIDBase.hpp"

#include "PortType.hpp"

#include <functional>


#include "camera/CameraRoomNode.hpp"

 
namespace act {
	namespace proc {

		
		using ImageInputPort		= InputPort<cv::UMat, room::CameraRoomNodeRef>;
		using ImageInputPortRef		= std::shared_ptr<ImageInputPort>;
		using ImageOutputPort		= OutputPort<cv::UMat, room::CameraRoomNodeRef>;
		using ImageOutputPortRef	= std::shared_ptr<ImageOutputPort>;

	}
}