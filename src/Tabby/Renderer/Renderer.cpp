#include <Tabby/Renderer/ShaderLibrary.h>
#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Renderer/Image.h>
#include <Tabby/Utils/Utils.h>

#include <Tabby/Core/Application.h>

#include <glm/gtc/packing.hpp>
#include <bx/math.h>

namespace Tabby {

static std::unordered_map<uint8_t, bgfx::FrameBufferHandle> s_FrameBuffers;
static std::unordered_map<uint8_t, ViewSpecification> s_ViewSpecifications;

static void InitRenderer2D();

void Renderer::Init()
{
    ShaderLibrary::Init();
    InitRenderer2D();
}

void Renderer::Shutdown()
{
    ShaderLibrary::Shutdown();
}

void Renderer::SetViewTarget(const ViewSpecification& spec)
{
    // check if recreation is needed.
    if (s_ViewSpecifications.find(spec.view_id) != s_ViewSpecifications.end()) {
        if (s_ViewSpecifications.find(spec.view_id)->second == spec) {
            return;
        }
    }

    s_ViewSpecifications[spec.view_id] = spec;

    std::vector<bgfx::Attachment> framebuffer_attachments;

    if (s_FrameBuffers.find(spec.view_id) != s_FrameBuffers.end()) {
        if (bgfx::isValid(s_FrameBuffers.find(spec.view_id)->second)) {
            bgfx::destroy(s_FrameBuffers.find(spec.view_id)->second);
            s_FrameBuffers.erase(spec.view_id);
        }
    }

    for (auto render_target : spec.render_targets) {

        if (!render_target)
            continue;

        auto tex_spec = render_target->GetSpecification();

        if (tex_spec.extent != spec.render_area) {
            tex_spec.extent = spec.render_area;
            render_target->Update(tex_spec);
        }

        bgfx::Attachment framebuffer_attachment;
        framebuffer_attachment.init(render_target->Raw());
        framebuffer_attachments.push_back(framebuffer_attachment);
    }

    uint16_t flags = 0
        | BGFX_CLEAR_COLOR;

    if (spec.depth_buffer) {
        auto tex_spec = spec.depth_buffer->GetSpecification();

        if (tex_spec.extent != spec.render_area) {
            tex_spec.extent = spec.render_area;
            spec.depth_buffer->Update(tex_spec);
        }

        bgfx::Attachment framebuffer_attachment;
        framebuffer_attachment.init(spec.depth_buffer->Raw());
        framebuffer_attachments.push_back(framebuffer_attachment);

        flags |= BGFX_CLEAR_DEPTH;
    }

    s_FrameBuffers[spec.view_id] = bgfx::createFrameBuffer(framebuffer_attachments.size(), framebuffer_attachments.data(), false);

    bgfx::setViewFrameBuffer(spec.view_id, s_FrameBuffers[spec.view_id]);
    bgfx::setViewRect(spec.view_id, spec.offset.x, spec.offset.y, spec.render_area.x, spec.render_area.y);
    bgfx::setViewClear(spec.view_id, flags, Utils::ColorToHex(spec.clear_value), 1.0f, 0);
    bgfx::touch(spec.view_id);
}

void Renderer::SetViewMatrix(uint8_t view_id, const Matrix4& view, const Matrix4& projection)
{
    bgfx::setViewTransform(view_id, glm::value_ptr(view), glm::value_ptr(projection));
}

void Renderer::UnsetViewTarget(uint8_t view_id)
{
    if (s_FrameBuffers.find(view_id) != s_FrameBuffers.end()) {
        if (bgfx::isValid(s_FrameBuffers.find(view_id)->second)) {
            bgfx::destroy(s_FrameBuffers.find(view_id)->second);
            s_FrameBuffers.erase(view_id);
            s_ViewSpecifications.erase(view_id);
            bgfx::setViewFrameBuffer(view_id, BGFX_INVALID_HANDLE);
        }
    }
}

void Renderer::DrawMesh(uint8_t view_id, Shared<Mesh> mesh)
{

    bgfx::setTransform(glm::value_ptr(mesh->m_Transform));
    bgfx::setVertexBuffer(0, mesh->m_Specification.vertex_buffer_handle);
    bgfx::setIndexBuffer(mesh->m_Specification.index_buffer_handle);

    bgfx::setState(mesh->m_Specification.state);

    bgfx::submit(view_id, mesh->m_Specification.program_handle);
}

void Renderer::Frame()
{
    bgfx::frame();
}

// ------------------------------------
// ------ Renderer 2D -----------------
// ------------------------------------

struct QuadVertex {
    Vector3 position;
    Vector4 color;
    Vector2 tex_coord;
    float tex_index;
    float tiling_factor;
    // int entity_id;
};

struct TextVertex {
    Vector3 position;
    Vector4 color;
    Vector2 tex_coord;
    float tex_index;
    // TODO: bg color for outline/bg
    // int EntityID;
};

struct Renderer2DData {
    static constexpr uint32_t s_MaxQuads = 10000;
    static constexpr uint32_t s_MaxVertices = s_MaxQuads * 4;
    static constexpr uint32_t s_MaxIndices = s_MaxQuads * 6;
    static constexpr uint32_t s_MaxTextureSlots = 16;

