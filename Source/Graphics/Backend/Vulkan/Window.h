// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Graphics/Core/GWindow.h"
#include <GLFW/glfw3.h>

namespace Graphics
{
	namespace VulkanBackend
	{
		class VulkanWindow : public GWindow {
		public:
			VulkanWindow() {}
			~VulkanWindow() {}

			virtual void Initialize(UI32 width, UI32 height, const char* pTitle) override final;
			virtual void Terminate() override final;

			virtual void PollInputs() override final;

			GLFWwindow* GetWindowHandle() const { return pWindowHandle; }

		private:
			void SetupInputs();
			void SetupCallbacks();

		private:
			GLFWwindow* pWindowHandle = nullptr;
		};
	}
}