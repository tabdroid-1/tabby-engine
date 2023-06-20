#pragma once

#include <filesystem>

#include "Tabby/Core/Base.h"
#include "Tabby/Renderer/Texture.h"

namespace Tabby {

	struct MSDFData;

	class Font
	{
	public:
		Font(const std::filesystem::path& font);
		~Font();

		// const MSDFData* GetMSDFData() const { return m_Data; }
		Ref<Texture2D> GetAtlasTexture() const { return m_AtlasTexture; }

		static Ref<Font> GetDefault();
	private:
		// MSDFData* m_Data;
		Ref<Texture2D> m_AtlasTexture;
	};

}
