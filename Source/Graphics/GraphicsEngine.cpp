// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsEngine.h"
#include "Core/ErrorHandler/Logger.h"

#include "Backend/Vulkan/VulkanDevice.h"

namespace Graphics
{
	void GraphcisEngine::Initialize(GraphcisAPI gAPI)
	{
		switch (gAPI)
		{
		case Graphics::GraphcisAPI::VULKAN:
			pDevice = new VulkanBackend::VulkanDevice();
			break;
		default:
			Logger::LogError(TEXT("Invalid or undefined Graphics API type!"));
			break;
		}

		GetDevice()->CreateWindow(mDefaultExtent.mWidth, mDefaultExtent.mHeight, "Shader Studio v1.0");

#if SS_DEBUG
		GetDevice()->Initialize(true);

#else
		GetDevice()->Initialize(false);

#endif	// SS_DEBUG

		CreateRenderTarget();
	}

	void GraphcisEngine::Update()
	{
		GetDevice()->BeginDraw();
		GetDevice()->Update();
		GetDevice()->EndDraw();
	}

	void GraphcisEngine::Terminate()
	{
		DestroyRenderTarget();
		GetDevice()->Terminate();
		delete GetDevice();
	}
	
	void GraphcisEngine::CreateRenderTarget()
	{
		pRenderTarget = GetDevice()->CreateRenderTarget(RenderTargetType::SCREEN_BOUND_3D, mDefaultExtent.mWidth, mDefaultExtent.mHeight, 0.0f, 0.0f);
	}
	
	void GraphcisEngine::DestroyRenderTarget()
	{
		GetDevice()->DestroyRenderTarget(pRenderTarget);
	}
}
