#pragma once
#include <filesystem>
#include <string_view>

namespace Molecular::Assets
{
    std::filesystem::path Root();
    std::filesystem::path Path(std::string_view relative);

    bool Exists(std::string_view relative);
}
