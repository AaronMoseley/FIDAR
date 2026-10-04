#include "DroneControllerWidget.h"

DroneControllerWidget::DroneControllerWidget(const glm::vec3& position, float rotation, const std::string& ipAddress) : QDialog(nullptr)
{
    setWindowTitle("Drone Controller");
    setWindowFlags(Qt::Dialog);

    m_droneInterface = std::make_shared<TelloDroneInterface>(ipAddress);
    m_droneInterface->SetDronePosition(position);
    m_droneInterface->SetDroneRotationDegrees(rotation);

    SetupUI();

    m_connectionSuccessful = m_droneInterface->TakeoffDrone();
    if(m_connectionSuccessful == false)
    {
        QMessageBox::critical(
            this,
            "Error",
            "Could not connect to Tello drone. Please try reconnecting."
        );
    } else 
    {
        m_updateImageThread = std::thread(&DroneControllerWidget::UpdateUIImageThread, this);
        m_updateImageThread.detach();
    }
}

void DroneControllerWidget::MoveDrone(TelloDroneInterface::MovementType movementType)
{
    if(m_droneInterface == nullptr)
    {
        return;
    }

    m_droneInterface->MoveDrone(movementType);
    UpdateTransformLabels();
}

void DroneControllerWidget::keyPressEvent(QKeyEvent* event)
{
    Qt::Key key = static_cast<Qt::Key>(event->key());

    if(kMovementMap.contains(key))
    {
        MoveDrone(kMovementMap.at(key));
        return;
    }

    if(key == Qt::Key::Key_F && m_addCameraCallback)
    {
        std::filesystem::path imagePath = m_droneInterface->GetCurrentDroneImage();
        glm::vec3 position;
        m_droneInterface->GetDronePosition(position);
        glm::vec3 rotation = {kBaseDroneXRotation, m_droneInterface->GetDroneRotation(), kBaseDroneZRotation};
        m_addCameraCallback(imagePath, position, rotation);
    }

    if(key == Qt::Key::Key_Escape)
    {
        LandDrone();
    }
}

void DroneControllerWidget::showEvent(QShowEvent *event)
{
    if(m_connectionSuccessful == false)
    {
        setVisible(false);
        close();
    }
}

void DroneControllerWidget::LandDrone()
{
    setVisible(false);
    close();

    if(m_droneInterface == nullptr)
    {
        return;
    }

    m_droneInterface->LandDrone();
    m_droneInterface = nullptr;
}

QImage DroneControllerWidget::MatToQImage(const cv::Mat& inputImage) 
{
    switch (inputImage.type()) {
    case CV_8UC3:
        return QImage(inputImage.data, inputImage.cols, inputImage.rows, static_cast<qsizetype>(inputImage.step),
                      QImage::Format_BGR888).copy();
    case CV_8UC4:
        return QImage(inputImage.data, inputImage.cols, inputImage.rows, static_cast<qsizetype>(inputImage.step),
                      QImage::Format_ARGB32).copy();
    case CV_8UC1:
        return QImage(inputImage.data, inputImage.cols, inputImage.rows, static_cast<qsizetype>(inputImage.step),
                      QImage::Format_Grayscale8).copy();
    default:
        return {};
    }
}

void DroneControllerWidget::UpdateUIImageThread()
{
    while(m_threadsRunning)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if(m_droneInterface == nullptr)
        {
            m_threadsRunning = false;
            return;
        }

        cv::Mat currentFrame;
        m_droneInterface->GetCurrentDroneImage(currentFrame);

        QImage currentFrameImage = MatToQImage(currentFrame);

        QPixmap pixmap = QPixmap::fromImage(currentFrameImage).scaled(
            kImageSize, Qt::KeepAspectRatio, Qt::FastTransformation
        );
        m_imageLabel->setPixmap(pixmap);
    }
}

void DroneControllerWidget::SetupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout();
    setLayout(mainLayout);

    mainLayout->addWidget(new QLabel(kHelpText.c_str()));

    glm::vec3 currentPosition;
    m_droneInterface->GetDronePosition(currentPosition);

    QHBoxLayout* positionLayout = new QHBoxLayout();
    mainLayout->addLayout(positionLayout);
    positionLayout->addWidget(new QLabel("X:"));
    m_xLabel = new QLabel(QString::number(currentPosition.x));
    positionLayout->addWidget(m_xLabel);
    positionLayout->addWidget(new QLabel("Y:"));
    m_yLabel = new QLabel(QString::number(currentPosition.y));
    positionLayout->addWidget(m_yLabel);
    positionLayout->addWidget(new QLabel("Z:"));
    m_zLabel = new QLabel(QString::number(currentPosition.z));
    positionLayout->addWidget(m_zLabel);

    QHBoxLayout* rotationLayout = new QHBoxLayout();
    mainLayout->addLayout(rotationLayout);
    rotationLayout->addWidget(new QLabel("Rotation:"));
    m_rotationLabel = new QLabel(QString::number(glm::degrees(m_droneInterface->GetDroneRotation())));
    rotationLayout->addWidget(m_rotationLabel);

    QPushButton* landButton = new QPushButton("Land Drone");
    connect(landButton, &QPushButton::pressed, this, &DroneControllerWidget::close);
    mainLayout->addWidget(landButton);

    m_imageLabel = new QLabel();
    mainLayout->addWidget(m_imageLabel);
}

void DroneControllerWidget::UpdateTransformLabels()
{
    if(m_droneInterface == nullptr)
    {
        return;
    }

    glm::vec3 currentPosition;
    m_droneInterface->GetDronePosition(currentPosition);

    m_xLabel->setText(QString::number(currentPosition.x));
    m_yLabel->setText(QString::number(currentPosition.y));
    m_zLabel->setText(QString::number(currentPosition.z));

    m_rotationLabel->setText(QString::number(glm::degrees(m_droneInterface->GetDroneRotation())));
}

void DroneControllerWidget::closeEvent(QCloseEvent *event)
{
    if(m_closedCallback)
    {
        m_closedCallback();
    }

    m_threadsRunning = false;
}