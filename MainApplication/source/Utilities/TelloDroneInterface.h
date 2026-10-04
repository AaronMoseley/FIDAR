#ifndef POINTCLOUDAPP_TELLODRONEINTERFACE_H
#define POINTCLOUDAPP_TELLODRONEINTERFACE_H

#include <map>
#include <string>
#include <memory>
#include "opencv2/opencv.hpp"
#include <filesystem>
#include <atomic>
#include <thread>

#include "Utilities/SocketInterface.h"
#include "glm.hpp"
#include "gtc/quaternion.hpp"

//API: https://dl-cdn.ryzerobotics.com/downloads/tello/0228/Tello+SDK+Readme.pdf

class TelloDroneInterface
{
public:
    enum MovementType
    {
        None,
        Forward,
        Backward,
        Left,
        Right,
        Up,
        Down,
        RotateClockwise,
        RotateCounterClockwise
    };

    TelloDroneInterface(const std::string& ipAddress);

    void SetDronePosition(const glm::vec3& position) { m_currentDronePosition = position; }
    void SetDroneRotationDegrees(float rotationDegrees) { m_currentDroneRotation = glm::radians(rotationDegrees); }

    void GetDronePosition(glm::vec3& outPosition) { outPosition = m_currentDronePosition; }
    float GetDroneRotation() { return m_currentDroneRotation; }

    bool TakeoffDrone();
    bool MoveDrone(MovementType movementType);
    void LandDrone();

    std::filesystem::path GetCurrentDroneImage();
    void GetCurrentDroneImage(cv::Mat& outImage);

private:
    static constexpr uint16_t kTelloPort = 8889;
    static constexpr uint32_t kHorizontalMovementCM = 20;
    static constexpr uint32_t kVerticalMovementCM = 20;
    static constexpr uint32_t kRotationAmountDegrees = 10;
    static constexpr float kRotationAmountRadians = glm::radians(static_cast<float>(kRotationAmountDegrees));
    const std::filesystem::path kDroneCaptureDirectory = "drone_captures";
    const std::string kBaseCaptureFileName = "capture_";

    void FrameLoadThread();

    std::atomic<bool> m_threadsRunning = true;

    inline static const std::map<MovementType, std::string> kMovementTypeToCommand = 
    {
        {Forward, "forward"},
        {Backward, "back"},
        {Left, "left"},
        {Right, "right"},
        {Up, "up"},
        {Down, "down"},
        {RotateClockwise, "cw"},
        {RotateCounterClockwise, "ccw"}
    };

    glm::vec3 m_currentDronePosition = { 0.0f, 0.0f, 0.0f };
    float m_currentDroneRotation = 0.0f;

    std::shared_ptr<SocketInterface> m_socketInterface;

    std::shared_ptr<cv::VideoCapture> m_videoFeed;
    std::mutex m_latestFrameMutex;
    cv::Mat m_lastFrame;
    std::thread m_loadFrameThread;
    size_t m_requestedFrameCount = 0;
};

#endif