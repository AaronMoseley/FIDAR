#ifndef POINTCLOUDAPP_DRONEMANAGER_H
#define POINTCLOUDAPP_DRONEMANAGER_H

#include "Objects/ObjectComponent.h"
#include "Widgets/DroneControllerWidget.h"
#include "Widgets/DroneInitializationWidget.h"
#include "Components/ImageSourceManager.h"

class DroneManager : public ObjectComponent
{
public:
    DroneManager();

    void Start() override;
	void Update(float deltaTime) override;

private:
    void ActivateDroneInitialization();
    void ActivateDroneController(const glm::vec3& position, float rotation, const std::string& ipAddress);
    void DroneControllerClosedCallback();
    void AddCameraFromDrone();
    void LandDrone();

    DroneControllerWidget* m_currentControllerWidget = nullptr;
};

#endif