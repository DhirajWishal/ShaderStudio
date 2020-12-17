-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Shader Studio project description ----------

project "ShaderStudio"
	kind "ConsoleApp"
	cppdialect "C++17"
	language "C++"
	staticruntime "On"
	systemversion "latest"

	targetdir "$(SolutionDir)Builds/Binaries/$(Configuration)-$(Platform)/$(ProjectName)"
	objdir "$(SolutionDir)Builds/Intermediate/$(Configuration)-$(Platform)/$(ProjectName)"

	files {
		"**.txt",
		"**.cpp",
		"**.h",
		"**.lua"
	}

	includedirs {
		"$(SolutionDir)Source",
	}

	links {
		"Graphics",
	}
