// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/DataTypes.h"
#include "Inputs/InputCenter.h"

namespace Graphics
{
	struct WindowExtent {
		WindowExtent() = default;
		WindowExtent(UI32 width, UI32 height) : mWidth(width), mHeight(height) {}

		UI32 mWidth = 0, mHeight = 0;
	};

	class GWindow {
	public:
		GWindow() {}
		virtual ~GWindow() {}

		virtual void Initialize(UI32 width, UI32 height, const char* pTitle) {}
		virtual void Terminate() {}

		virtual void PollInputs() {}

		Inputs::InputCenter* GetInputCenter() const { return const_cast<Inputs::InputCenter*>(&mInputCenter); }
		WindowExtent GetExtent() const { return mExtent; }

	protected:
		Inputs::InputCenter mInputCenter = {};

		WindowExtent mExtent = {};
	};
}