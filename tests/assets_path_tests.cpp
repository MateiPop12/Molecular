#include "vendor/doctest/doctest.h"

#include "Molecular/Core/Assets.h"

#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;
using namespace Molecular;

TEST_CASE("Root is absolute and normalized")
{
    const fs::path root = Assets::Root();
    CHECK(root.is_absolute());
    CHECK(root == root.lexically_normal());
    CHECK(fs::exists(root)); // the assets dir itself must exist
}

TEST_CASE("Path resolves relative paths under the root")
{
    const fs::path p = Assets::Path("shaders/whatever.glsl");
    CHECK(p == (Assets::Root() / "shaders" / "whatever.glsl").lexically_normal());
}

TEST_CASE("Path rejects absolute inputs")
{
    CHECK_THROWS_AS((void)Assets::Path("C:/Windows/System32"), std::invalid_argument);
}

TEST_CASE("Path rejects escapes from the root")
{
    CHECK_THROWS_AS((void)Assets::Path("../outside.txt"), std::invalid_argument);
    CHECK_THROWS_AS((void)Assets::Path("shaders/../../outside.txt"), std::invalid_argument);
}

TEST_CASE("Path accepts dotdot that stays inside the root")
{
    const fs::path p = Assets::Path("shaders/../textures/x.png");
    CHECK(p == (Assets::Root() / "textures" / "x.png").lexically_normal());
}

TEST_CASE("Exists reports real files")
{
    CHECK(Assets::Exists("shaders/FlatColor.glsl"));
    CHECK_FALSE(Assets::Exists("definitely/not/here.bin"));
}

TEST_CASE("Path rejects root-relative inputs that jump to the drive root")
{
    CHECK_THROWS_AS((void)Assets::Path("/shaders/Texture.glsl"), std::invalid_argument);
}

TEST_CASE("Path rejects drive-relative inputs (reviewer-added)")
{
    CHECK_THROWS_AS((void)Assets::Path("C:foo"), std::invalid_argument);
}

TEST_CASE("Path rejects empty input")
{
    CHECK_THROWS_AS((void)Assets::Path(""), std::invalid_argument);
}
