
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

#include "cinder/app/App.h"
#include "CinderOpenCV.h" //"main/InACTually.hpp"

#ifndef _BASE64_H_
#define _BASE64_H_

#include <vector>
#include <string>
typedef unsigned char BYTE;

std::string base64_encode(BYTE const* buf, unsigned int bufLen);
std::vector<BYTE> base64_decode(std::string const&);

std::string  surface8uToBase64(ci::Surface8u imgSurface8u, cv::String ext);
std::string  matToBase64(cv::Mat imgMat, cv::String ext, int quality = 70, bool scale = false, int newWidth = 1280);
#endif