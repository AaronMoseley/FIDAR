#include "DroneInitializationWidget.h"

DroneInitializationWidget::DroneInitializationWidget() : QDialog(nullptr)
{
    setWindowTitle("Drone Initialization");
    setWindowFlags(Qt::Dialog);
    SetupUI();
}

void DroneInitializationWidget::SetupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout();
    setLayout(mainLayout);

    QHBoxLayout* ipAddressLayout = new QHBoxLayout();
    mainLayout->addLayout(ipAddressLayout);
    ipAddressLayout->addWidget(new QLabel("IP Address:"));
    m_ipAddressEdit = new QLineEdit("192.168.0.0");
    ipAddressLayout->addWidget(m_ipAddressEdit);

    QHBoxLayout* positionLayout = new QHBoxLayout();
    mainLayout->addLayout(positionLayout);
    positionLayout->addWidget(new QLabel("X:"));
    m_xBox = new QDoubleSpinBox();
    InitializePositionSpinBox(m_xBox);
    positionLayout->addWidget(m_xBox);
    positionLayout->addWidget(new QLabel("Y:"));
    m_yBox = new QDoubleSpinBox();
    InitializePositionSpinBox(m_yBox);
    positionLayout->addWidget(m_yBox);
    positionLayout->addWidget(new QLabel("Z:"));
    m_zBox = new QDoubleSpinBox();
    InitializePositionSpinBox(m_zBox);
    positionLayout->addWidget(m_zBox);

    QHBoxLayout* rotationLayout = new QHBoxLayout();
    mainLayout->addLayout(rotationLayout);
    rotationLayout->addWidget(new QLabel("Rotation:"));
    m_rotationBox = new QDoubleSpinBox();
    InitializeRotationSpinBox(m_rotationBox);
    rotationLayout->addWidget(m_rotationBox);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    mainLayout->addLayout(buttonLayout);
    QPushButton* cancelButton = new QPushButton("Cancel");
    connect(cancelButton, &QPushButton::pressed, this, &DroneInitializationWidget::close);
    buttonLayout->addWidget(cancelButton);
    QPushButton* confirmButton = new QPushButton("Add Drone");
    connect(confirmButton, &QPushButton::pressed, this, &DroneInitializationWidget::OKButtonPressed);
    buttonLayout->addWidget(confirmButton);
}

void DroneInitializationWidget::InitializePositionSpinBox(QDoubleSpinBox* spinBox)
{
    spinBox->setValue(0.0);
    spinBox->setMaximum(500.0);
    spinBox->setMinimum(-500.0);
    spinBox->setSingleStep(1.0);
}

void DroneInitializationWidget::InitializeRotationSpinBox(QDoubleSpinBox* spinBox)
{
    spinBox->setValue(0.0);
    spinBox->setMaximum(360.0);
    spinBox->setMinimum(0.0);
    spinBox->setSingleStep(1.0);
}

void DroneInitializationWidget::OKButtonPressed()
{
    setVisible(false);

    if(m_confirmedCallback)
    {
        glm::vec3 enteredPosition = { m_xBox->value(), m_yBox->value(), m_zBox->value() };
        float enteredRotation = m_rotationBox->value();
        std::string enteredIP = m_ipAddressEdit->text().toStdString();

        m_confirmedCallback(enteredPosition, enteredRotation, enteredIP);
    }

    close();
}