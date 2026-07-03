#include "readfile.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>

uint32_t uint32_reverse_bytes(uint32_t value) {
    return ((value >> 24) & 0x000000FF)
         | ((value >> 8)  & 0x0000FF00)
         | ((value << 8)  & 0x00FF0000)
         | ((value << 24) & 0xFF000000);
}

std::optional<Pictures> readDataFile() {
    std::ifstream images_file("src/train-images.idx3-ubyte", std::ios::binary);

    if (!images_file.is_open()) {
        std::cout << "Could not open the file" << std::endl;
        return std::nullopt;
    }

    PictureHeader header;
    if (!images_file.read(reinterpret_cast<char*>(&header), sizeof(header))) {
        return std::nullopt;   
    }

    header.magic_num = uint32_reverse_bytes(header.magic_num);
    header.pics_count = uint32_reverse_bytes(header.pics_count);
    header.pic_size_x = uint32_reverse_bytes(header.pic_size_x);
    header.pic_size_y = uint32_reverse_bytes(header.pic_size_y);

    std::vector<Picture> pictures;
    pictures.resize(header.pics_count);

    for (size_t i = 0; i < header.pics_count; i++)
    {
        if (!images_file.read(reinterpret_cast<char*>(pictures[i].pixels), sizeof(pictures[i].pixels))) {
            std::cout << "Some sort of a bullshit!!" << std::endl;
            return std::nullopt;
        }
    }

    images_file.close();

    std::ifstream labels_file("src/train-labels.idx1-ubyte", std::ios::binary);

    if (!labels_file.is_open()) {
        std::cout << "Fuck you motherflippa" << std::endl;
        return std::nullopt;
    }

    int header_plug;
    labels_file.read(reinterpret_cast<char*>(&header_plug), sizeof(header_plug));
    labels_file.read(reinterpret_cast<char*>(&header_plug), sizeof(header_plug));

    std::vector<uint8_t> labels;
    labels.resize(header.pics_count);

    for (size_t i = 0; i < header.pics_count; i++)
    {
        if (!labels_file.read(reinterpret_cast<char*>(&labels[i]), sizeof(labels[i]))) {
            std::cout << "Some sort of a bullshit!! AGAIN" << std::endl;
            return std::nullopt;
        }
    }

    labels_file.close();

    return Pictures(header, std::move(labels), std::move(pictures));
}