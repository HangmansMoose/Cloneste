#include "bigwhoop_lib.h"
#include "platform.h"

#define APIENTRY
#define GL_GLEXT_PROTOTYPES
#include "glcorearb.h"

#ifdef _WIN32
#include "win32_platform.cpp"
#endif

#include "gl_renderer.cpp"


int main()
{
    BumpAllocator transientStorage = make_bump_allocator(MB(50));
    platform_create_window(1200, 720, "Cloneste Window");

    gl_init(&transientStorage);

    while(running)
    {
        // Update all the things
        platform_update_window();
    }

    return 0;
}
