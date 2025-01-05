#pragma once
#include "../../Molecular/Core/Input.h"

namespace Molecular
{
    class WindowsInput : public Input
    {
        bool IsKeyPressedImpl(int keycode);
    };
}



