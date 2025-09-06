#include <iostream>
#include <torch/torch.h>

#include "load.h"

struct CNN : torch::nn::Module {
    CNN() {
        conv1 = register_module("conv1", torch::nn::Conv2d(
                torch::nn::Conv2dOptions(1, 32, 3).padding(1)));
        conv2 = register_module("conv2", torch::nn::Conv2d(
                torch::nn::Conv2dOptions(32, 64, 3).padding(1)));
        pool = register_module("pool", torch::nn::MaxPool2d(
                torch::nn::MaxPool2dOptions(2).stride(2)));
        fc1 = register_module("fc1", torch::nn::Linear(64 * 14 * 14, 128));
        fc2 = register_module("fc2", torch::nn::Linear(128, 10));
        dropout = register_module("dropout", torch::nn::Dropout(0.5));
    }

    torch::Tensor forward(torch::Tensor x) {
        x = torch::relu(conv1->forward(x));
        x = pool->forward(torch::relu(conv2->forward(x)));
        x = x.view({x.size(0), -1});
        x = torch::relu(fc1->forward(x));
        x = dropout->forward(x);
        x = fc2->forward(x);
        return x;
    }

    torch::nn::Conv2d conv1{nullptr}, conv2{nullptr};
    torch::nn::MaxPool2d pool{nullptr};
    torch::nn::Linear fc1{nullptr}, fc2{nullptr};
    torch::nn::Dropout dropout{nullptr};
};

bool train() {
    // TODO
    return true;
}