#pragma once

namespace Tabby {
using MouseCode = uint32_t;

namespace Mouse {
    enum : MouseCode {
        Button0 = 1,
        Button1 = 3,
        Button2 = 2,
        Button3 = 4,
        Button4 = 5,

        ButtonLeft = Button0,
        ButtonRight = Button1,
        ButtonMiddle = Button2
    };
}
}
