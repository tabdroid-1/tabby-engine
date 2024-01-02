#pragma once

#include "Tabby/Core/Buffer.h"

namespace Tabby {

	class FileSystem
	{
	public:
		// TODO: move to FileSystem class
		static Buffer ReadFileBinary(const std::filesystem::path& filepath);
	};

}
