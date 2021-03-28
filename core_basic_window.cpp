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
#include <limits>

void add_option_int(nk_context *ctx, char* name, int* outVar, int min, int max, int step){
    nk_layout_row_dynamic(ctx, 10, 1);
    nk_label(ctx, FormatText("%s: %i", name, *outVar), NK_TEXT_CENTERED);
    nk_slider_int(ctx, min, outVar, max, 1);
    nk_layout_row_dynamic(ctx, 10, 1);
}

void add_option_float(nk_context *ctx, char* name, float* outVar, float min, float max, float step){
    nk_layout_row_dynamic(ctx, 10, 1);
    nk_label(ctx, FormatText("%s: %f", name, *outVar), NK_TEXT_CENTERED);
    nk_slider_float(ctx, min, outVar, max, 1);
    nk_layout_row_dynamic(ctx, 10, 1);
}

void add_option_list(nk_context *ctx, char* name, int* outVar, char *names[], int min, int max, int step){
    nk_layout_row_dynamic(ctx, 10, 1);
    nk_label(ctx, FormatText("%s: %s", name, names[*outVar]), NK_TEXT_CENTERED);
    nk_slider_int(ctx, min, outVar, max, 1);
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

int main(int argc, char* argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;
    Vector3 cubeSize = {50, 50, 50};
    float w = 0;
    float camAngle = 0;
    int seed = 1337;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    Camera3D camera = { 0 };
    camera.position = { cubeSize.x+cubeSize.x/2, cubeSize.y+cubeSize.y/4, cubeSize.z+cubeSize.z/2 };
    camera.target = {cubeSize.x/2, cubeSize.y/2, cubeSize.z/2};
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.type = CAMERA_PERSPECTIVE;

    FastNoiseLite::NoiseType noiseTypes[] = { // List of availible noise types.
        noise.NoiseType_OpenSimplex2,
        noise.NoiseType_OpenSimplex2S,
        noise.NoiseType_Cellular,
        noise.NoiseType_Perlin,
        noise.NoiseType_ValueCubic,
        noise.NoiseType_Value
    }; 
    char* noiseNames[] = {
        "Open Simplex 2",
        "Open Simplex 2S",
        "Cellular",
        "Perlin",
        "Value Cubic",
        "Value"
    };
    int noiseType = 1;
    noise.SetNoiseType(noiseTypes[noiseType]);

    //--------------------------------------------------------------------------------------

    // Gui
    //--------------------------------------------------------------------------------------
    struct nk_context *ctx;
    ctx = nk_raylib_init();
    //--------------------------------------------------------------------------------------

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update Gui
        //----------------------------------------------------------------------------------
        nk_raylib_input(ctx); // Update the nuklear input        
        
        if (nk_begin(ctx, "Config", nk_rect(0, 0, nk_window_is_collapsed(ctx, "Config") ? 28 : 230, screenHeight), NK_WINDOW_BORDER|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {
            add_option_float(ctx, "Cube X Size", &(cubeSize.x), 1, 500, 1);
            add_option_float(ctx, "Cube Y Size", &(cubeSize.y), 1, 500, 1);
            add_option_float(ctx, "Cube Z Size", &(cubeSize.z), 1, 500, 1);

            add_option_int(ctx, "Seed", &(noise.mSeed), 0, std::numeric_limits<int>::max(), 1);

            add_option_list(ctx, "Noise Type", &noiseType, noiseNames, 0, sizeof(noiseTypes)/sizeof(noiseTypes[0])-1, 1);
        }
        nk_end(ctx);
        //----------------------------------------------------------------------------------


        // Update Noise from Gui
        //----------------------------------------------------------------------------------
        if (camera.target.x != cubeSize.x/2 || camera.target.y != cubeSize.y/2 || camera.target.z != cubeSize.z/2) {
            camera.position = {cubeSize.x+cubeSize.x/2, cubeSize.y+cubeSize.y/4, cubeSize.z+cubeSize.z/2};
            camera.target = {cubeSize.x/2, cubeSize.y/2, cubeSize.z/2};
        }
        noise.mNoiseType = noiseTypes[noiseType];
        //----------------------------------------------------------------------------------


        // Update Other Vars
        //----------------------------------------------------------------------------------
        camAngle += 1.0f/10.0f; // Update camera rotation

        if (camAngle >= 360) camAngle = 0; // If degrees is 360 or greater set degrees to 0

        camera.position = { cosf(camAngle*PI/180)*(cubeSize.x+cubeSize.x/2)+camera.target.x, camera.position.y, sinf(camAngle*PI/180)*(cubeSize.z+cubeSize.z/2)+camera.target.z }; // Set camera's position to computed position on circle
        //----------------------------------------------------------------------------------


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing(); // Start drawing
            ClearBackground(RAYWHITE); // Clear previous frame from screen

            BeginMode3D(camera); // Set camera to 3d mode to draw cubes

                for (float x = 0; x < cubeSize.x*10; x += 10){          // Iterate x dimension of the cube
                    for (float y = 0; y < cubeSize.y*10; y += 10){      // Iterate y dimension of the cube
                        for (float z = 0; z < cubeSize.z*10; z += 10){  // Iterate z dimension of the cube
                            //std::cout << FormatText("(%f, %f, %f), {0, 0, 0, %f}", x, y, z, 255*(noise.GetNoise(x, y, z)/2+0.5)) << std::endl;
                            //_sleep(1000);

                            if ( (camera.position.x <= 0 && x == 0) || (camera.position.x >= 0 && x == cubeSize.x*10-10) || y == cubeSize.y*10-10 || (camera.position.z <= 0 && z == 0) || (camera.position.z >= 0 && z == cubeSize.z*10-10) ){ // Only run if point is on the side or top of the cube
                                float averagedNoise = (
                                    noise.GetNoise(x, y, z) + // Get noise on the xyz plane
                                    noise.GetNoise(y, z, w) + // Get noise on the yzw plane
                                    noise.GetNoise(z, w, x) + // Get noise on the zwx plane
                                    noise.GetNoise(w, x, y)   // Get noise on the wxy plane
                                )/4; // Average the 4 planes to get a value

                                rgbColor color = hsv2rgb({averagedNoise*180+180, 0.5, 0.5, 0.5}); // Convert value to hsv, then to rgb, with the value as the hue
                                        
                                DrawCube( // Draw cube
                                    {x/10, y/10, z/10}, // Set draw location
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
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------   
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0; // Return success
}