#include "raylib.h"
#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdint>
#include "readfile.h"
#include "perceptron.h"

Camera2D cam = {0};

int main() {
    std::cout << "Reading data..." << std::endl;
    auto result = readDataFile();

    if (!result.has_value()) {
        return -1;
    }

    Pictures data = result.value();

    std::cout << "Training..." << std::endl;
    uint8_t target = 3;
    Perceptron perceptron = Perceptron(0.001, target);
    perceptron.train(500, data);
    std::cout << "Training ended!" << std::endl;
    
    InitWindow(800, 600, "Neural network");
    SetTargetFPS(60);
    cam.zoom = 1.0f;

    int idx = 0;
    int predict = 0;
    while(!WindowShouldClose()) {
        if (IsKeyPressed(KEY_RIGHT)) {
            if (++idx >= data.header.pics_count) {
                idx = 0;
            }

            predict = perceptron.predict(data.pictures[idx]);
        }
        if (IsKeyPressed(KEY_LEFT)) {
            if (--idx < 0) {
                idx = data.header.pics_count - 1;
            }

            predict = perceptron.predict(data.pictures[idx]);
        }

        ClearBackground(BLACK);
        BeginDrawing();
        BeginMode2D(cam);
        {
            DrawRectangleLines(100, 100, 280, 280, WHITE);
            std::string idx_text = "idx:" + std::to_string(idx + 1);
            DrawText(idx_text.c_str(), 10, 10, 30, WHITE);
            
            Color lcol = WHITE;
            if (target == data.labels[idx]) {
                lcol = GREEN;
            }
            std::string labels_text = "num:" + std::to_string(data.labels[idx]);
            DrawText(labels_text.c_str(), 10, 40, 30, lcol);

            Color pcol = WHITE;
            if (target == data.labels[idx] && predict == 1) {
                pcol = GREEN;
            } else if (target == data.labels[idx] && predict == 0) {
                pcol = RED;
            }
            std::string predict_text = "predict:" + std::to_string(predict);
            DrawText(predict_text.c_str(), 10, 70, 30, pcol);

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