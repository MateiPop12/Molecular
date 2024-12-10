#pragma once
#include "Input.h"

namespace Molecular
{
    class WindowsInput : public Input
    {
        bool IsKeyPressedImpl(int keycode);
    };
}