    inline static bool s_Initialized = false;
    inline static uint8_t s_ViewID = 0;
    inline static bgfx::VertexLayout s_QuadVertexLayout;
    inline static bgfx::VertexLayout s_TextVertexLayout;

    bgfx::DynamicVertexBufferHandle quad_vertex_buffer;
    bgfx::IndexBufferHandle quad_index_buffer;
    bgfx::ProgramHandle quad_program;

    bgfx::DynamicVertexBufferHandle text_vertex_buffer;
    bgfx::IndexBufferHandle text_index_buffer;
    bgfx::ProgramHandle text_program;

    uint32_t quad_index_count = 0;
    QuadVertex* quad_vertex_buffer_base = nullptr;
    QuadVertex* quad_vertex_buffer_ptr = nullptr;

    uint32_t text_index_count = 0;
    TextVertex* text_vertex_buffer_base = nullptr;
    TextVertex* text_vertex_buffer_ptr = nullptr;

    std::array<bgfx::UniformHandle, s_MaxTextureSlots> texture_uniforms;
    std::array<Shared<Image>, s_MaxTextureSlots> texture_slots;
    uint32_t texture_slot_index = 1; // 0 = missing texture

    std::array<bgfx::UniformHandle, s_MaxTextureSlots> font_texture_uniforms;
    std::array<Shared<Image>, s_MaxTextureSlots> font_texture_slots;
    uint32_t font_texture_slot_index = 1; // 0 = missing texture

    Vector4 quad_vertex_positions[4];
};

static Renderer2DData s_Renderer2DData;

static void InitRenderer2D()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Renderer2D::Init");

    if (s_Renderer2DData.s_Initialized)
        return;

