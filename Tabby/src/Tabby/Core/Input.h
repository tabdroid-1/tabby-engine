#pragma once

#include "Tabby/Core/KeyCodes.h"
#include "Tabby/Core/MouseCodes.h"

#include <glm/glm.hpp>

namespace Tabby {

	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
