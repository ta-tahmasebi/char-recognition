#define QT_NO_KEYWORDS
#include <Python.h>
#include <QApplication>
#include <QLabel>
#ifdef slots
#undef slots
#endif

#include <opencv2/opencv.hpp>
#include <torch/torch.h>
#include <asio.hpp>
#include <armadillo>
#include <mlpack/core.hpp>
#include <mlpack/methods/linear_regression/linear_regression.hpp>
#include <Eigen/Dense>
#include <iostream>

QImage cvMatToQImage(const cv::Mat& mat)
{
    cv::Mat rgb;
    cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
    return QImage((const unsigned char*)rgb.data,
                  rgb.cols, rgb.rows,
                  static_cast<int>(rgb.step),
                  QImage::Format_RGB888).copy();
}

int main(int argc, char** argv)
{
    Py_Initialize();
    if (!Py_IsInitialized()) {
        std::cerr << "Failed to initialize Python interpreter\n";
        return -1;
    }

    const char* pyCode = R"(
import sys
print('Hello from embedded Python!')
)";
    int pyResult = PyRun_SimpleString(pyCode);
    if (pyResult != 0) {
        std::cerr << "Error running embedded Python code\n";
    }

    QApplication app(argc, argv);

    cv::Mat img = cv::imread("/home/amirmahdi/Pictures/Screenshots/Screenshot_20250228_183004.png");
    if (img.empty())
    {
        std::cerr << "Failed to load test.jpg\n";
        Py_Finalize();
        return -1;
    }
    QLabel label;
    label.setPixmap(QPixmap::fromImage(cvMatToQImage(img)));
    label.setWindowTitle("OpenCV + Qt6 Demo");
    label.show();

    Eigen::Matrix3f m;
    m << 1,2,3,
    4,5,6,
    7,8,9;
    std::cout << "Eigen matrix m:\n" << m << "\n";

    torch::Device device(torch::kCUDA);
    auto a = torch::rand({3,3}, device);
    auto b = torch::rand({3,3}, device);
    auto c = a + b;
    std::cout << "Torch CUDA tensor c:\n" << c << "\n";

    asio::io_context io;
    asio::steady_timer timer(io, std::chrono::seconds(1));
    timer.async_wait([](auto){ std::cout << "Asio timer fired after 1 second\n"; });
    std::thread asio_thread([&]{ io.run(); });

    arma::mat X = arma::randu<arma::mat>(1, 100);
    arma::rowvec y = 2.5 * X.row(0) + 0.3 + 0.01 * arma::randn<arma::rowvec>(100);

    mlpack::LinearRegression linReg(X, y);
    std::cout << "mlpack LR intercept fit? " << std::boolalpha << linReg.Intercept() << "\n";
    std::cout << "Coefficient: " << linReg.Parameters()( linReg.Intercept() ? 1 : 0 ) << "\n";
    if (linReg.Intercept())
        std::cout << "Intercept value: " << linReg.Parameters()(0) << "\n";

    int ret = app.exec();

    io.stop();
    asio_thread.join();
    Py_Finalize();
    return ret;
}
