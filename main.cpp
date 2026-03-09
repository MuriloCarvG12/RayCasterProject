

#include <iostream>

#include "Vector3D.h"
#include "Intersections.h"
#include <cmath>
#include <cstdint>
#include <fstream>
#include <vector>
#include <windows.h>
#include <chrono>
#define KEY_DOWN(k) (GetAsyncKeyState(k))

static auto G_LastFrame = std::chrono::high_resolution_clock::now();

HWND g_window_handle = 0;
MONITORINFO G_MonitorInfo;
LONG G_MonitorWidth = 0;
LONG G_MonitorHeight = 0;

typedef struct SCENE_BIT_MAP
{
    BITMAPINFO BitMapInfo;
    void* memory_canvas;
} SCENE_BIT_MAP;

SCENE_BIT_MAP g_backbuffer;

struct Pixel {
    uint8_t Blue;
    uint8_t Green;
    uint8_t Red;
    uint8_t Alpha;

    Pixel(double R, double G, double B)
        : Blue((uint8_t)(std::min(B, 1.0) * 255)),
          Green((uint8_t)(std::min(G, 1.0) * 255)),
          Red((uint8_t)(std::min(R, 1.0) * 255)),
          Alpha(255) {}
};

struct Sphere {
    Vector_3D Center;
    double Radius;
    Vector_3D Material;
};

std::vector<Sphere> Spheres = {
    { Vector_3D(1, 0, -5),  1.0, Vector_3D(1, 1, 0) },  // yellow sphere
    { Vector_3D(0, 2, -6),  2.0, Vector_3D(1, 0, 0) },  // red sphere
    { Vector_3D(2, 0, -4),  0.5, Vector_3D(1, 0, 1) },  // purple sphere
{ Vector_3D(4, 0, -4),  0.5, Vector_3D(1, 0.5, 0) },  // purple sphere
};

int height = 400;
int width = 600;

//vector stuff - here we set up the vector directions up foward right which will be used to calculate other vectors in our main function
//we also determine the camera position adopting the standard origin XYZ 000

Vector_3D WorldUp = Vector_3D(0, 1, 0);

static double G_Yaw   = -90.0; // -90 faces forward (down -Z)
static double G_Pitch =   0.0; // 0 is flat/horizontal
Vector_3D G_LookAt =  Vector_3D(0, 0, 0);
Vector_3D G_CameraPosition = Vector_3D(0, 0, -0.5);
Vector_3D G_Forward = (G_LookAt - G_CameraPosition).normalized();;
Vector_3D G_Right = WorldUp.CrossProduct(G_Forward).normalized();
Vector_3D G_Up = G_Right.CrossProduct(G_Forward).normalized();

//camera stuff here we define the aspect ratio, as well as the FOV
//reminder that it will be the FOV that will be usd to define how large or thin or image plane will be
//also convert it to radians

double G_AspectRatio = width / double(height);
int G_FOV = 60;

double  G_FovRad = (G_FOV * M_PI )/ 180;
double G_CameraDistance = 1.0;

//imageplane stuff setting up the width height and locating its center , its important to remember
//that we are assuming the image plane is exactly in front of our camera thus the center is just the camera pos vec
// + the foward * 1

double G_ImagePlaneHeight = 2.0 * G_CameraDistance * std::tan(G_FovRad * 0.5);
double G_ImagePlaneWidth  = G_ImagePlaneHeight * G_AspectRatio;

// initializing the image buffer setting all pixels to black
std::vector<Pixel> G_ImageBuffer(height * width, Pixel(0, 0, 0));

Vector_3D G_LightDirection = Vector_3D(1,1,-1).normalized();

