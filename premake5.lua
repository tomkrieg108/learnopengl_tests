workspace "gl_app"

	system "windows"
	architecture "x64"
	language "C++"
	cppdialect "C++17"
	platforms {"Win64"}

	configurations  
	{ 
		"Debug", 
		"Release" 
	}
	

vendor_dir = "C:/dev/vendor"
external_libs_dir = "C:/dev/external_libs"
output_dir = "%{cfg.platform}-%{cfg.buildcfg}"

project "gl_app"

	location "%{prj.name}"
	kind "ConsoleApp"
	targetdir ("bin/" .. output_dir)
	objdir ("bin-obj/" .. output_dir)
	pchheader "pch.h"
	pchsource "%{prj.location}/src/framework/pch.cpp"
	
	files 
	{ 
		"%{prj.location}/src/**.h", 
		"%{prj.location}/src/**.cpp", 

		 "%{vendor_dir}/glm/*.hpp",
		--vendor_dir .. "/glm/**.hpp",
		--vendor_dir .. "/imgui/**.cpp",
		--vendor_dir .. "/imgui/**.h",

		--Files are taken from c:/dev/vendor
        --"%{vendor_dir}/imgui/*.cpp",
	    --"%{vendor_dir}/imgui/*.h",
		--"%{vendor_dir}/imgui/backends/imgui_impl_glfw.cpp",
		--"%{vendor_dir}/imgui/backends/imgui_impl_opengl3.cpp",
		--"%{vendor_dir}/imgui/backends/imgui_impl_glfw.h",
		--"%{vendor_dir}/imgui/backends/imgui_impl_opengl3.h",

		--Files are taken from c:/dev/vendor
        "%{vendor_dir}/imgui_docking/*.cpp",
	    "%{vendor_dir}/imgui_docking/*.h",
		"%{vendor_dir}/imgui_docking/backends/imgui_impl_glfw.cpp",
		"%{vendor_dir}/imgui_docking/backends/imgui_impl_opengl3.cpp",
		"%{vendor_dir}/imgui_docking/backends/imgui_impl_glfw.h",
		"%{vendor_dir}/imgui_docking/backends/imgui_impl_opengl3.h",


		vendor_dir .. "/stb_image/stb_image.cpp",
		vendor_dir .. "/stb_image/stb_image.h",
		--vendor_dir .. "/spdlog/**.h"
	}

	removefiles
	{
		--vendor_dir .. "/imgui/main*.cpp",
		"%{prj.location}/src/exclude/**.cpp", 
		"%{prj.location}/src/exclude/**.h"
	}

	includedirs 
	{ 
		external_libs_dir .. "/GLEW/include",
		external_libs_dir .. "/GLFW/%{cfg.platform}/include",
		external_libs_dir .. "/ASSIMP/%{cfg.platform}/include",
		external_libs_dir .. "/FREETYPE/%{cfg.platform}/include", --https://freetype.org/
		--vendor_dir .. "/boost_1_77_0",
		--vendor_dir .. "/cereal-1.3.0/include",
		--vendor_dir .. "/spdlog/include",
		vendor_dir,

		--"%{vendor_dir}/imgui",
        --"%{vendor_dir}/imgui/backends",

		"%{vendor_dir}/imgui_docking",
        "%{vendor_dir}/imgui_docking/backends",

		"%{prj.location}/src", 
		"%{prj.location}/src/framework", 
		"%{prj.location}/src/test_progs", 
		"%{prj.location}/src/test_progs/learn_opengl", 
		"%{prj.location}/src/test_progs/opengl_bible", 
		"%{prj.location}/src/test_progs/misc"
	}

	libdirs 
	{ 
		external_libs_dir .. "/GLEW/lib/release/%{cfg.platform}",
		external_libs_dir .. "/GLFW/%{cfg.platform}/lib-vc2022",
		external_libs_dir .. "/ASSIMP/%{cfg.platform}/lib/release",
		external_libs_dir .. "/FREETYPE/%{cfg.platform}/lib/release"
	}
	
	links
	{
		"glew32s",
		"glfw3",
		"opengl32",
		"assimp-vc143-mt", --64bit version
		"User32",
		"Gdi32",
		"Shell32",
		"freetype"
	}

	defines { "GLEW_STATIC" }
	
		filter "system:windows"
		systemversion "latest"
		--staticruntime "on"
	
	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

	--filter "platforms:Win32"
	--	system "windows"
	--	architecture "x86"
		
	--filter "platforms:Win64"
	--	system "windows"
	--	architecture "x86_64"	

--Don't use pch for anything in the vendor dir'	
	--filter{	"files:C:/dev/vendor/**.cpp" }
	--	flags { "NoPCH" }
	
	filter{	"files:../../vendor/**.cpp" }
		flags { "NoPCH" }	




	