#include <vector>
#include <string>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <torch/torch.h>


struct MNISTData {
    std::vector<float> images;
    std::vector<int64_t> labels;
    int num_images;
    int rows;
    int cols;
};

struct MNISTDataset {
    torch::Tensor train_images;
    torch::Tensor train_labels;
    torch::Tensor test_images;
    torch::Tensor test_labels;
};


int reverseInt(int i) {
    unsigned char c1, c2, c3, c4;
    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;
    return ((int) c1 << 24) + ((int) c2 << 16) + ((int) c3 << 8) + c4;
}

MNISTData read_mnist(const std::string &image_path, const std::string &label_path) {
    MNISTData dataset;

    std::ifstream image_file(image_path, std::ios::binary);
    if (!image_file.is_open()) throw std::runtime_error("Failed to open " + image_path);

    int magic_number = 0, num_images = 0, rows = 0, cols = 0;
    image_file.read((char *) &magic_number, sizeof(magic_number));
    image_file.read((char *) &num_images, sizeof(num_images));
    image_file.read((char *) &rows, sizeof(rows));
    image_file.read((char *) &cols, sizeof(cols));

    magic_number = reverseInt(magic_number);
    num_images = reverseInt(num_images);
    rows = reverseInt(rows);
    cols = reverseInt(cols);

    dataset.images.resize(num_images * rows * cols);

    for (int i = 0; i < num_images; ++i) {
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                unsigned char temp = 0;
                image_file.read((char *) &temp, sizeof(temp));
                dataset.images[i * rows * cols + r * cols + c] =
                        static_cast<float>(temp) / 255.0f;
            }
        }
    }

    std::ifstream label_file(label_path, std::ios::binary);
    if (!label_file.is_open()) throw std::runtime_error("Failed to open " + label_path);

    int magic_label = 0, num_labels = 0;
    label_file.read((char *) &magic_label, sizeof(magic_label));
    label_file.read((char *) &num_labels, sizeof(num_labels));
    magic_label = reverseInt(magic_label);
    num_labels = reverseInt(num_labels);

    if (num_images != num_labels)
        throw std::runtime_error("Number of images and labels do not match!");

    dataset.labels.resize(num_labels);

    for (int i = 0; i < num_labels; ++i) {
        unsigned char temp = 0;
        label_file.read((char *) &temp, sizeof(temp));
        dataset.labels[i] = static_cast<int>(temp);
    }

    dataset.num_images = num_images;
    dataset.rows = rows;
    dataset.cols = cols;
    return dataset;
}


std::pair<torch::Tensor, torch::Tensor> to_torch(const MNISTData &dataset) {
    torch::Tensor images = torch::from_blob(
            (void *) dataset.images.data(),
            {dataset.num_images, 1, dataset.rows, dataset.cols},
            torch::kFloat32
    ).clone();

    torch::Tensor labels = torch::from_blob(
            (void *) dataset.labels.data(),
            {dataset.num_images},
            torch::kInt64
    ).clone();

    return {images, labels};
}

void printByIndex(torch::Tensor images,torch::Tensor labels, int idx){
    std::cout << "Label: " << labels[idx].item<int>() << std::endl;
    auto img = images[idx][0];
    for (int r = 0; r < img.size(0); r++) {
        for (int c = 0; c < img.size(1); c++) {
            float pixel = img[r][c].item<float>();
            std::cout << (pixel > 0.5 ? "#" : ".");
        }
        std::cout << std::endl;
    }
}

MNISTDataset load_dataset() {
    //TODO assert!
    MNISTData train_data = read_mnist("data/train-images", "data/train-labels");
    MNISTData test_data  = read_mnist("data/test-images", "data/test-labels");

    auto [train_images, train_labels] = to_torch(train_data);
    auto [test_images, test_labels]   = to_torch(test_data);

    return {train_images, train_labels, test_images, test_labels};
}