int render() {
    LARGE_INTEGER frequency;
    LARGE_INTEGER frameStart, frameEnd;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&frameStart);
    G_Forward = (G_LookAt - G_CameraPosition).normalized();;
    G_Right = WorldUp.CrossProduct(G_Forward).normalized();
    G_Up = G_Right.CrossProduct(G_Forward).normalized();

    HDC DeviceContext = GetDC(g_window_handle);
    Vector_3D PlaneCenter = G_CameraPosition + G_Forward.ScalarMultiplication(G_CameraDistance);
    Vector_3D BottomLeft = PlaneCenter - G_Right.ScalarMultiplication((G_ImagePlaneWidth * 0.5)) - G_Up.ScalarMultiplication((G_ImagePlaneHeight * 0.5));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double CurrentUp = (y + 0.5) / double(height);
            double CurrentRight = (x + 0.5) / double(width);

            Vector_3D CurrentImagePlanePositon = BottomLeft + G_Up.ScalarMultiplication((CurrentUp * G_ImagePlaneHeight)) +
                                                              G_Right.ScalarMultiplication((CurrentRight * G_ImagePlaneWidth));

            Vector_3D Direction = (CurrentImagePlanePositon - G_CameraPosition).normalized();
            Ray3D CameraRay = Ray3D(G_CameraPosition,Direction);

            double ClosestT = -1;
            Sphere* HitSphere = nullptr;

            for (Sphere& S : Spheres) {
                double T = SphericalIntersection(CameraRay, S.Center, S.Radius);
                if (T > 0 && (ClosestT < 0 || T < ClosestT)) {
                    ClosestT = T;
                    HitSphere = &S;
                }
            }

            if (HitSphere != nullptr)
            {
                Vector_3D pixel = G_CameraPosition + Direction.ScalarMultiplication(ClosestT);
                Vector_3D Normal = (pixel - HitSphere->Center).normalized();

                double ambient = 0.05;
                double LightingIntensity = std::max(0.0, Normal.DotProduct(G_LightDirection));

                Pixel Color(
                    HitSphere->Material.x * (LightingIntensity + ambient),
                    HitSphere->Material.y * (LightingIntensity + ambient),
                    HitSphere->Material.z * (LightingIntensity + ambient)
                );
                memcpy_s((Pixel*)g_backbuffer.memory_canvas + (width * y) + x, sizeof(Color), &Color, sizeof(Pixel));
                //g_backbuffer.memory_canvas[(width * y) + x] = Color;
            }
            else {
                //return sky color
                Pixel Color(0x9c / 255.0, 0xc1 / 255.0, 0xfc / 255.0);
                memcpy_s((Pixel*)g_backbuffer.memory_canvas + (width * y) + x, sizeof(Color), &Color, sizeof(Pixel));
                //g_backbuffer.memory_canvas[(width * y) + x] = Color;
            }

        }
    }
    QueryPerformanceCounter(&frameEnd);
    StretchDIBits(DeviceContext, 0, 0, G_MonitorWidth, G_MonitorHeight, 0, 0, width, height, g_backbuffer.memory_canvas, &g_backbuffer.BitMapInfo, DIB_RGB_COLORS, SRCCOPY);

    double elapsedTime = static_cast<double>(frameEnd.QuadPart - frameStart.QuadPart) / frequency.QuadPart;
    char c_DebugBuffer[64] = {0};

    sprintf_s(c_DebugBuffer, sizeof(c_DebugBuffer), "Elapsed Time %0.02f", elapsedTime);
    //this TextOut function prints stuff onto our program!
    TextOutA(DeviceContext, 0,0, c_DebugBuffer, (int) strlen(c_DebugBuffer));

    return 0;
};

