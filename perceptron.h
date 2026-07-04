#pragma once
#include <vector>
#include "readfile.h"
#include <iostream>
#include <random>
#include <algorithm>

class Perceptron {
private:
    const size_t picture_size = 28;

    std::vector<float> weights;
    float step;
    float bias;
    uint8_t target;

    void init_weights() {
        weights.resize(picture_size*picture_size);
        for(auto& w : weights) {
            w = (rand() % 1000) / 1000.0f - 0.5f;
        }
    }
    float normalize_input(uint8_t input) {
        return static_cast<float>(input) / 255.0f;
    }
public:
    Perceptron(float p_step, uint8_t p_target) {
        step = p_step;
        bias = 0.0f;
        target = p_target;
        init_weights();
    }

    int predict(const Picture& picture) {
        float result = do_step(picture);
        int activation = activate(result);
        return activation;
    }

    void train(int epochs, const Pictures& dataset) {
        for (size_t i = 0; i < epochs; i++)
        {
            std::vector<size_t> indices(dataset.header.pics_count);
            for (size_t i = 0; i < indices.size(); i++)
            {
                indices[i] = i;
            }
            
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(indices.begin(), indices.end(), g);

            for (size_t idx : indices) {
                train_step(dataset.pictures[idx], dataset.labels[idx]);
            }
            std::cout << "Epoch #" << i + 1 << " / " << epochs << " trained." << std::endl;
        }
    }

    void train_step(const Picture& picture, uint8_t label) {
        float result = do_step(picture);
        int activation = activate(result);

        int expected = 0;
        if (label == target) {
            expected = 1;
        }
        process_error(picture, activation, expected);
    }

    float do_step(const Picture& picture) {
        float result = 0;
        for (size_t i = 0; i < picture_size*picture_size; i++)
        {
            result += normalize_input(picture.pixels[i / picture_size][i % picture_size]) * weights[i];
        }
        result += bias;
        return result;
    }

    int activate(float result) {
        if (result >= 0) {
            return 1;
        } else {
            return 0;
        }
    }

    void process_error(const Picture& picture, int activation, int expected) {
        int error = expected - activation;
        for (size_t i = 0; i < picture_size*picture_size; i++)
        {
            weights[i] = weights[i] + step * error * normalize_input(picture.pixels[i / picture_size][i % picture_size]);
        }
        bias = bias + step * error;
    }
};