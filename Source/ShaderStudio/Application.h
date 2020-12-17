// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Graphics/GraphicsEngine.h"

/**
 * Main application object.
 */
class Application {
public:
	Application();
	~Application();

private:
	void Initialize();
	void Execute();
	void Terminate();

private:
	Graphics::GraphcisEngine mGraphicsEngine = {};
	Inputs::InputCenter* pInputCenter = nullptr;
};