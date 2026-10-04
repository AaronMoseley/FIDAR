#ifndef POINTCLOUDAPP_DRONECONTROLLERWIDGET_H
#define POINTCLOUDAPP_DRONECONTROLLERWIDGET_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QKeyEvent>
#include <QMessageBox>
#include <QShowEvent>

#include <thread>
#include <atomic>

#include "Utilities/TelloDroneInterface.h"

class DroneControllerWidget : public QDialog
{
    Q_OBJECT
public:
    inline static const std::map<Qt::Key, TelloDroneInterface::MovementType> kMovementMap = 
    {
        { Qt::Key::Key_W, TelloDroneInterface::MovementType::Forward },
        { Qt::Key::Key_S, TelloDroneInterface::MovementType::Backward },
        { Qt::Key::Key_A, TelloDroneInterface::MovementType::Left },
        { Qt::Key::Key_D, TelloDroneInterface::MovementType::Right },
        { Qt::Key::Key_Shift, TelloDroneInterface::MovementType::Up },
        { Qt::Key::Key_Control, TelloDroneInterface::MovementType::Down },
        { Qt::Key::Key_E, TelloDroneInterface::MovementType::RotateClockwise },
        { Qt::Key::Key_Q, TelloDroneInterface::MovementType::RotateCounterClockwise }
    };

    DroneControllerWidget(const glm::vec3& position, float rotation, const std::string& ipAddress);

    void MoveDrone(TelloDroneInterface::MovementType movementType);

    void LandDrone();

    void SetClosedCallback(std::function<void()> closedCallback) { m_closedCallback = closedCallback; };
    void SetAddCameraCallback(std::function<void(const std::filesystem::path&, const glm::vec3&, const glm::vec3&)> addCameraCallback) 
        { m_addCameraCallback = addCameraCallback; };

    bool IsConnectionSuccessful() { return m_connectionSuccessful; }

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void showEvent(QShowEvent *event) override;

private:
    const std::string kHelpText = "WASD to move horizontally, Shift + Ctrl to move vertically, Q + E to rotate, F to take a picture";
    static constexpr float kBaseDroneXRotation = 0.0f;
    static constexpr float kBaseDroneZRotation = 0.0f;

    void UpdateTransformLabels();
    void SetupUI();
    void UpdateUIImageThread();
    QImage MatToQImage(const cv::Mat& mat);

    const QSize kImageSize = { 300, 200 };

    std::shared_ptr<TelloDroneInterface> m_droneInterface = nullptr;

    std::function<void()> m_closedCallback;
    std::function<void(const std::filesystem::path&, const glm::vec3&, const glm::vec3&)> m_addCameraCallback;

    QLabel* m_xLabel;
    QLabel* m_yLabel;
    QLabel* m_zLabel;
    QLabel* m_rotationLabel;
    QLabel* m_imageLabel;

    bool m_connectionSuccessful = false;

    std::thread m_updateImageThread;
    std::atomic<bool> m_threadsRunning = true;
};

#endif