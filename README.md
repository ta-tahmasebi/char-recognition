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
  
<p align="center">
  <video src="https://github.com/user-attachments/assets/3400d623-8b07-4285-b1c6-9066d0cb1bd1" width="90%" controls></video>
</p>

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
