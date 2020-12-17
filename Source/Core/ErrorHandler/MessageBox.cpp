// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "MessageBox.h"

#include <Windows.h>

#ifdef MessageBox
#undef MessageBox

#endif

namespace MessageBox
{
	void IssueInfo(const wchar* pMessage)
	{
		UI32 flags = MB_ICONINFORMATION | MB_OK;
		MessageBoxW(NULL, pMessage, TEXT("Information."), flags);
	}

	void IssueWarn(const wchar* pMessage)
	{
		UI32 flags = MB_ICONWARNING | MB_OK;
		MessageBoxW(NULL, pMessage, TEXT("Warning!"), flags);
	}

	void IssueError(const wchar* pMessage)
	{
		UI32 flags = MB_ICONERROR | MB_OK;
		MessageBoxW(NULL, pMessage, TEXT("Error!"), flags);
	}
}