void processInput(void) {
    G_Forward = (G_LookAt - G_CameraPosition).normalized();
    G_Right   = WorldUp.CrossProduct(G_Forward).normalized();
    G_Up      = G_Right.CrossProduct(G_Forward).normalized();

    // Delta time
    static auto LastFrame = std::chrono::high_resolution_clock::now();
    auto Now = std::chrono::high_resolution_clock::now();
    double DeltaTime = std::chrono::duration<double>(Now - LastFrame).count();
    LastFrame = Now;

    double CameraSpeed = 1.0 * DeltaTime;

    bool EscKeyIsDown     = KEY_DOWN(VK_ESCAPE);
    bool RiseKeyIsDown    = KEY_DOWN(VK_SPACE);
    bool LowerKeyIsDown   = KEY_DOWN(VK_LSHIFT);
    bool UpKeyIsDown      = KEY_DOWN('W');
    bool LeftKeyIsDown    = KEY_DOWN('A');
    bool RightKeyIsDown   = KEY_DOWN('D');
    bool DownKeyIsDown    = KEY_DOWN('S');
    bool UpArrowIsDown    = KEY_DOWN(VK_UP);
    bool LeftArrowIsDown  = KEY_DOWN(VK_LEFT);
    bool RightArrowIsDown = KEY_DOWN(VK_RIGHT);
    bool DownArrowIsDown  = KEY_DOWN(VK_DOWN);
    bool ResetKeyIsDown   = KEY_DOWN('R');

    if (EscKeyIsDown)
        SendMessageA(g_window_handle, WM_CLOSE, 0, 0);

    if (UpKeyIsDown)    G_CameraPosition = G_CameraPosition + G_Forward.ScalarMultiplication(CameraSpeed);
    if (DownKeyIsDown)  G_CameraPosition = G_CameraPosition - G_Forward.ScalarMultiplication(CameraSpeed);
    if (RightKeyIsDown) G_CameraPosition = G_CameraPosition + G_Right.ScalarMultiplication(CameraSpeed);
    if (LeftKeyIsDown)  G_CameraPosition = G_CameraPosition - G_Right.ScalarMultiplication(CameraSpeed);
    if (RiseKeyIsDown)  G_CameraPosition = G_CameraPosition + G_Up.ScalarMultiplication(CameraSpeed);
    if (LowerKeyIsDown) G_CameraPosition = G_CameraPosition - G_Up.ScalarMultiplication(CameraSpeed);
    if (ResetKeyIsDown)  G_CameraPosition.y = 0, G_CameraPosition.x = 0, G_CameraPosition.z = 0, G_Yaw = -90, G_Pitch = 0;

    double RotateSpeed = 60.0 * DeltaTime; // degrees per second

    if (UpArrowIsDown)    G_Pitch += RotateSpeed;
    if (DownArrowIsDown)  G_Pitch -= RotateSpeed;
    if (LeftArrowIsDown)  G_Yaw   -= RotateSpeed;
    if (RightArrowIsDown) G_Yaw   += RotateSpeed;

    // Clamp pitch so you can't flip upside down
    if (G_Pitch >  89.0) G_Pitch =  89.0;
    if (G_Pitch < -89.0) G_Pitch = -89.0;

    // Derive G_LookAt direction from angles
    double YawRad   = G_Yaw   * M_PI / 180.0;
    double PitchRad = G_Pitch * M_PI / 180.0;

    G_LookAt.x = G_CameraPosition.x + cos(PitchRad) * cos(YawRad);
    G_LookAt.y = G_CameraPosition.y + sin(PitchRad);
    G_LookAt.z = G_CameraPosition.z + cos(PitchRad) * sin(YawRad);
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    const char CLASS_NAME[] = "RendererWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc   = WindowProc;       // Window procedure callback
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    G_MonitorInfo.cbSize = sizeof(MONITORINFO);

    if (GetMonitorInfoA(MonitorFromWindow(g_window_handle, MONITOR_DEFAULTTOPRIMARY), &G_MonitorInfo) == 0)// this takes 2 input parameters a monitor handle a and monitor data structure our monitor handle is a function call that returns a handle for our monitor
    {
        return -1;
    };

    G_MonitorWidth = G_MonitorInfo.rcMonitor.right - G_MonitorInfo.rcMonitor.left;
    G_MonitorHeight = G_MonitorInfo.rcMonitor.bottom - G_MonitorInfo.rcMonitor.top;

    // here we are registering our bitmap info
    g_backbuffer.BitMapInfo.bmiHeader.biWidth = width;
    // make sure we are supplying a negative height for windows to make use of our buffer top -> down supplying it with a positive inverts this direction and for some reason pixels are not drawn
    g_backbuffer.BitMapInfo.bmiHeader.biHeight = -height;
    g_backbuffer.BitMapInfo.bmiHeader.biSize = sizeof(g_backbuffer.BitMapInfo.bmiHeader);
    g_backbuffer.BitMapInfo.bmiHeader.biCompression = BI_RGB;
    g_backbuffer.BitMapInfo.bmiHeader.biBitCount = 32;
    g_backbuffer.BitMapInfo.bmiHeader.biPlanes = 1;

    g_backbuffer.memory_canvas = VirtualAlloc(NULL, width * height * sizeof(Pixel), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    RegisterClass(&wc);

    // 2. Create the window
     g_window_handle = CreateWindowEx(
        0,                          // Optional window styles
        CLASS_NAME,                 // Window class name
        "Raytracer Window",          // Window title
        WS_OVERLAPPEDWINDOW,        // Window style

        // Position & size (x, y, width, height)
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,

        NULL,       // Parent window
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (g_window_handle == NULL) return 0;

    ShowWindow(g_window_handle, nCmdShow);
    UpdateWindow(g_window_handle);

    MSG msg = {};
    bool running = true;

    while (running)
    {
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                running = false;
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!running) break;

        processInput();
        render();
    }

    return (int)msg.wParam;
}

// 4. Window Procedure — handles messages sent to the window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:

            return 0;


        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

