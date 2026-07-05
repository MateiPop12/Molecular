#include "Assets.h"

#include <stdexcept>

namespace Molecular::Assets
{
    std::filesystem::path Root()
    {
        static std::filesystem::path s_Root = std::filesystem::path(MOL_ASSETS_DIR).lexically_normal();
        return s_Root;
    }

    std::filesystem::path Path(const std::string_view relative)
    {
        if (relative.empty())
            throw std::invalid_argument("Path must not be empty");

        const std::filesystem::path rel = std::filesystem::path(relative).lexically_normal();

        if (rel.has_root_directory() || rel.is_absolute() || rel.has_root_name())
            throw std::invalid_argument("Path must be relative");

        std::filesystem::path joined = (Root() / rel).lexically_normal();

        const std::filesystem::path check = joined.lexically_relative(Root());
        if (check.empty() || *check.begin() == "..")
            throw std::invalid_argument("Path must not escape the assets directory");

        return joined;
    }

    bool Exists(std::string_view relative)
    {
        return std::filesystem::exists(Path(relative));
    }
}