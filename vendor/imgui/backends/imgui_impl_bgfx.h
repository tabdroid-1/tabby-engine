#pragma once

// Derived from this Gist by Richard Gale:
//     https://gist.github.com/RichardGale/6e2b74bc42b3005e08397236e4be0fd0

// ImGui BGFX binding

void ImGui_Implbgfx_Init(int view);
void ImGui_Implbgfx_Shutdown();
void ImGui_Implbgfx_NewFrame();
void ImGui_Implbgfx_RenderDrawLists(struct ImDrawData* draw_data);

// Use if you want to reset your rendering device without losing ImGui state.
void ImGui_Implbgfx_InvalidateDeviceObjects();
bool ImGui_Implbgfx_CreateDeviceObjects();
