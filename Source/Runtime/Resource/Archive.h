/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>

namespace resource
{
	namespace serialize
	{
		class RUNTIME_API Archive
		{
		private:
			//The rtti virtual function only exists to enable an archive to be used in a polymorphic fashion, if necessary.
			virtual void rtti() {}
		};
	}
}
