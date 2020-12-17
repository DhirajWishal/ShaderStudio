// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/ErrorHandler/Logger.h"

/**
 * Vulkan Assert macro.
 * This checks if the return of an expression is equal to VK_SUCCESS.
 *
 * @param exp: The expression.
 * @param msg: The message to be logged if failed.
 */
#ifdef SS_DEBUG
#define VK_ASSERT(exp, msg)			if (exp != VK_SUCCESS)	::Logger::LogError(TEXT(msg)), __debugbreak()  

#else
#define VK_ASSERT(exp, msg)			exp

#endif // SS_DEBUG


 /**
  * Get VkBool32 macro.
  * This converts bool values to VkBool32 values (VK_TRUE and VK_FALSE).
  *
  * @param condition: The boolean condition.
  */
#define GET_VK_BOOL(condition)		condition ? VK_TRUE : VK_FALSE
