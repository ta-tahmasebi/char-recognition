#include "UI.h"
#include <filesystem>
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFrame>
#include <QPainter>
#include <QPen>
#include <QImage>
#include <QFont>
#include <QColor>
#include <QLinearGradient>
#include <QPalette>
#include <QMouseEvent>
#undef slots

#include <vector>
#include "train.h"

class DrawWidget : public QLabel {
Q_OBJECT

public:
    explicit DrawWidget(QWidget *parent = nullptr) : QLabel(parent) {
        setFixedSize(280, 280);
        image = QImage(280, 280, QImage::Format_RGB32);
        image.fill(Qt::white);
        setStyleSheet("background-color: #FFFFFF; border: 2px solid #5D5D5D; border-radius: 10px;");
    }

    void clearCanvas() {
        image.fill(Qt::white);
        update();
    }


    QImage getImage() {
        QImage gray = image.convertToFormat(QImage::Format_Grayscale8);
        QImage scaled = gray.scaled(28, 28, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        scaled.invertPixels();

        return scaled;
    }

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton)
            lastPoint = event->pos();
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (event->buttons() & Qt::LeftButton) {
            QPainter painter(&image);
            QPen pen(QColor("#6636f5"), 20, Qt::SolidLine, Qt::RoundCap);
            painter.setPen(pen);
            painter.drawLine(lastPoint, event->pos());
            lastPoint = event->pos();
            update();
        }
    }

    void paintEvent(QPaintEvent *event) override {
        QPainter canvasPainter(this);
        canvasPainter.setRenderHint(QPainter::Antialiasing);
        canvasPainter.drawImage(0, 0, image);
    }

private:
    QImage image;
    QPoint lastPoint;
};

