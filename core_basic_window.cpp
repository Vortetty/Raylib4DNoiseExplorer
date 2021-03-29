/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h" // Include rendering library
#include "FastNoiseLite.hpp" // FastNoiseLite library for generating noise
#include "colors.h" // Color handler library
#include <random> // Random lib
#include <deque>
#include <limits>
#include <cmath>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_RAYLIB_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_raylib.h"

void add_option_int(nk_context *ctx, char* name, int* outVar, int min, int max, int step){
    nk_layout_row_dynamic(ctx, 10, 1);
    nk_label(ctx, FormatText("%s: %i", name, *outVar), NK_TEXT_CENTERED);
    nk_slider_int(ctx, min, outVar, max, step);
    nk_layout_row_dynamic(ctx, 10, 1);
}

void add_option_float(nk_context *ctx, char* name, float* outVar, float min, float max, float step){
    nk_layout_row_dynamic(ctx, 10, 1);
    nk_label(ctx, FormatText("%s: %0f", name, *outVar), NK_TEXT_CENTERED);
    nk_slider_float(ctx, min, outVar, max, step);
    nk_layout_row_dynamic(ctx, 10, 1);
}

void add_option_onoff(nk_context *ctx, char* name, int* outVar){
    nk_layout_row_dynamic(ctx, 10, 1);
    nk_label(ctx, FormatText("%s: %s", name, *outVar ? "True" : "False"), NK_TEXT_CENTERED);
    nk_slider_int(ctx, 0, outVar, 1, 1);
    nk_layout_row_dynamic(ctx, 10, 1);
}

void add_option_list(nk_context *ctx, char* name, int* outVar, std::deque<char*> names){
    nk_layout_row_dynamic(ctx, 10, 1);
    nk_label(ctx, FormatText("%s: %s", name, names[*outVar]), NK_TEXT_CENTERED);
    nk_slider_int(ctx, 0, outVar, names.size()-1, 1);
    nk_layout_row_dynamic(ctx, 10, 1);
}

void add_option_separator(nk_context *ctx, char* name){
    nk_layout_row_dynamic(ctx, 10, 1);
    nk_layout_row_dynamic(ctx, 10, 1);
    nk_label(ctx, name, NK_TEXT_CENTERED|NK_TEXT_ALIGN_CENTERED);
    nk_layout_row_dynamic(ctx, 10, 1);
}

FastNoiseLite noise;

int wrap(int kX, int const kLowerBound, int const kUpperBound) // Just wraps an integer, nothing big
{
    int range_size = kUpperBound - kLowerBound + 1;

    if (kX < kLowerBound)
        kX += range_size * ((kLowerBound - kX) / range_size + 1);

    return kLowerBound + (kX - kLowerBound) % range_size;
}

float lerp(float start, float dest, float ammnt)
{
    return start + ammnt * (dest - start);
}

