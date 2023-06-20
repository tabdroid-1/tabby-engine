#include "tbpch.h"
#include "Tabby/Core/Window.h"

#if defined(TB_PLATFORM_WINDOWS)
	#include "Platform/Windows/WindowsWindow.h"
#elif defined(TB_PLATFORM_LINUX)
	#include "Platform/Linux/LinuxWindow.h"
#elif defined(TB_PLATFORM_MACOS)
	#include "Platform/Mac/MacWindow.h"
#endif

namespace Tabby
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
	#if defined(TB_PLATFORM_WINDOWS)
		return CreateScope<WindowsWindow>(props);
	#elif defined(TB_PLATFORM_LINUX)
		return CreateScope<LinuxWindow>(props);
	#elif defined(TB_PLATFORM_MACOS)
		return CreateScope<MacWindow>(props);
	#else
		TB_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
	#endif
	}

}