#include "Base.h"
#include <Scenes/TestScene.h>

Base::Base()
    : Layer("Base")
{
}

void Base::OnAttach()
{
    TB_PROFILE_FUNCTION();

    Tabby::Renderer2D::SetLineWidth(3.0f);

    Tabby::Ref<TestScene> testScene = Tabby::CreateRef<TestScene>();

    Tabby::SceneStateMachine::Add("Test", testScene);
    Tabby::SceneStateMachine::SwitchTo("Test");
}

void Base::OnDetach()
{
    TB_PROFILE_FUNCTION();
}

void Base::OnUpdate(Tabby::Timestep ts)
{
    TB_PROFILE_FUNCTION();

    Tabby::Renderer2D::ResetStats();
    {
        TB_PROFILE_SCOPE("Renderer Prep");
        Tabby::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        Tabby::RenderCommand::Clear();
    }

    Tabby::SceneStateMachine::Update(ts);
    Tabby::SceneStateMachine::GetCurrentScene()->OnViewportResize(m_ViewPortSize.x, m_ViewPortSize.y);
}

void Base::OnImGuiRender()
{
    TB_PROFILE_FUNCTION();
}

void Base::OnEvent(Tabby::Event& e)
{
    Tabby::EventDispatcher dispatcher(e);
    // dispatcher.Dispatch<Tabby::KeyPressedEvent>(TB_BIND_EVENT_FN(Base::OnKeyPressed));
    // dispatcher.Dispatch<Tabby::MouseButtonPressedEvent>(TB_BIND_EVENT_FN(Base::OnMouseButtonPressed));
}