    s_Renderer2DData.s_QuadVertexLayout
        .begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float, true)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true) // texcoord
        .add(bgfx::Attrib::TexCoord1, 1, bgfx::AttribType::Float) // tex_index
        .add(bgfx::Attrib::TexCoord2, 1, bgfx::AttribType::Float) // tiling_factor
        .end();

    s_Renderer2DData.s_TextVertexLayout
        .begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float, true)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true) // texcoord
        .add(bgfx::Attrib::TexCoord1, 1, bgfx::AttribType::Float) // tex_index
        .end();

    s_Renderer2DData.texture_slots[0] = AssetManager::GetMissingTexture();
    for (uint32_t i = 0; i < s_Renderer2DData.s_MaxTextureSlots; i++) {
        bgfx::setTexture(i, s_Renderer2DData.texture_uniforms[i], s_Renderer2DData.texture_slots[0]->Raw());
    }

    s_Renderer2DData.font_texture_slots[0] = AssetManager::GetMissingTexture();
    for (uint32_t i = 0; i < s_Renderer2DData.s_MaxTextureSlots; i++) {
        bgfx::setTexture(i, s_Renderer2DData.font_texture_uniforms[i], s_Renderer2DData.font_texture_slots[0]->Raw());
    }

    std::string quad_vertex_path;
    std::string quad_fragment_path;

    std::string text_vertex_path;
    std::string text_fragment_path;

    // More ellegant way of loading shaders after dealing with shaderc(bgfx)
    switch (bgfx::getRendererType()) {
    case bgfx::RendererType::OpenGL:
        quad_vertex_path = "shaders/quad_shader/glsl/vs.sc.bin";
        quad_fragment_path = "shaders/quad_shader/glsl/fs.sc.bin";
        text_vertex_path = "shaders/text_shader/glsl/vs.sc.bin";
        text_fragment_path = "shaders/text_shader/glsl/fs.sc.bin";
        break;
    case bgfx::RendererType::OpenGLES:
        quad_vertex_path = "shaders/quad_shader/essl/vs.sc.bin";
        quad_fragment_path = "shaders/quad_shader/essl/fs.sc.bin";
        text_vertex_path = "shaders/text_shader/essl/vs.sc.bin";
        text_fragment_path = "shaders/text_shader/essl/fs.sc.bin";
        break;
    case bgfx::RendererType::Vulkan:
        quad_vertex_path = "shaders/quad_shader/spv/vs.sc.bin";
        quad_fragment_path = "shaders/quad_shader/spv/fs.sc.bin";
        text_vertex_path = "shaders/text_shader/spv/vs.sc.bin";
        text_fragment_path = "shaders/text_shader/spv/fs.sc.bin";
        break;
    default:
        break;
    }

    ShaderLibrary::LoadShader("quad_shader", quad_vertex_path, quad_fragment_path);
    s_Renderer2DData.quad_program = ShaderLibrary::GetShader("quad_shader");

    ShaderLibrary::LoadShader("text_shader", text_vertex_path, text_fragment_path);
    s_Renderer2DData.text_program = ShaderLibrary::GetShader("text_shader");

    s_Renderer2DData.quad_vertex_buffer_base = new QuadVertex[s_Renderer2DData.s_MaxVertices];
    s_Renderer2DData.quad_vertex_buffer = bgfx::createDynamicVertexBuffer(s_Renderer2DData.s_MaxVertices, Renderer2DData::s_QuadVertexLayout);

    s_Renderer2DData.text_vertex_buffer_base = new TextVertex[s_Renderer2DData.s_MaxVertices];
    s_Renderer2DData.text_vertex_buffer = bgfx::createDynamicVertexBuffer(s_Renderer2DData.s_MaxVertices, Renderer2DData::s_TextVertexLayout);

    uint16_t* quadIndices = new uint16_t[s_Renderer2DData.s_MaxIndices];

    uint16_t offset = 0;
    for (uint16_t i = 0; i < s_Renderer2DData.s_MaxIndices; i += 6) {
        quadIndices[i + 0] = offset + 0;
        quadIndices[i + 1] = offset + 1;
        quadIndices[i + 2] = offset + 2;

        quadIndices[i + 3] = offset + 2;
        quadIndices[i + 4] = offset + 3;
        quadIndices[i + 5] = offset + 0;

        offset += 4;
    }

    s_Renderer2DData.quad_index_buffer = bgfx::createIndexBuffer(bgfx::copy(quadIndices, s_Renderer2DData.s_MaxIndices));
    s_Renderer2DData.text_index_buffer = bgfx::createIndexBuffer(bgfx::copy(quadIndices, s_Renderer2DData.s_MaxIndices));
    delete[] quadIndices;

    s_Renderer2DData.quad_vertex_positions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
    s_Renderer2DData.quad_vertex_positions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
    s_Renderer2DData.quad_vertex_positions[2] = { 0.5f, 0.5f, 0.0f, 1.0f };
    s_Renderer2DData.quad_vertex_positions[3] = { -0.5f, 0.5f, 0.0f, 1.0f };

    for (int i = 0; i < s_Renderer2DData.s_MaxTextureSlots; i++) {
        std::string name = "u_Albedo" + std::to_string(i);
        s_Renderer2DData.texture_uniforms[i] = bgfx::createUniform(name.c_str(), bgfx::UniformType::Sampler);
        bgfx::setTexture(i, s_Renderer2DData.texture_uniforms[i], s_Renderer2DData.texture_slots[0]->Raw());

        s_Renderer2DData.texture_uniforms[i] = bgfx::createUniform(name.c_str(), bgfx::UniformType::Sampler);
        bgfx::setTexture(i, s_Renderer2DData.texture_uniforms[i], s_Renderer2DData.texture_slots[0]->Raw());
    }

    s_Renderer2DData.s_Initialized = true;
}

static void StartQuadBatch()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Renderer2D::NextQuadBatch");

    s_Renderer2DData.quad_index_count = 0;
    s_Renderer2DData.quad_vertex_buffer_ptr = s_Renderer2DData.quad_vertex_buffer_base;
}

