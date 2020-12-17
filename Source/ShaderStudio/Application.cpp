// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Application.h"
#include "Core/ErrorHandler/Logger.h"

Application::Application()
{
	Logger::LogInfo(TEXT("Welcome to the Shader Studio!"));

	Initialize();
	Execute();
}

Application::~Application()
{
	Terminate();
}

void Application::Initialize()
{
	// Initialize the graphcis engine.
	mGraphicsEngine.Initialize();
	pInputCenter = mGraphicsEngine.GetInputCenter();
}

void Application::Execute()
{
	while (pInputCenter->IsWindowOpen)
	{
		mGraphicsEngine.Update();

		if (pInputCenter->KeyA.IsPressed())
			Logger::LogInfo(TEXT("Key A"));
	}
}

void Application::Terminate()
{
}