int main(int argc, char* argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;
    Vector3 cubeSize = {50, 50, 50};
    float w = 0;
    float camAngle = 0;
    int noiseSampleScale = 10;
    float targetZoom = 1, currentZoom = 50;
    float defaultCamFov = 67.5f;
    int lockToCube = true;
    float averagedNoise;
    bool hasBeenWarned = false;
    bool exitNow = false;

    Vector3 tmpXYZ = {0, 0, 0}, tmpYZW = {0, 0, 0}, tmpZWX = {0, 0, 0}, tmpWXY = {0, 0, 0};

    SetConfigFlags(FLAG_WINDOW_RESIZABLE|FLAG_WINDOW_ALWAYS_RUN);
    InitWindow(screenWidth, screenHeight, "4D Noise Cube");

    Camera3D camera = { 0 };
    camera.position = { cubeSize.x+cubeSize.x/2, cubeSize.y+cubeSize.y/4, cubeSize.z+cubeSize.z/2 };
    camera.target = {cubeSize.x/2, cubeSize.y/2, cubeSize.z/2};
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.type = CAMERA_PERSPECTIVE;

    std::deque<char*> noiseNames = { "Open Simplex 2", "Open Simplex 2S", "Cellular", "Perlin", "Value Cubic", "Value" };
    noise.mNoiseType = (FastNoiseLite::NoiseType)1;

    std::deque<char*> cellularDistanceFuncs = { "Euclidean", "Euclidian Square", "Manhattan", "Hybrid" };
    std::deque<char*> cellularReturnTypes = { "Cell Value", "Distance", "Distance 2", "Distance 2 Add", "Distance 2 Sub", "Distance 2 Mul", "Distance 2 Div" };
    
    std::deque<char*> noiseMods = { "None", "Fractal" };
    int noiseMod = 0;

    std::deque<char*> fractals = { "None", "FBm", "Ridged", "Ping Pong", "Domain Warp Progressive", "Domain Warp Independent" };
    std::deque<char*> warpTypes = { "Open Simplex 2", "Open Simplex 2 Reduced", "Basic Grid" };
    int warpType = 0;

    //--------------------------------------------------------------------------------------

    // Gui
    //--------------------------------------------------------------------------------------
    struct nk_context *ctx;
    ctx = nk_raylib_init();
    //--------------------------------------------------------------------------------------

    // Photoepilepsy warning
    //--------------------------------------------------------------------------------------
    while (!hasBeenWarned && !exitNow){
        BeginDrawing(); // Start drawing
            ClearBackground(RAYWHITE); // Clear previous frame from screen
            DrawText("Photoepilepsy warning:\n  I am not responsible for any harm\n  that comes from usage of this program\n\nPress [ENTER] to accept, [ESC] to exit", 10, 10, 20, BLACK);
            
        EndDrawing();

        if (GetKeyPressed() == KEY_ENTER){
            hasBeenWarned = true;
        }

        if (WindowShouldClose()){
            exitNow = true;
        }
    }
    //--------------------------------------------------------------------------------------
    

    SetTargetFPS(60);

    // Main game loop
    while (!exitNow)    // Detect window close button or ESC key
    {
        // Update Gui
        //----------------------------------------------------------------------------------
        nk_raylib_input(ctx); // Update the nuklear input        
        
        if (nk_begin(ctx, "Config", nk_rect(0, 0, nk_window_is_collapsed(ctx, "Config") ? 28 : 230, GetScreenHeight()), NK_WINDOW_BORDER|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {
            add_option_separator(ctx, "Camera Settings");
            add_option_float(ctx, "Camera Zoom", &targetZoom, 0.4, 2, 0.05);
            add_option_onoff(ctx, "Cube Only", &lockToCube);

            add_option_separator(ctx, "Cube Settings");
            if (lockToCube){
                add_option_float(ctx, "Cube Size", &(cubeSize.x), 1, 250, 1);
                cubeSize.y = cubeSize.x;
                cubeSize.z = cubeSize.x;
            } else {
                add_option_float(ctx, "Cube X Size", &(cubeSize.x), 1, 250, 1);
                add_option_float(ctx, "Cube Y Size", &(cubeSize.y), 1, 250, 1);
                add_option_float(ctx, "Cube Z Size", &(cubeSize.z), 1, 250, 1);
            }

            add_option_separator(ctx, "Noise Settings");
            add_option_int(ctx, "Noise Sample Scale", &noiseSampleScale, 1, 50, 1);

            add_option_int(ctx, "Seed", &(noise.mSeed), 0, std::numeric_limits<int>::max(), 1);

            add_option_list(ctx, "Noise Type", (int*)&(noise.mNoiseType), noiseNames);
            add_option_list(ctx, "Noise Modifier", &noiseMod, noiseMods);

            // Extra Cellular settings
            if (noise.mNoiseType == noise.NoiseType_Cellular){
                add_option_separator(ctx, "Cellular Settings");
                add_option_list(ctx, "Distance Function", (int*)&(noise.mCellularDistanceFunction), cellularDistanceFuncs);
                add_option_list(ctx, "Return Type", (int*)&(noise.mCellularReturnType), cellularReturnTypes);
                add_option_float(ctx, "Jitter Modifier", &(noise.mCellularJitterModifier), 0.1, 5, 0.1);
            }

            // Extra Fractal Settings
            if (noiseMod == 1){
                add_option_separator(ctx, "Fractal Settings");
                add_option_list(ctx, "Fractal Type", (int*)&(noise.mFractalType), fractals);
            }

            // Extra Domain Warp Settings
            if ((int)(noise.mFractalType) > 3 && noiseMod == 1){
                add_option_separator(ctx, "Domain Warp Settings");
                add_option_list(ctx, "Warp Type", &warpType, warpTypes);
                noise.SetDomainWarpType((FastNoiseLite::DomainWarpType)warpType);
                add_option_float(ctx, "Warp Amplifier", &(noise.mDomainWarpAmp), 0.1, 2, 0.1);
            }
        }
        nk_end(ctx);
        //----------------------------------------------------------------------------------


        // Update Camera
        //----------------------------------------------------------------------------------
        currentZoom = lerp(currentZoom, targetZoom, 0.01);

        camAngle += 1.0f/10.0f; // Update camera rotation

        if (camAngle >= 360) camAngle = 0; // If degrees is 360 or greater set degrees to 0

        camera.target = {cubeSize.x/2, cubeSize.y/2, cubeSize.z/2};
        camera.position = { cosf(camAngle*PI/180)*std::min(cubeSize.x+cubeSize.x/2, cubeSize.z+cubeSize.z/2)+camera.target.x, camera.position.y, sinf(camAngle*PI/180)*std::min(cubeSize.x+cubeSize.x/2, cubeSize.z+cubeSize.z/2)+camera.target.z }; // Set camera's position to computed position on circle
        camera.fovy = defaultCamFov/currentZoom;
        //----------------------------------------------------------------------------------


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing(); // Start drawing
            ClearBackground(RAYWHITE); // Clear previous frame from screen

            BeginMode3D(camera); // Set camera to 3d mode to draw cubes

                for (float x = 0; x < cubeSize.x*noiseSampleScale; x += noiseSampleScale){          // Iterate x dimension of the cube
                    for (float y = 0; y < cubeSize.y*noiseSampleScale; y += noiseSampleScale){      // Iterate y dimension of the cube
                        for (float z = 0; z < cubeSize.z*noiseSampleScale; z += noiseSampleScale){  // Iterate z dimension of the cube
                            //std::cout << FormatText("(%f, %f, %f), {0, 0, 0, %f}", x, y, z, 255*(noise.GetNoise(x, y, z)/2+0.5)) << std::endl;
                            //_sleep(1000);

                            if ( (camera.position.x <= 0 && x == 0) || (camera.position.x >= 0 && x == cubeSize.x*noiseSampleScale-noiseSampleScale) || y == cubeSize.y*noiseSampleScale-noiseSampleScale || (camera.position.z <= 0 && z == 0) || (camera.position.z >= 0 && z == cubeSize.z*noiseSampleScale-noiseSampleScale) ){ // Only run if point is on the side or top of the cube
                                if ((int)(noise.mFractalType) > 3 && noiseMod == 1){
                                    tmpXYZ = {x, y, z};
                                    tmpYZW = {y, z, w};
                                    tmpZWX = {z, w, x};
                                    tmpWXY = {w, x, y};

                                    noise.TransformDomainWarpCoordinate(tmpXYZ.x, tmpXYZ.y, tmpXYZ.z); // Get noise on the xyz plane
                                    noise.TransformDomainWarpCoordinate(tmpYZW.x, tmpYZW.y, tmpYZW.z); // Get noise on the yzw plane
                                    noise.TransformDomainWarpCoordinate(tmpZWX.x, tmpZWX.y, tmpZWX.z); // Get noise on the zwx plane
                                    noise.TransformDomainWarpCoordinate(tmpWXY.x, tmpWXY.y, tmpWXY.z);  // Get noise on the wxy plane
                                    
                                    averagedNoise = (
                                        noise.GetNoise(tmpXYZ.x, tmpXYZ.y, tmpXYZ.z) + // Get noise on the xyz plane
                                        noise.GetNoise(tmpYZW.x, tmpYZW.y, tmpYZW.z) + // Get noise on the yzw plane
                                        noise.GetNoise(tmpZWX.x, tmpZWX.y, tmpZWX.z) + // Get noise on the zwx plane
                                        noise.GetNoise(tmpWXY.x, tmpWXY.y, tmpWXY.z)   // Get noise on the wxy plane
                                    )/4; // Average the 4 planes to get a value
                                } else {
                                    averagedNoise = (
                                        noise.GetNoise(x, y, z) + // Get noise on the xyz plane
                                        noise.GetNoise(y, z, w) + // Get noise on the yzw plane
                                        noise.GetNoise(z, w, x) + // Get noise on the zwx plane
                                        noise.GetNoise(w, x, y)   // Get noise on the wxy plane
                                    )/4; // Average the 4 planes to get a value
                                }

                                rgbColor color = hsv2rgb({averagedNoise*180+180, 0.5, 0.5, 0.5}); // Convert value to hsv, then to rgb, with the value as the hue
                                        
                                DrawCube( // Draw cube
                                    {x/noiseSampleScale, y/noiseSampleScale, z/noiseSampleScale}, // Set draw location
                                    1, 1, 1, // Cube is 1x1x1
                                    {(unsigned char)(color.r*255), (unsigned char)(color.g*255), (unsigned char)(color.b*255), 255} // Set color to the computed rgb with no transparency
                                );
                            }
                        }
                    }
                }

                // to do 3d given 2d and an x, y, and z, you would take these planes and combine each point through averaging:
                // x, y
                // y, z
                // z, x

                // so for 4d give z, y, z, and w, you would take these planes  and combine each point through averaging:
                // x, y, z
                // y, z, w
                // z, w, x
                // w, x, y

                //std::cout << "Frame " << w << " rendered (" << cosf(camAngle*PI/180)*15.0f << ", " << sinf(camAngle*PI/180)*15.0f << ")" << std::endl;

                w++; // Increase w dimension by one
            
            EndMode3D(); // Stop 3d mode

            DrawFPS(nk_window_is_collapsed(ctx, "Config") ? 38 : 240, 10); // Draw fps indicator

            nk_raylib_render(ctx); // Draw Nuklear Windows

        EndDrawing(); // Stop drawing and display what was drawn
        //----------------------------------------------------------------------------------

        if (WindowShouldClose()){
            exitNow = true;
        }
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------   
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0; // Return success
}