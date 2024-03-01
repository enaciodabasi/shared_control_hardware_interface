/**
 * @file joint.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "shared_control_hardware_interface/joint.hpp"

namespace schi
{
  namespace joint
  { 

    Joint::Joint()
    {

    }
    
    Joint::Joint(Joint& to_copy_from)
    {
        actualPosition = to_copy_from.actualPosition;
        actualVelocity = to_copy_from.actualVelocity;
        actualAcceleration = to_copy_from.actualAcceleration;
        actualEffort = to_copy_from.actualEffort;

        targetPosition = to_copy_from.targetPosition;
        targetVelocity = to_copy_from.targetVelocity;
        targetAcceleration = to_copy_from.targetAcceleration;
        targetEffort = to_copy_from.targetEffort;
    }
  } // End of namespace joint
} // // End of namespace schi