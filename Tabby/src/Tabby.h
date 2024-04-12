#pragma once

// For use by Tabby applications

#include <Tabby/Debug/Instrumentor.h>
#include <Tabby/UI/ImGui/ImGuiLayer.h>

// --- Core -------------------------
#include <Tabby/Core/Application.h>
#include <Tabby/Core/Assert.h>
#include <Tabby/Core/Base.h>
#include <Tabby/Core/Layer/Layer.h>
#include <Tabby/Core/Time/Timestep.h>
#include <Tabby/Log/Log.h>
// -----------------------------------

// --- Input -------------------------
#include <Tabby/Input/Input.h>
#include <Tabby/Input/KeyCodes.h>
#include <Tabby/Input/MouseCodes.h>
// -----------------------------------

// --- Scene -------------------------
#include <Tabby/Scene/Components.h>
#include <Tabby/Scene/Entity.h>
#include <Tabby/Scene/Scene.h>
#include <Tabby/Scene/SceneManager.h>
#include <Tabby/Scene/ScriptableEntity.h>
// -----------------------------------

// --- Asset -------------------------
// #include <Tabby/Asset/Importers/TextureImporter.h>
#include <Tabby/Asset/AssetManager.h>
// -----------------------------------

// ---Renderer------------------------
#include <Tabby/Renderer/Buffer.h>
#include <Tabby/Renderer/Framebuffer.h>
#include <Tabby/Renderer/RenderCommand.h>
#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Renderer/Renderer2D.h>
#include <Tabby/Renderer/Shader.h>
#include <Tabby/Renderer/Texture.h>
#include <Tabby/Renderer/VertexArray.h>
// -----------------------------------
