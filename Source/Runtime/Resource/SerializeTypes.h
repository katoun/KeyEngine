/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

namespace resource
{
	namespace serialize
	{
		enum class Types
		{
			VOID,
			BOOLEAN,
			INTEGER,
			UNSIGNED_INTEGER,
			REAL,
			STRING,
			OBJECT,
			ADDRESS,
			COUNT
		};

		enum class TypeFlags
		{
			NORMAL,
			POLYMORFHIC
		};

		enum class TypeMode
		{
			VALUE,
			REFERENCE
		};
	}
}
