#pragma once

#if DEBUG
#define TB_ASSERT(condition, message)                                                                                \
    do {                                                                                                             \
        if (!(condition)) {                                                                                          \
            std::cerr << "Assertion failed: " << message << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
            std::abort();                                                                                            \
        }                                                                                                            \
    } while (0)
#else
#define TB_ASSERT(condition, message)
#endif