static void StartTextBatch()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Renderer2D::NextTextBatch");

    s_Renderer2DData.text_index_count = 0;
    s_Renderer2DData.text_vertex_buffer_ptr = s_Renderer2DData.text_vertex_buffer_base;
}

static void FlushQuad()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Renderer2D::NextBatch");

    if (s_Renderer2DData.quad_index_count) {

        uint32_t dataSize = (uint32_t)((uint8_t*)s_Renderer2DData.quad_vertex_buffer_ptr - (uint8_t*)s_Renderer2DData.quad_vertex_buffer_base);

        bgfx::update(s_Renderer2DData.quad_vertex_buffer, 0, bgfx::copy(s_Renderer2DData.quad_vertex_buffer_base, dataSize));

        bgfx::setTransform(glm::value_ptr(Matrix4(1))); // quads are already transformed in vertex buffer.
        bgfx::setVertexBuffer(0, s_Renderer2DData.quad_vertex_buffer);
        bgfx::setIndexBuffer(s_Renderer2DData.quad_index_buffer, 0, s_Renderer2DData.quad_index_count);

        for (uint32_t i = 0; i < s_Renderer2DData.texture_slot_index; i++) {
            bgfx::setTexture(i, s_Renderer2DData.texture_uniforms[i], s_Renderer2DData.texture_slots[i]->Raw());
        }

        uint64_t state = 0
            | BGFX_STATE_MSAA
            | BGFX_STATE_WRITE_Z
            | BGFX_STATE_WRITE_A
            | BGFX_STATE_WRITE_RGB
            | BGFX_STATE_BLEND_ALPHA
            | BGFX_STATE_DEPTH_TEST_LESS;

        bgfx::setState(state);

        bgfx::submit(s_Renderer2DData.s_ViewID, s_Renderer2DData.quad_program);
    }
}

static void FlushText()
{
    if (s_Renderer2DData.text_index_count) {

        uint32_t dataSize = (uint32_t)((uint8_t*)s_Renderer2DData.text_vertex_buffer_ptr - (uint8_t*)s_Renderer2DData.text_vertex_buffer_base);

        bgfx::update(s_Renderer2DData.text_vertex_buffer, 0, bgfx::copy(s_Renderer2DData.text_vertex_buffer_base, dataSize));

        bgfx::setTransform(glm::value_ptr(Matrix4(1))); // texts are already transformed in vertex buffer.
        bgfx::setVertexBuffer(0, s_Renderer2DData.text_vertex_buffer);
        bgfx::setIndexBuffer(s_Renderer2DData.text_index_buffer, 0, s_Renderer2DData.text_index_count);

        for (uint32_t i = 0; i < s_Renderer2DData.font_texture_slot_index; i++) {
            bgfx::setTexture(i, s_Renderer2DData.font_texture_uniforms[i], s_Renderer2DData.font_texture_slots[i]->Raw());
        }

        uint64_t state = 0
            | BGFX_STATE_MSAA
            | BGFX_STATE_WRITE_Z
            | BGFX_STATE_WRITE_A
            | BGFX_STATE_WRITE_RGB
            | BGFX_STATE_BLEND_ALPHA
            | BGFX_STATE_DEPTH_TEST_LESS;

        bgfx::setState(state);

        bgfx::submit(s_Renderer2DData.s_ViewID, s_Renderer2DData.text_program);
    }
}

static void NextQuadBatch()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Renderer2D::NextBatch");

    FlushQuad();
    StartQuadBatch();
}

static void NextTextBatch()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Renderer2D::NextBatch");

    FlushText();
    StartTextBatch();
}

