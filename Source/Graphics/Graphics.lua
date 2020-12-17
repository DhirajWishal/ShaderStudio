-- Copyright 2020 Dhiraj Wishal
-- SPDX-License-Identifier: Apache-2.0

---------- Graphics project description ----------

project "Graphics"
	kind "StaticLib"
	language "C++"
	systemversion "latest"
	cppdialect "C++17"
	staticruntime "On"

	targetdir "$(SolutionDir)Builds/Binaries/$(Configuration)-$(Platform)/$(ProjectName)"
	objdir "$(SolutionDir)Builds/Intermediate/$(Configuration)-$(Platform)/$(ProjectName)"

	files {
		"**.txt",
		"**.cpp",
		"**.h",
		"**.lua",
		"**.txt",
		"**.md",
	}

	includedirs {
		"$(SolutionDir)Source/",
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.GLFW}",
	}

	libdirs {
		"%{IncludeLib.Vulkan}",
		"%{IncludeLib.GLFW}",
	}

	links { 
		"Core",
		"Inputs",
		"vulkan-1",
		"glfw3dll",
	}