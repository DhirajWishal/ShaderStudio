// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/GDevice.h"

namespace Graphics
{
	/**
	 * Graphics API enum.
	 */
	enum class GraphcisAPI : UI8 {
		VULKAN,
	};

	class GraphcisEngine {
	public:
		GraphcisEngine() {}
		~GraphcisEngine() {}

		void Initialize(GraphcisAPI gAPI = GraphcisAPI::VULKAN);
		void Update();
		void Terminate();

		Inputs::InputCenter* GetInputCenter() const { return pDevice->GetInputCenter(); }

	private:
		constexpr GDevice* GetDevice() const noexcept { return pDevice; }

	private:
		void CreateRenderTarget();
		void DestroyRenderTarget();

	private:
		GDevice* pDevice = nullptr;
		GRenderTarget* pRenderTarget = nullptr;
		WindowExtent mDefaultExtent = WindowExtent(1280, 720);

		GraphcisAPI mAPI = GraphcisAPI::VULKAN;
	};
}