void Renderer::DrawQuad(const Matrix4& transform, Shared<Image> texture, const Vector4& color, float tiling_factor, int horizontal_frames, int current_horizontal_frame, int vertical_frames, int current_vertical_frame)
{

    TB_PROFILE_SCOPE_NAME("Tabby::Renderer2D::DrawQuad");

    constexpr size_t quadVertexCount = 4;

    const Vector2 textureCoords[] = {
        { current_horizontal_frame / (float)horizontal_frames, current_vertical_frame / (float)vertical_frames },
        { (current_horizontal_frame + 1) / (float)horizontal_frames, current_vertical_frame / (float)vertical_frames },
        { (current_horizontal_frame + 1) / (float)horizontal_frames, (current_vertical_frame + 1) / (float)vertical_frames },
        { current_horizontal_frame / (float)horizontal_frames, (current_vertical_frame + 1) / (float)vertical_frames }
    };

    if (!s_Renderer2DData.s_Initialized)
        InitRenderer2D();

    if (s_Renderer2DData.quad_index_count >= Renderer2DData::s_MaxIndices)
        NextQuadBatch();

    float textureIndex = 0.0f;
    for (uint32_t i = 1; i < s_Renderer2DData.texture_slot_index; i++) {
        if (!texture)
            break;

        if (s_Renderer2DData.texture_slots[i]->Raw().idx == texture->Raw().idx) {
            textureIndex = (float)i;
            break;
        }
    }

    if (textureIndex == 0.0f && texture) {
        if (s_Renderer2DData.texture_slot_index >= Renderer2DData::s_MaxTextureSlots)
            NextQuadBatch();

        textureIndex = (float)s_Renderer2DData.texture_slot_index;
        s_Renderer2DData.texture_slots[s_Renderer2DData.texture_slot_index] = texture;
        s_Renderer2DData.texture_slot_index++;
    }

    for (size_t i = 0; i < quadVertexCount; i++) {
        s_Renderer2DData.quad_vertex_buffer_ptr->position = transform * s_Renderer2DData.quad_vertex_positions[i];
        s_Renderer2DData.quad_vertex_buffer_ptr->color = color;
        s_Renderer2DData.quad_vertex_buffer_ptr->tex_coord = textureCoords[i];
        s_Renderer2DData.quad_vertex_buffer_ptr->tex_index = textureIndex;
        s_Renderer2DData.quad_vertex_buffer_ptr->tiling_factor = tiling_factor;
        // s_Renderer2DData.quad_vertex_buffer_ptr->entity_id = entityID;
        s_Renderer2DData.quad_vertex_buffer_ptr++;
    }

    s_Renderer2DData.quad_index_count += 6;
}

