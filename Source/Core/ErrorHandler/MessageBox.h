// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/DataTypes.h"

namespace MessageBox
{
	/**
	 * Issue an info message box.
	 * 
	 * @param pMessage: The message to be displayed.
	 */
	void IssueInfo(const wchar* pMessage);

	/**
	 * Issue an warning message box.
	 *
	 * @param pMessage: The message to be displayed.
	 */
	void IssueWarn(const wchar* pMessage);

	/**
	 * Issue an error message box.
	 *
	 * @param pMessage: The message to be displayed.
	 */
	void IssueError(const wchar* pMessage);
}