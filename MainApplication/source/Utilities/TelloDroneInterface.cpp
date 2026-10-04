#include "TelloDroneInterface.h"

TelloDroneInterface::TelloDroneInterface(const std::string& ipAddress)
{
    m_socketInterface = std::make_shared<SocketInterface>(ipAddress, kTelloPort);
}

bool TelloDroneInterface::TakeoffDrone()
{
    std::string response = "";
    bool result = m_socketInterface->SendCommand("command", response);

    if(result == false || response.find("ok") == std::string::npos)
    {
        return false;
    }

    result = m_socketInterface->SendCommand("streamon", response);

    if(result == false || response.find("ok") == std::string::npos)
    {
        return false;
    }

    result = m_socketInterface->SendCommand("takeoff", response);

    if(result == false || response.find("ok") == std::string::npos)
    {
        return false;
    }

    setenv("OPENCV_FFMPEG_CAPTURE_OPTIONS", "fflags;nobuffer|flags;low_delay", 1);
    m_videoFeed = std::make_shared<cv::VideoCapture>("udp://0.0.0.0:11111", cv::CAP_FFMPEG);
    result &= m_videoFeed->isOpened();

    if(result)
    {
        m_loadFrameThread = std::thread(&TelloDroneInterface::FrameLoadThread, this);
        std::filesystem::create_directory(kDroneCaptureDirectory);
    }

    return result;
}

bool TelloDroneInterface::MoveDrone(MovementType movementType)
{
    std::string command = kMovementTypeToCommand.at(movementType) + " ";
    switch(movementType)
    {
        case Up:
        case Down:
            command += std::to_string(kVerticalMovementCM);
            break;
        case Forward:
        case Backward:
        case Left:
        case Right:
            command += std::to_string(kHorizontalMovementCM);
            break;
        case RotateClockwise:
        case RotateCounterClockwise:
            command += std::to_string(kRotationAmountDegrees);
            break;
    }

    std::string response = "";
    bool result = m_socketInterface->SendCommand(command, response);
    if(result == false || response.find("ok") == std::string::npos)
    {
        return false;
    }

    glm::quat rotationQuat = glm::quat(glm::vec3(0.0f, glm::radians(m_currentDroneRotation), 0.0f));
    glm::vec3 forward = rotationQuat * glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 right   = rotationQuat * glm::vec3(1.0f, 0.0f,  0.0f);

    switch(movementType)
    {
        case Up:
            m_currentDronePosition.y += kVerticalMovementCM / 100.0f;
            break;
        case Down:
            m_currentDronePosition.y -= kVerticalMovementCM / 100.0f;
            break;
        case Forward:
            m_currentDronePosition += forward * (kHorizontalMovementCM / 100.0f);
            break;
        case Backward:
            m_currentDronePosition += forward * (-kHorizontalMovementCM / 100.0f);
            break;
        case Left:
            m_currentDronePosition += right * (-kHorizontalMovementCM / 100.0f);
            break;
        case Right:
            m_currentDronePosition += right * (kHorizontalMovementCM / 100.0f);
            break;
        case RotateClockwise:
            m_currentDroneRotation -= kRotationAmountRadians;
            break;
        case RotateCounterClockwise:
            m_currentDroneRotation += kRotationAmountRadians;
            break;
    }

    return true;
}

void TelloDroneInterface::LandDrone()
{
    std::string response = "";

    m_threadsRunning = false;
    m_loadFrameThread.join();

    m_socketInterface->SendCommand("streamoff", response);
    m_socketInterface->SendCommand("land", response);
}

void TelloDroneInterface::FrameLoadThread()
{
    cv::Mat currentFrame;
    while(m_threadsRunning)
    {
        if(m_videoFeed->isOpened() == false)
        {
            m_threadsRunning = false;
            break;
        }

        try
        {
            if (!m_videoFeed->read(currentFrame) || currentFrame.empty()) 
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                continue;
            }

            {
                std::lock_guard<std::mutex> lock(m_latestFrameMutex);
                currentFrame.copyTo(m_lastFrame);
            }
        } catch(const std::exception& e)
        {
            m_threadsRunning = false;
            return;
        }
    }
}

void TelloDroneInterface::GetCurrentDroneImage(cv::Mat& outImage)
{
    std::lock_guard<std::mutex> lock(m_latestFrameMutex);
    m_lastFrame.copyTo(outImage);
}

std::filesystem::path TelloDroneInterface::GetCurrentDroneImage()
{
    std::lock_guard<std::mutex> lock(m_latestFrameMutex);

    std::string fileName = kBaseCaptureFileName + std::to_string(m_requestedFrameCount) + ".png";
    std::filesystem::path imagePath = kDroneCaptureDirectory / fileName;
    cv::imwrite(imagePath, m_lastFrame);

    m_requestedFrameCount++;

    return imagePath;
}