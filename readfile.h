#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <optional>

struct PictureHeader {
    uint32_t magic_num;
    uint32_t pics_count;
    uint32_t pic_size_x;
    uint32_t pic_size_y;
};

struct Picture {
    uint8_t pixels[28][28];
};

struct Pictures {
    PictureHeader header;
    std::vector<uint8_t> labels;
    std::vector<Picture> pictures;
};

std::optional<Pictures> readDataFile();