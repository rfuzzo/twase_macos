#include "stdafx.hpp"

#include "WinMain.hpp"

#include "../Hooking/Hook.hpp"

#include "../../sdk/Attila/Addresses.hpp"

namespace
{
int64_t WinMain(void* aInstance, void* aPrevInstance, const char16_t* aCommandLine);
Hook<decltype(&WinMain)> WinMain_fnc(sdk::Attila::Addresses::WinMain, &WinMain);

std::string ToUtf8(const char16_t* aText)
{
    std::string result;
    if (!aText)
    {
        return result;
    }

    for (; *aText; ++aText)
    {
        uint32_t c = *aText;

        // surrogate pair
        if (c >= 0xD800 && c <= 0xDBFF && aText[1] >= 0xDC00 && aText[1] <= 0xDFFF)
        {
            c = 0x10000 + ((c - 0xD800) << 10) + (aText[1] - 0xDC00);
            ++aText;
        }

        if (c < 0x80)
        {
            result += static_cast<char>(c);
        }
        else if (c < 0x800)
        {
            result += static_cast<char>(0xC0 | (c >> 6));
            result += static_cast<char>(0x80 | (c & 0x3F));
        }
        else if (c < 0x10000)
        {
            result += static_cast<char>(0xE0 | (c >> 12));
            result += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
            result += static_cast<char>(0x80 | (c & 0x3F));
        }
        else
        {
            result += static_cast<char>(0xF0 | (c >> 18));
            result += static_cast<char>(0x80 | ((c >> 12) & 0x3F));
            result += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
            result += static_cast<char>(0x80 | (c & 0x3F));
        }
    }

    return result;
}

void LogMods(std::string_view aCommandLine)
{
    // statements are separated by ';', e.g. "mod @Fireforged-Empire_1.pack;"
    std::vector<std::string_view> mods;
    size_t start = 0;
    while (start < aCommandLine.size())
    {
        auto end = aCommandLine.find(';', start);
        if (end == std::string_view::npos)
        {
            end = aCommandLine.size();
        }

        auto statement = aCommandLine.substr(start, end - start);
        while (!statement.empty() && statement.front() == ' ')
        {
            statement.remove_prefix(1);
        }

        if (statement.starts_with("mod "))
        {
            mods.push_back(statement.substr(4));
        }

        start = end + 1;
    }

    if (mods.empty())
    {
        spdlog::info("No mods are enabled");
        return;
    }

    spdlog::info("Mods enabled ({}):", mods.size());
    for (auto mod : mods)
    {
        spdlog::info("  - {}", mod);
    }
}

int64_t WinMain(void* aInstance, void* aPrevInstance, const char16_t* aCommandLine)
{
    auto commandLine = ToUtf8(aCommandLine);
    spdlog::info("Game command line: {}", commandLine);
    LogMods(commandLine);

    return WinMain_fnc(aInstance, aPrevInstance, aCommandLine);
}
} // namespace

bool Hooks::WinMainHook::Attach()
{
    spdlog::trace("Trying to attach the WinMain hook at {:#x}...", WinMain_fnc.GetAddress());

    auto result = WinMain_fnc.Attach();
    if (result != 0)
    {
        spdlog::error("Could not attach the WinMain hook. Dobby error code: {}", result);
        return false;
    }

    spdlog::info("The WinMain hook was attached");
    return true;
}
