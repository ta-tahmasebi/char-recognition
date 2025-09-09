#ifndef LOAD_H
#define LOAD_H

#include <vector>
#include <string>
#include <cstdint>
#include <torch/torch.h>

struct MNISTData {
    std::vector<float> images;
    std::vector<int64_t> labels;
    int num_images;
    int rows;
    int cols;

    MNISTData();
};

struct MNISTDataset {
    torch::Tensor train_images;
    torch::Tensor train_labels;
    torch::Tensor test_images;
    torch::Tensor test_labels;
};

MNISTDataset load_dataset();

#endif