void Renderer::DrawString(const std::wstring& string, Shared<Font> font, const Matrix4& transform, const TextParams& text_params)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Renderer2D::DrawString");

    const auto& atlas_data = font->GetAtlasData();
    AssetHandle fontAtlasHandle = font->GetAtlasData().texture;
    auto fontAtlas = AssetManager::GetAsset<Image>(fontAtlasHandle);

    if (s_Renderer2DData.text_index_count >= Renderer2DData::s_MaxIndices)
        NextTextBatch();

    float fontAtlasIndex = -1.0f;
    for (uint32_t i = 0; i < s_Renderer2DData.font_texture_slot_index; i++) {
        if (s_Renderer2DData.font_texture_slots[i]->Raw().idx == fontAtlas->Raw().idx) {
            fontAtlasIndex = (float)i;
            break;
        }
    }

    if (fontAtlasIndex == -1.0f) {
        if (s_Renderer2DData.font_texture_slot_index >= Renderer2DData::s_MaxTextureSlots)
            NextTextBatch();

        fontAtlasIndex = (float)s_Renderer2DData.font_texture_slot_index;
        s_Renderer2DData.font_texture_slots[s_Renderer2DData.font_texture_slot_index] = fontAtlas;
        s_Renderer2DData.font_texture_slot_index++;
    }

    double x = 0.0;
    double fsScale = 1.0 / (atlas_data.ascender - atlas_data.descender);
    double y = 0.0;

    const float spaceGlyphAdvance = font->GetCharacterByCodepoint(' ').advance;

    for (size_t i = 0; i < string.size(); i++) {
        char character = string[i];
        if (character == '\r')
            continue;

        if (character == '\n') {
            x = 0;
            y -= fsScale * atlas_data.line_height + text_params.line_spacing;
            continue;
        }

        if (character == ' ') {
            float advance = spaceGlyphAdvance;
            if (i < string.size() - 1) {
                char nextCharacter = string[i + 1];
                double dAdvance = font->GetCharacterAdvanceByCodepoint(character, nextCharacter);
                advance = (float)dAdvance;
            }

            x += fsScale * advance + text_params.kerning;
            continue;
        }

        if (character == '\t') {
            x += 4.0f * (fsScale * spaceGlyphAdvance + text_params.kerning);
            continue;
        }

        auto glyph = font->GetCharacterByCodepoint(character);
        if (!glyph)
            glyph = font->GetCharacterByCodepoint('?');
        if (!glyph)
            return;

        // double al, ab, ar, at;
        // glyph->getQuadAtlasBounds(al, ab, ar, at);
        Vector2 texCoordMin((float)glyph.atlas_bound_left, (float)glyph.atlas_bound_bottom);
        Vector2 texCoordMax((float)glyph.atlas_bound_right, (float)glyph.atlas_bound_top);

        // double pl, pb, pr, pt;
        // glyph->getQuadPlaneBounds(pl, pb, pr, pt);
        Vector2 quadMin((float)glyph.plane_bound_left, (float)glyph.plane_bound_bottom);
        Vector2 quadMax((float)glyph.plane_bound_right, (float)glyph.plane_bound_top);

        quadMin *= fsScale, quadMax *= fsScale;
        quadMin += Vector2(x, y);
        quadMax += Vector2(x, y);

        float texelWidth = 1.0f / fontAtlas->GetSpecification().extent.x;
        float texelHeight = 1.0f / fontAtlas->GetSpecification().extent.y;
        texCoordMin *= Vector2(texelWidth, texelHeight);
        texCoordMax *= Vector2(texelWidth, texelHeight);

        // render here
        s_Renderer2DData.text_vertex_buffer_ptr->position = transform * Vector4(quadMin, 0.0f, 1.0f);
        s_Renderer2DData.text_vertex_buffer_ptr->color = text_params.color;
        s_Renderer2DData.text_vertex_buffer_ptr->tex_coord = texCoordMin;
        s_Renderer2DData.text_vertex_buffer_ptr->tex_index = fontAtlasIndex;
        // s_Renderer2DData.text_vertex_buffer_ptr->EntityID = entityID;
        s_Renderer2DData.text_vertex_buffer_ptr++;

        s_Renderer2DData.text_vertex_buffer_ptr->position = transform * Vector4(quadMin.x, quadMax.y, 0.0f, 1.0f);
        s_Renderer2DData.text_vertex_buffer_ptr->color = text_params.color;
        s_Renderer2DData.text_vertex_buffer_ptr->tex_coord = { texCoordMin.x, texCoordMax.y };
        s_Renderer2DData.text_vertex_buffer_ptr->tex_index = fontAtlasIndex;
        // s_Renderer2DData.text_vertex_buffer_ptr->EntityID = entityID;
        s_Renderer2DData.text_vertex_buffer_ptr++;

        s_Renderer2DData.text_vertex_buffer_ptr->position = transform * Vector4(quadMax, 0.0f, 1.0f);
        s_Renderer2DData.text_vertex_buffer_ptr->color = text_params.color;
        s_Renderer2DData.text_vertex_buffer_ptr->tex_coord = texCoordMax;
        s_Renderer2DData.text_vertex_buffer_ptr->tex_index = fontAtlasIndex;
        // s_Renderer2DData.text_vertex_buffer_ptr->EntityID = entityID;
        s_Renderer2DData.text_vertex_buffer_ptr++;

        s_Renderer2DData.text_vertex_buffer_ptr->position = transform * Vector4(quadMax.x, quadMin.y, 0.0f, 1.0f);
        s_Renderer2DData.text_vertex_buffer_ptr->color = text_params.color;
        s_Renderer2DData.text_vertex_buffer_ptr->tex_coord = { texCoordMax.x, texCoordMin.y };
        s_Renderer2DData.text_vertex_buffer_ptr->tex_index = fontAtlasIndex;
        // s_Renderer2DData.text_vertex_buffer_ptr->EntityID = entityID;
        s_Renderer2DData.text_vertex_buffer_ptr++;

        s_Renderer2DData.text_index_count += 6;
        // s_Renderer2DData.Stats.QuadCount++;

        if (i < string.size() - 1) {
            char nextCharacter = string[i + 1];
            double advance = font->GetCharacterAdvanceByCodepoint(character, nextCharacter);

            x += fsScale * advance + text_params.kerning;
        }
    }
}

void Renderer::Begin2D(uint8_t view_id)
{
    if (!s_Renderer2DData.s_Initialized)
        InitRenderer2D();

    s_Renderer2DData.s_ViewID = view_id;
    StartQuadBatch();
    StartTextBatch();
}

void Renderer::End2D()
{
    if (!s_Renderer2DData.s_Initialized)
        InitRenderer2D();

    FlushQuad();
    FlushText();
}

}
