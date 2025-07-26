
/*
	InACTually
	> interactive theater for actual acts
	> this file is part of the "InACTually Engine", a MediaServer for driving all technology

	Copyright (c) 2021–2025 Lars Engeln, Fabian Töpfer
	Copyright (c) 2025 InACTually Community
	Licensed under the MIT License.
	See LICENSE file in the project root for full license information.

	This file is created and substantially modified: 2024

	contributors:
	Lars Engeln - mail@lars-engeln.de
*/

#include "Logger.hpp"
#include "cinder/app/App.h"
#include "cinder/Utilities.h"

bool act::util::Logger::m_isInitialized = false;

void act::util::Logger::initialize()
{
	if (m_isInitialized)
		return;

	m_isInitialized = true;

	ci::log::makeLogger<ci::log::LoggerSystem>()->setLevel(ci::log::LEVEL_WARNING);

	ci::log::makeLogger<ci::log::LoggerFileRotating>(ci::app::getAppPath() / "logs", "InActually_%Y-%m-%d.log", true, [](const std::filesystem::path& path) {
		ci::limitDirectoryFileCount(path.parent_path(), 10); // logs from the last 10 days
	});
}
