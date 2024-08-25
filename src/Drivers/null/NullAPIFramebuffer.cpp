#include <tbpch.h>
#include <Drivers/null/NullAPIFramebuffer.h>

namespace Tabby {

NullAPIFramebuffer::NullAPIFramebuffer(const FramebufferSpecification& spec)
    : m_Specification(spec)
{
}

NullAPIFramebuffer::~NullAPIFramebuffer()
{
}

void NullAPIFramebuffer::Bind()
{
}

void NullAPIFramebuffer::Unbind()
{
}

void NullAPIFramebuffer::Resize(uint32_t width, uint32_t height)
{
}

int NullAPIFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
{
    return 0;
}

void NullAPIFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
{
}
}
