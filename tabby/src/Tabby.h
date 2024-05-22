#pragma once

// For use by Tabby applications
//
// --- Random -----------------------
#include <Tabby/Math/Math.h>
// --- Core -------------------------
#include <Tabby/Core/Application.h>
#include <Tabby/Core/Assert.h>
#include <Tabby/Core/Base.h>
#include <Tabby/Core/Layer/Layer.h>
#include <Tabby/Core/Time/Timestep.h>
#include <Tabby/Core/Log/Log.h>
// -----------------------------------
//
// --- Scene -------------------------
#include <Tabby/World/World.h>
#include <Tabby/World/Components.h>
#include <Tabby/World/Entity.h>
#include <Tabby/World/ScriptableEntity.h>
// -----------------------------------
//
// --- Physics -----------------------
#include <Tabby/Physics/2D/Physics2D.h>
#include <Tabby/Physics/2D/Physics2DTypes.h>
// -----------------------------------
//
// --- Input -------------------------
#include <Tabby/Core/Input/Input.h>
#include <Tabby/Core/Input/KeyCodes.h>
#include <Tabby/Core/Input/MouseCodes.h>
// -----------------------------------
//
// --- Asset -------------------------
#include <Tabby/Asset/AssetManager.h>
// -----------------------------------
//
// --- Renderer ----------------------
#include <Tabby/Renderer/Buffer.h>
#include <Tabby/Renderer/Framebuffer.h>
#include <Tabby/Renderer/RenderCommand.h>
#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Renderer/Renderer2D.h>
#include <Tabby/Renderer/Shader.h>
#include <Tabby/Renderer/Texture.h>
#include <Tabby/Renderer/VertexArray.h>
// -----------------------------------
//
// --- Debug -------------------------
#include <Tabby/Debug/Instrumentor.h>
#include <Tabby/Debug/Debug.h>
// -----------------------------------
//
// --- UI ----------------------------
#include <Tabby/UI/ImGui/ImGuiLayer.h>
// -----------------------------------
