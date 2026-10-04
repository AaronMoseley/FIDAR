#include "DroneManager.h"
#include "Objects/RenderObject.h"

DroneManager::DroneManager()
{

}

void DroneManager::Start()
{
    std::function<void()> droneActivationCallback = std::bind(&DroneManager::ActivateDroneInitialization, this);
    GetWindowManager()->AddButton("Activate Drone", droneActivationCallback);
}

void DroneManager::Update(float deltaTime)
{
    if(m_currentControllerWidget == nullptr)
    {
        return;
    }
}

void DroneManager::ActivateDroneInitialization()
{
    DroneInitializationWidget* initializationWidget = new DroneInitializationWidget();

    std::function<void(const glm::vec3&, float, const std::string&)> closedCallback = std::bind(&DroneManager::ActivateDroneController, 
        this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    initializationWidget->SetConfirmedCallback(closedCallback);

    initializationWidget->exec();
}

void DroneManager::ActivateDroneController(const glm::vec3& position, float rotation, const std::string& ipAddress)
{
    m_currentControllerWidget = new DroneControllerWidget(position, rotation, ipAddress);

    if(m_currentControllerWidget->IsConnectionSuccessful() == false)
    {
        m_currentControllerWidget = nullptr;
        return;
    }

    //set callbacks
    std::function<void()> closedCallback = std::bind(&DroneManager::DroneControllerClosedCallback, this);
    m_currentControllerWidget->SetClosedCallback(closedCallback);

    std::function<void()> addCameraCallback = std::bind(&DroneManager::AddCameraFromDrone, this);
    m_currentControllerWidget->SetAddCameraCallback(addCameraCallback);

    m_currentControllerWidget->exec();
}

void DroneManager::DroneControllerClosedCallback()
{
    m_currentControllerWidget->LandDrone();
    m_currentControllerWidget = nullptr;
}

void DroneManager::AddCameraFromDrone()
{
    m_currentControllerWidget->GetCurrentDroneImage();

    //save image somewhere

    std::shared_ptr<ImageSourceManager> imageSourceManager = GetOwner()->GetComponent<ImageSourceManager>();

    //add camera with drone position, rotation, and image source
}

void DroneManager::LandDrone()
{
    m_currentControllerWidget->LandDrone();
    m_currentControllerWidget->close();
    m_currentControllerWidget = nullptr;
}