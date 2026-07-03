#include "raylib.h"
#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdint>
#include "readfile.h"

Camera2D cam = {0};

/*

Z = sum(xi * wi) + b;

*/

int main() {
    auto result = readDataFile();

    if (!result.has_value()) {
        return -1;
    }

    Pictures data = result.value();
    
    InitWindow(800, 600, "Neural network");
    SetTargetFPS(60);
    cam.zoom = 1.0f;

    int idx = 0;
    while(!WindowShouldClose()) {
        if (IsKeyPressed(KEY_RIGHT)) {
            if (++idx >= data.header.pics_count) {
                idx = 0;
            }
        }
        if (IsKeyPressed(KEY_LEFT)) {
            if (--idx < 0) {
                idx = data.header.pics_count - 1;
            }
        }

        ClearBackground(BLACK);
        BeginDrawing();
        BeginMode2D(cam);
        {
            DrawRectangleLines(100, 100, 280, 280, WHITE);
            std::string idx_text = "idx:" + std::to_string(idx + 1);
            DrawText(idx_text.c_str(), 10, 10, 30, WHITE);

            std::string labels_text = "num:" + std::to_string(data.labels[idx]);
            DrawText(labels_text.c_str(), 10, 40, 30, WHITE);

            for (size_t y = 0; y < 28; y++)
            {
                for (size_t x = 0; x < 28; x++)
                {
                    Color c = Color{
                        .r = 255,
                        .g = 255,
                        .b = 255,
                        .a = data.pictures[idx].pixels[y][x]
                    };
                    DrawRectangle(100 + x*10, 100 + y*10, 10, 10, c);
                }
            }
            
        }
        EndMode2D();
        EndDrawing();
    }

    CloseWindow();
    
    return 0;
}