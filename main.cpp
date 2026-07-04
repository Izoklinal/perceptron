#include "raylib.h"
#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdint>
#include "readfile.h"
#include "perceptron.h"

const int weight = 800;
const int height = 600;

const float padding = 25;
float drawingPxs = height - padding * 3;
float cellSize = 0;

Camera2D cam = {0};
int idx = 0;
int predict = 0;
uint8_t target = 7;
bool drawingMode = false;
Picture drawingPicture = {0};
Pictures data = {0};


void input(Perceptron& perceptron);
void render(const Picture& pictureToRender);

int main() {
    std::cout << "Reading data..." << std::endl;
    auto result = readDataFile();

    if (!result.has_value()) {
        return -1;
    }

    data = result.value();

    cellSize = drawingPxs / data.header.pic_size_x;

    std::cout << "Training..." << std::endl;
    Perceptron perceptron = Perceptron(0.001, target);
    perceptron.train(500, data);
    std::cout << "Training ended!" << std::endl;
    
    InitWindow(weight, height, "Neural network");
    SetTargetFPS(60);
    cam.zoom = 1.0f;

    
    while(!WindowShouldClose()) {
        
        if (drawingMode) {
            render(drawingPicture);
        } else {
            render(data.pictures[idx]);
        }
        input(perceptron);
    }

    CloseWindow();
    
    return 0;
}

bool checkMouseBorders(Vector2 mouse_pos) {
    if (mouse_pos.x < padding 
        || mouse_pos.x > height - padding * 2 
        || mouse_pos.y < padding 
        || mouse_pos.y > height - padding * 2) {
        return false;
    }
    return true;
}

void input(Perceptron& perceptron) {
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
    
    if (IsKeyPressed(KEY_TAB)) {
        drawingMode = !drawingMode;
        predict = perceptron.predict(data.pictures[idx]);
    }

    if (IsKeyPressed(KEY_C)) {
        for (size_t y = 0; y < data.header.pic_size_y; y++)
        {
            for (size_t x = 0; x < data.header.pic_size_x; x++)
            {
                drawingPicture.pixels[y][x] = 0;
            }
        }
        predict = perceptron.predict(drawingPicture);
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse_pos = GetMousePosition();

        if (checkMouseBorders(mouse_pos)) {
            size_t x = (mouse_pos.x - padding) / cellSize;
            size_t y = (mouse_pos.y - padding) / cellSize;

            drawingPicture.pixels[y][x] = 255;

            if (y - 1 >= 0) {
                if (drawingPicture.pixels[y - 1][x] == 0) {
                    drawingPicture.pixels[y - 1][x] = 100;
                }
            }
            if (y + 1 < cellSize) {
                if (drawingPicture.pixels[y + 1][x] == 0) {
                    drawingPicture.pixels[y + 1][x] = 100;
                }
            }

            if (x - 1 >= 0) {
                if (drawingPicture.pixels[y][x - 1] == 0) {
                    drawingPicture.pixels[y][x - 1] = 100;
                }
            }
            if (x + 1 < cellSize) {
                if (drawingPicture.pixels[y][x + 1] == 0) {
                    drawingPicture.pixels[y][x + 1] = 100;
                }
            }
        }

        predict = perceptron.predict(drawingPicture);
    }
}

void render(const Picture& pictureToRender) {
    ClearBackground(BLACK);
    BeginDrawing();
    BeginMode2D(cam);
    {
        // 1. Rendering picture area
        DrawRectangleLines(padding, padding, height - padding*2, height - padding*2, WHITE);

        // 2. Rendering idx
        std::string idx_text = "idx:" + std::to_string(idx + 1);
        DrawText(idx_text.c_str(), height, padding, 30, WHITE);

        // 3. Rendering labels
        Color lcol = WHITE;
        if (target == data.labels[idx]) {
            lcol = GREEN;
        }
        std::string labels_text = "num:" + std::to_string(data.labels[idx]);
        DrawText(labels_text.c_str(), height, padding + 30, 30, lcol);

        // 4. Rendering predict
        Color pcol = WHITE;
        if (target == data.labels[idx] && predict == 1) {
            pcol = GREEN;
        } else if ((target == data.labels[idx] && predict == 0) || (target != data.labels[idx] && predict == 1)) {
            pcol = RED;
        }
        std::string predict_text = "predict:" + std::to_string(predict);
        DrawText(predict_text.c_str(), height, padding + 30*2, 30, pcol);

        // 5. Rendering mode
        if (drawingMode) {
            DrawText("Draw: on", height, padding + 30*3, 30, WHITE);
        } else {
            DrawText("Draw: off", height, padding + 30*3, 30, WHITE);
        }


        // 6. Rendering picture
        for (size_t y = 0; y < data.header.pic_size_y; y++)
        {
            for (size_t x = 0; x < data.header.pic_size_y; x++)
            {
                Color c = Color{
                    .r = 255,
                    .g = 255,
                    .b = 255,
                    .a = pictureToRender.pixels[y][x]
                };
                DrawRectangle(padding + cellSize * x, padding + cellSize * y, cellSize, cellSize, c);
            }
            
        }
        
    }
    EndMode2D();
    EndDrawing();
}