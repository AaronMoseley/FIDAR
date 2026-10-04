#ifndef POINTCLOUDAPP_DRONEINITIALIZATIONWIDGET_H
#define POINTCLOUDAPP_DRONEINITIALIZATIONWIDGET_H

#include <string>

#include <QDialog>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include "glm.hpp"

class DroneInitializationWidget : public QDialog
{
    Q_OBJECT
public:
    DroneInitializationWidget();

    void SetConfirmedCallback(std::function<void(const glm::vec3&, float, const std::string&)> confirmedCallback) { m_confirmedCallback = confirmedCallback; }

private:
    void OKButtonPressed();

    void SetupUI();
    void InitializePositionSpinBox(QDoubleSpinBox* spinBox);
    void InitializeRotationSpinBox(QDoubleSpinBox* spinBox);

    QDoubleSpinBox* m_xBox;
    QDoubleSpinBox* m_yBox;
    QDoubleSpinBox* m_zBox;

    QDoubleSpinBox* m_rotationBox;

    QLineEdit* m_ipAddressEdit;

    std::function<void(const glm::vec3&, float, const std::string&)> m_confirmedCallback;
};

#endif