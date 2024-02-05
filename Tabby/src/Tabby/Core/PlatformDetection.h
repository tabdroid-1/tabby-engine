// Platform detection using predefined macros
#ifdef _WIN32
/* Windows x64/x86 */
#ifdef _WIN64
/* Windows x64  */
#define TB_PLATFORM_WINDOWS
#define TB_ARCH_X64
#else
/* Windows x86 */
#define TB_PLATFORM_WINDOWS
#define TB_ARCH_X86
#endif
#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
/* TARGET_OS_MAC exists on all the platforms
 * so we must check all of them (in this order)
 * to ensure that we're running on MAC
 * and not some other Apple platform */
#if TARGET_IPHONE_SIMULATOR
#define TB_PLATFORM_IOS
#error "IOS simulator is not supported!"
#elif TARGET_OS_IPHONE
#define TB_PLATFORM_IOS
#error "IOS is not supported!"
#elif TARGET_OS_MAC
#define TB_PLATFORM_MACOS
#else
#error "Unknown Apple platform!"
#endif
/* We also have to check __ANDROID__ before __linux__
 * since android is based on the linux kernel
 * it has __linux__ defined */
#elif defined(__ANDROID__)
#define TB_PLATFORM_ANDROID
#error "Android is not supported!"
#elif defined(__linux__)
#define TB_PLATFORM_LINUX
#elif defined(__EMSCRIPTEN__)
#define TB_PLATFORM_WEB
#else
/* Unknown compiler/platform */
#error "Unknown platform!"
#endif // End of platform detection
