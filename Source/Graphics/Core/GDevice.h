// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GWindow.h"
#include "GRenderTarget.h"

namespace Graphics
{
	class GDevice {
	public:
		GDevice() {}
		virtual ~GDevice() {}

		virtual void CreateWindow(UI32 width, UI32 height, const char* pTitle) {}
		virtual void DestroyWindow() {}

		virtual void Initialize(bool enableValidation) {}
		virtual void Terminate() {}

		virtual void BeginDraw() {}
		virtual void Update() {}
		virtual void EndDraw() {}

	public:
		virtual GRenderTarget* CreateRenderTarget(RenderTargetType type, UI32 width, UI32 height, float xOffset, float yOffset) { return nullptr; }
		virtual void DestroyRenderTarget(GRenderTarget* pRenderTarget) {}

	public:
		Inputs::InputCenter* GetInputCenter() const { return pWindow->GetInputCenter(); }

	protected:
		GWindow* pWindow = nullptr;
	};
}