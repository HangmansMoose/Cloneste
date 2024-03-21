
//NOTE: All of these top sections here will be eventually migrated to their own files

// ############################################################################
//                              Platform Globals
// ############################################################################
static bool running = true;


// ############################################################################
//                              Platform Functions
// ############################################################################

bool platform_create_window(int width, int height, char* title);
void platform_update_window();

// ############################################################################
//                              Windows Platform
// ############################################################################
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// ############################################################################
//                              Windows Globals
// ############################################################################
static HWND window;

// ############################################################################
//                              Platform Implementations
// ############################################################################

// Creating our own callback 
LRESULT CALLBACK windows_window_callback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch(msg)
    {
        case WM_CLOSE:
        {
            running = false;
            break;
        }

        default:
        {
            // Let windows hand the default input for now

            result = DefWindowProcA(window, msg, wParam, lParam);
        }
    }

    return result;
}


bool platform_create_window(int width, int height, char* title)
{
    // The why of all of this can be found on MSDN
    // To open a window in Windows we first need a instance (and a handle to that instance)
    // then we need to create a window class, passing it the handle to that instance

    // The 'A' in GetModuleHandleA stands for ANSI
    HINSTANCE instance = GetModuleHandleA(0);

    WNDCLASSA wc = {};
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(instance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);       // This means we decide the look of the cursor
    wc.lpszClassName = title;                       // This is not the title, just a unique identifier
    wc.lpfnWndProc = windows_window_callback;                // callback for input into the window

    if(!RegisterClassA(&wc))
    {
        return false;
    }
    
    // Now that we have an instance and have registered a windows class with Windows 
    // we need to create the actual window

    // WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
    int dwStyle = WS_OVERLAPPEDWINDOW;

    window = CreateWindowExA(  0, 
                                    title, // references the lpszClassName from the wndclass
                                    title, // The actual title
                                    dwStyle,
                                    100,
                                    100,
                                    width,
                                    height,
                                    NULL,   //parent
                                    NULL,   // menu
                                    instance,
                                    NULL);

    if(window == NULL)
    {
        return false;
    }

    ShowWindow(window, SW_SHOW);

    return true;

}

void platform_update_window()
{
    // So in order to interact with the window we need to handle messages to and from the window
    MSG msg;

    while(PeekMessageA(&msg, window, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

#endif


int main()
{
    platform_create_window(1200, 720, "Cloneste Window");

    while(running)
    {
        // Update all the things
        platform_update_window();
    }

    return 0;
}