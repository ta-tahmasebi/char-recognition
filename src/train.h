#include <iostream>
#include <cinttypes>
#include <torch/torch.h>

#include "load.h"
#include "color.h"

void printAcc(double acc, int64_t correct, int64_t total) {
    char buf[200];
    colorPrint::println("");
    colorPrint::print("Test Accuracy: ");
    std::sprintf(buf, "%02.2f", acc);
    colorPrint::print(buf, colorPrint::Color::GREEN);
    std::sprintf(buf, "%% (%" PRId64 " / %" PRId64 ")", correct, total);
    colorPrint::println(buf);
    colorPrint::println("");
}

void printData(int epoch, int epochs, double running_loss, size_t batch_index, std::chrono::duration<double> elapsed) {
    char buf[200];
    colorPrint::print("Epoch [");
    std::sprintf(buf, "%02d", epoch);
    colorPrint::print(buf, colorPrint::Color::MAGENTA);
    std::sprintf(buf, "/ %d], Loss: ", epochs);
    colorPrint::print(buf);
    std::sprintf(buf, "%02.6f", running_loss / static_cast<double>(std::max<size_t>(1, batch_index)));
    colorPrint::print(buf, colorPrint::Color::YELLOW);
    colorPrint::print(", time: ");
    std::sprintf(buf, "%02.4f", elapsed.count());
    colorPrint::print(buf, colorPrint::Color::BLUE);
    colorPrint::println("s");
}

bool hasDataFiles(const std::string &folderPath) {
    std::filesystem::path path(folderPath);
    return std::filesystem::exists(path) &&
           std::filesystem::exists(path / "test-images") &&
           std::filesystem::exists(path / "test-labels") &&
           std::filesystem::exists(path / "train-images") &&
           std::filesystem::exists(path / "train-labels");
}

bool hasModelFiles(const std::string &folderPath) {
    std::filesystem::path path(folderPath);
    return std::filesystem::exists(path) &&
           std::filesystem::exists(path / "mnist_cnn.pt");
}

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


class MyDataset : public torch::data::Dataset<MyDataset> {
private:
    torch::Tensor images_, labels_;

public:
    MyDataset(torch::Tensor images, torch::Tensor labels)
            : images_(std::move(images)), labels_(std::move(labels)) {}

    torch::data::Example<> get(size_t index) override {
        auto cast_index = static_cast<int64_t>(index);
        return {images_[cast_index], labels_[cast_index]};
    }

    [[nodiscard]] torch::optional<size_t> size() const override {
        return labels_.size(0);
    }
};

bool train() {
    const int epochs = 20;
    const size_t batch_size = 128;

    if (!hasDataFiles("./data")) {
        std::cerr << "Missing some data files." << std::endl;
        std::cerr << "First download them." << std::endl;
        return false;
    }

    auto ds = load_dataset();

    auto train_images = ds.train_images;
    auto train_labels = ds.train_labels;
    auto test_images = ds.test_images;
    auto test_labels = ds.test_labels;

    auto train_dataset = MyDataset(train_images, train_labels)
            .map(torch::data::transforms::Stack<>());

    auto test_dataset = MyDataset(test_images, test_labels)
            .map(torch::data::transforms::Stack<>());

    auto train_loader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(
            std::move(train_dataset),
            torch::data::DataLoaderOptions().batch_size(batch_size).workers(2)
    );

    auto test_loader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
            std::move(test_dataset),
            torch::data::DataLoaderOptions().batch_size(batch_size).workers(2)
    );

    torch::Device device(torch::kCUDA);
    if (!torch::cuda::is_available()) {
        colorPrint::println("CUDA not available, using CPU\n", colorPrint::Color::RED);
        device = torch::Device(torch::kCPU);
    }

    auto model = std::make_shared<CNN>();
    model->to(device);

    torch::optim::Adam optimizer(model->parameters(), torch::optim::AdamOptions(0.001));

    for (int epoch = 1; epoch <= epochs; ++epoch) {
        model->train();
        double running_loss = 0.0;
        size_t batch_index = 0;

        auto t0 = std::chrono::high_resolution_clock::now();
        for (auto &batch: *train_loader) {
            auto images = batch.data.to(device);
            auto labels = batch.target.to(device);

            optimizer.zero_grad();
            auto outputs = model->forward(images);
            auto loss = torch::nn::functional::cross_entropy(outputs, labels);
            loss.backward();
            optimizer.step();

            running_loss += loss.item<double>();
            ++batch_index;
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        printData(epoch, epochs, running_loss, batch_index, t1 - t0);
    }

    model->eval();
    int64_t correct = 0;
    int64_t total = 0;
    torch::NoGradGuard no_grad;
    for (auto &batch: *test_loader) {
        auto images = batch.data.to(device);
        auto labels = batch.target.to(device);

        auto outputs = model->forward(images);
        auto predicted = outputs.argmax(1);
        correct += predicted.eq(labels).sum().item<int64_t>();
        total += labels.size(0);
    }

    double acc = total ? (100.0 * static_cast<double>(correct) / static_cast<double>(total)) : 0.0;
    printAcc(acc, correct, total);

    std::string model_path = "data/mnist_cnn.pt";
    torch::save(model, model_path);
    std::cout << "Model saved to " << model_path << "\n";
    colorPrint::println("Train completed.", colorPrint::Color::GREEN);

    return true;
}


