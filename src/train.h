#ifndef TRAIN_H
#define TRAIN_H

#include <string>
#include <vector>
#include <torch/torch.h>
#include <torch/script.h>

struct CNN : torch::nn::Module {
    CNN();

    torch::Tensor forward(torch::Tensor x);

    torch::nn::Conv2d conv1{nullptr}, conv2{nullptr};
    torch::nn::MaxPool2d pool{nullptr};
    torch::nn::Linear fc1{nullptr}, fc2{nullptr};
    torch::nn::Dropout dropout{nullptr};
};

class MyDataset : public torch::data::Dataset<MyDataset> {
private:
    torch::Tensor images_, labels_;

public:
    MyDataset(torch::Tensor images, torch::Tensor labels);

    torch::data::Example<> get(size_t index) override;

    [[nodiscard]] torch::optional<size_t> size() const override;
};

bool train();

bool hasModelFiles(const std::string &folderPath);


class Predictor {
public:
    explicit Predictor(std::string model_path = "./data/model.pt");

    std::vector<float> predict(const std::vector<float> &input);

private:
    void load_model();

    torch::Device device_;
    std::string model_path_;
    std::shared_ptr<CNN> model_;
    bool initialized_;
};

#endif