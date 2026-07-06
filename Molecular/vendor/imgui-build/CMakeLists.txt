project(ImGui LANGUAGES CXX)

# The imgui submodule is pristine upstream source; this sibling directory owns
# the build definition so the repo tracks it and the submodule stays clean.
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../imgui)

# imgui_demo.cpp is deliberately omitted: the only engine reference is the
# commented-out ShowDemoWindow() in ImGuiLayer.cpp. Re-enabling that call
# requires adding the source back here, or it becomes a link error.
#
# The backend impl sources (imgui_impl_glfw.cpp, imgui_impl_opengl3.cpp) are
# NOT compiled here: Molecular unity-includes them in ImGuiBuild.cpp, so
# compiling them here too would produce duplicate symbols at link time.
add_library(ImGui STATIC
        ${IMGUI_DIR}/imconfig.h
        ${IMGUI_DIR}/imgui.h
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_internal.h
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imstb_rectpack.h
        ${IMGUI_DIR}/imstb_textedit.h
        ${IMGUI_DIR}/imstb_truetype.h
        ${IMGUI_DIR}/imgui_tables.cpp
)

# PUBLIC: any target that links ImGui inherits these, so consumers write
# plain #include "imgui.h". backends/ is exported because the engine
# compiles the GLFW/OpenGL3 impl files itself (ImGuiBuild.cpp).
target_include_directories(ImGui PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)

target_compile_features(ImGui PUBLIC cxx_std_17)