#pragma once

//platform detection / check
#ifdef _WIN32
	#define PLATFORM_WINDOWS
#else
	#error "Unsupported platform!"
#endif


#ifdef DEBUG
	#ifdef PLATFORM_WINDOWS
		#define DEBUGBREAK() __debugbreak()
	#else
		#error "Platform doesn't support DEBUGBREAK"
	#endif //PLATFORM_WINDOWS
	#define ENABLE_ASSERTS
#else
	#define DEBUGBREAK()
#endif // DEBUG

#define EXPAND_MACRO(x) x
#define STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#ifdef ENABLE_ASSERTS
	#define ASSERT(x) if(!(x)) DEBUGBREAK()	// __debugbreak is compiler specific
#else
	#define ASSERT(x)
#endif // ENABLE_ASSERTS



