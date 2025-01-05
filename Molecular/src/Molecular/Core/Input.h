#pragma once

#include "Core.h"

namespace Molecular
{
    class MOLECULAR_API Input
    {
        public:
        static bool IsKeyPressed(int key);

        static bool IsMouseButtonPressed(int button);
        //static glm::vec2 GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();
    };
}