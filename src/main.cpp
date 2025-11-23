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
    /*
     * Why 50MB? It may seem excessive and arbitrary however it is not. When we come to loading 
     * audio assets, a .wav file of around 2mins is slightly less than 50MB.
     * */
    ArenaAllocator transientStorage = make_bump_allocator(MB(50));
    platform_create_window(2160, 1440, "Cloneste Window");

    gl_init(&transientStorage);

    while(running)
    {
        // Update all the things
        platform_update_window();
    }

    return 0;
}