class App : public QWidget {
Q_OBJECT

public:
    explicit App(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Digit Recognition with Neural Network");
        setFixedSize(1000, 700);

        QPalette pal = palette();
        QLinearGradient gradient(0, 0, 0, 400);
        gradient.setColorAt(0.0, QColor("#0CF5B3"));
        gradient.setColorAt(1.0, QColor("#0F2EF7"));
        pal.setBrush(QPalette::Window, gradient);
        setPalette(pal);
        setAutoFillBackground(true);

        // Main layout
        auto *mainLayout = new QHBoxLayout(this);
        mainLayout->setSpacing(30);
        mainLayout->setContentsMargins(40, 40, 40, 40);

        auto *leftPanel = new QFrame();
        leftPanel->setStyleSheet(
                "background-color: rgba(255,255,255,180); border-radius: 15px; padding: 20px; border: 2px solid rgba(0,0,0,150);");
        auto *leftLayout = new QVBoxLayout(leftPanel);
        leftLayout->setAlignment(Qt::AlignCenter);

        auto *title = new QLabel("Draw a Digit (0-9)");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-family: Arial; font-size: 22px; font-weight: bold; color: #2C3E50; padding: 10px;");

        canvas = new DrawWidget();

        auto *buttonContainer = new QWidget();
        auto *buttonLayout = new QHBoxLayout(buttonContainer);
        buttonLayout->setContentsMargins(0, 0, 0, 0);

        auto *btnPredict = new QPushButton("Predict");
        btnPredict->setStyleSheet(
                "background-color: #27AE60; color: white; border: none; padding: 12px 25px; border-radius: 8px; font-weight: bold; font-size: 16px;");
        connect(btnPredict, &QPushButton::clicked, this, &App::predictDigit);

        auto *btnClear = new QPushButton("Clear");
        btnClear->setStyleSheet(
                "background-color: #E74C3C; color: white; border: none; padding: 12px 25px; border-radius: 8px; font-weight: bold; font-size: 16px;");
        connect(btnClear, &QPushButton::clicked, canvas, &DrawWidget::clearCanvas);

        buttonLayout->addWidget(btnPredict);
        buttonLayout->addWidget(btnClear);

        leftLayout->addWidget(title);
        leftLayout->addWidget(canvas);
        leftLayout->addWidget(buttonContainer);

        auto *rightPanel = new QFrame();
        rightPanel->setStyleSheet(
                "background-color: rgba(255,255,255,200); border-radius: 15px; padding: 20px; border: 2px solid #34495E;");
        auto *rightLayout = new QVBoxLayout(rightPanel);
        rightLayout->setAlignment(Qt::AlignCenter);

        auto *resultTitle = new QLabel("Prediction Result");
        resultTitle->setAlignment(Qt::AlignCenter);
        resultTitle->setStyleSheet(
                "font-family: Arial; font-size: 22px; font-weight: bold; color: #2C3E50; padding: 5px; margin-bottom: 5px;");

        auto *resultFrame = new QFrame();
        resultFrame->setStyleSheet("background-color: #34495E; border-radius: 10px;");
        resultFrame->setFixedHeight(100);
        auto *frameLayout = new QVBoxLayout(resultFrame);

        predictionLabel = new QLabel("—");
        predictionLabel->setAlignment(Qt::AlignCenter);
        predictionLabel->setStyleSheet(
                "color: #ECF0F1; font-family: Arial; font-size: 48px; font-weight: bold; padding: 0px;");

        frameLayout->addWidget(predictionLabel);

        auto *digitOutputsTitle = new QLabel("Model Outputs for Each Digit");
        digitOutputsTitle->setAlignment(Qt::AlignCenter);
        digitOutputsTitle->setStyleSheet(
                "font-family: Arial; font-size: 18px; font-weight: bold; color: #2C3E50; padding: 10px; margin-top: 10px;");

        auto *digitOutputsGrid = new QGridLayout();
        digitOutputsGrid->setSpacing(5);

        for (int i = 0; i < 10; i++) {
            auto *digitLabel = new QLabel(QString("%1: —").arg(i));
            digitLabel->setStyleSheet(
                    "font-family: Arial; font-size: 14px; color: #2C3E50; background-color: rgba(236,240,241,200); border-radius: 5px; padding: 5px;");
            digitLabel->setMinimumSize(80, 40);
            digitLabels.push_back(digitLabel);
            digitOutputsGrid->addWidget(digitLabel, i / 5, i % 5);
        }

        auto *digitOutputsContainer = new QWidget();
        digitOutputsContainer->setLayout(digitOutputsGrid);

        auto *infoText = new QLabel(
                "Instructions:\n"
                "1. Draw a digit (0-9) in the white box\n"
                "2. Click 'Predict' to see the result\n"
                "3. Click 'Clear' to start over"
        );
        infoText->setAlignment(Qt::AlignLeft);
        infoText->setStyleSheet(
                "color: #2C3E50; font-family: Arial; font-size: 14px; padding: 10px; background-color: rgba(236,240,241,150); border-radius: 8px; margin-top: 20px;");

        rightLayout->addWidget(resultTitle);
        rightLayout->addWidget(resultFrame);
        rightLayout->addWidget(digitOutputsTitle);
        rightLayout->addWidget(digitOutputsContainer);
        rightLayout->addSpacing(10);
        rightLayout->addWidget(infoText);

        mainLayout->addWidget(leftPanel, 55);
        mainLayout->addWidget(rightPanel, 45);

        clearPrediction();
    }

private Q_SLOTS:

    void clearPrediction() {
        predictionLabel->setText("—");
        for (int i = 0; i < 10; i++)
            digitLabels[i]->setText(QString("%1: —").arg(i));
    }

    void predictDigit() {
        auto img = canvas->getImage();
        std::vector<float> input;
        input.reserve(28 * 28);

        for (int y = 0; y < img.height(); y++) {
            const uchar *row = img.constScanLine(y);
            for (int x = 0; x < img.width(); x++) {
                uchar pixel = row[x];
                float value = static_cast<float>(pixel) / 255.0f;
                input.push_back(value);
            }
        }

        auto output = predictor.predict(input);
        int predictedDigit = std::max_element(output.begin(), output.end()) - output.begin();
        predictionLabel->setText(QString::number(predictedDigit));
        for (int i = 0; i < 10; i++) {
            digitLabels[i]->setText(QString("%1: %2").arg(i).arg(output[i], 0, 'f', 3));
        }
    }

private:
    Predictor predictor;
    DrawWidget *canvas;
    QLabel *predictionLabel;
    std::vector<QLabel *> digitLabels;
};

int runVisualApp(int argc, char *argv[]) {
    QApplication app(argc, argv);
    App window;
    window.show();
    return app.exec();
}

#include "UI.moc"
