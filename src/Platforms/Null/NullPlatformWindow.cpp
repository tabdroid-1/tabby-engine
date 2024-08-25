#include <tbpch.h>
#include <Platforms/Null/NullPlatformWindow.h>

namespace Tabby {

NullPlatformWindow::NullPlatformWindow(const WindowProps& props)
{
}

NullPlatformWindow::~NullPlatformWindow()
{
}

void NullPlatformWindow::Init(const WindowProps& props)
{
}

void NullPlatformWindow::Shutdown()
{
}

void NullPlatformWindow::OnUpdate()
{
}

void NullPlatformWindow::SetVSync(bool enabled)
{
}

bool NullPlatformWindow::IsVSync() const
{
    return false;
}

void NullPlatformWindow::SetResizable(bool enabled)
{
}

bool NullPlatformWindow::IsResizable() const
{
    return false;
}

void NullPlatformWindow::SetMinSize(uint32_t minWidth, uint32_t minHeight)
{
}

void NullPlatformWindow::SetFullscreen(uint8_t mode)
{
}

uint8_t NullPlatformWindow::GetFullscreenMode() const
{
    return 0;
}
}
