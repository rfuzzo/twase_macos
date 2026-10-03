#include "stdafx.hpp"

#include "LuaConsole.hpp"

#include <imgui.h>

// Runtime toggles for the built-in game tweaks, changes are saved to the [tweaks] section of config.ini.
void LuaConsole::DrawTweaksTab()
{
    // TODO: diplomacy deal score toggle once DiplomacyLikelihood is ported (Phase 5)
    ImGui::TextDisabled("No tweaks are available on macOS yet.");
}
