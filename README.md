# Digit Recognition with C++

This project is a **digit recognition application** written in **C++**.  
It allows you to **draw a digit (0–9)** on the canvas, then uses a trained neural network (trained on the **MNIST dataset**) to predict which digit you wrote.

## How to use
Run the program with one of the following options:

```bash
./digit_recognition [option]
````


* **download**:
  Download the MNIST dataset automatically.

![download](images/1.png?raw=true)

* **train**:
  Train the neural network using the downloaded MNIST dataset.

![train](images/2.png?raw=true)


* **ui**:
  Open the graphical interface to draw digits and test the trained model.
  
![ui](images/3.gif?raw=true)


## Third-party Libraries Used
- **[Qt6](https://www.qt.io/)**
- **[LibTorch (PyTorch C++ API)](https://pytorch.org/cppdocs/)**
- **[libcurl](https://curl.se/libcurl/)**
- **[zlib](https://zlib.net/)**  


## 🛠 Build & Run
After downloading and installing the required libraries, you can build and run the project:
```bash
mkdir build
cd build
cmake ..
make clean
make
./DigitRecognizer all
```

If your Torch or Qt installation is in a non-default location, update the `CMAKE_PREFIX_PATH` in your `CMakeLists.txt` file.
