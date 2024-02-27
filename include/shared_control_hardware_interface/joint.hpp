/**
 * @file joint.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef JOINT_HPP_
#define JOINT_HPP_

namespace schi
{

  namespace joint
  {

    enum JointVariables
    {
      ActualPosition,
      ActualVelocity,
      ActualAcceleration,
      ActualEffort,
      TargetPosition,
      TargetVelocity,
      TargetAcceleration,
      TargetEffort
    };

    struct Joint
    {
      public:

      explicit Joint() = default;

      double actualPosition;
      double actualVelocity;
      double actualAcceleration;
      double actualEffort;

      double targetPosition;
      double targetVelocity;
      double targetAcceleration;
      double targetEffort;
    };

  } // end of namespace joint

} // End ofnamespace schi


#endif // JOINT_HPP_