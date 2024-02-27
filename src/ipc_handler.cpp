/**
 * @file ipc_handler.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "shared_control_hardware_interface/ipc_handler.hpp"

namespace schi
{

  std::optional<std::vector<std::string>> parseConfigFile(const std::string& path_to_config_file)
  {
    YAML::Node configFileRoot = YAML::LoadFile(path_to_config_file);

    if(configFileRoot.IsNull())
    {
      return std::nullopt;
    }

    std::vector<std::string> jointNames;
    jointNames.reserve(1);

    std::string memoryRegionName = configFileRoot["hardware_interface"]["memory_region_name"].as<std::string>(); 
    jointNames[0] = memoryRegionName;

    YAML::Node jointArrayNode = configFileRoot["hardware_interface"]["joint_names"];

    for(YAML::const_iterator jointNameIt = jointArrayNode.begin(); jointNameIt != jointArrayNode.end(); jointNameIt++)
    {
      jointNames.push_back(
        jointNameIt->second.as<std::string>()
      );  
    } 

    return jointNames;
  }

  namespace ipc_handler
  {
    
    std::optional<double> IpcHandler::getSharedObjectVariable(
      const std::string& joint_name,
      const joint::JointVariables& joint_var
    )
    {
      auto findObj = m_ShJointMap.find(joint_name);

      if(findObj == m_ShJointMap.end())
      {
        return std::nullopt;
      }

      return getValueFromShMemory(
        joint_name,
        joint_var
      );
    }

    std::optional<double> IpcHandler::getValueFromShMemory(
        const std::string& joint_name,
        const joint::JointVariables& joint_var
    )
    {
      auto& jointIt = m_ShJointMap.at(joint_name);

      double val;
      std::optional<double> retVal;
      using namespace joint;
      switch (joint_var)
      {
      case JointVariables::ActualVelocity:
        retVal = jointIt->actualVelocity;
        break;
      case JointVariables::ActualPosition:
        retVal = jointIt->actualPosition;
        break;
      case JointVariables::ActualAcceleration:
        retVal = jointIt->actualAcceleration;
        break;
      case JointVariables::ActualEffort:
        retVal = jointIt->actualEffort;
        break;
      default:
        return std::nullopt;
        break;
      }

      return retVal;
    }

    ReturnCode IpcHandler::setSharedObjectVariable(
        const std::string& joint_name,
        const joint::JointVariables& joint_var,
        const double value
    )
    {
      
      auto findObj = m_ShJointMap.find(joint_name);

      if(findObj == m_ShJointMap.end())
      {
        return ReturnCode::Error;
      }

      return setValueToShMemory(
        joint_name,
        joint_var,
        value
      );
    }

    ReturnCode IpcHandler::setValueToShMemory(
        const std::string& joint_name,
        const joint::JointVariables& joint_var,
        const double value
    )
    {
      auto& jointIt = m_ShJointMap.at(joint_name);

      using namespace joint;
      switch (joint_var)
      {
      case JointVariables::TargetVelocity:
        jointIt->targetVelocity;
        break;
      case JointVariables::TargetPosition:
        jointIt->targetPosition;
        break;
      case JointVariables::TargetAcceleration:
        jointIt->targetAcceleration;
        break;
      case JointVariables::TargetEffort:
        jointIt->targetEffort;
        break;
      default:
        return ReturnCode::Error;
      }

      return ReturnCode::Success; 
    }

    namespace hardware_interface
    {

      IpcHandlerHWI::IpcHandlerHWI()
      {

      }

      IpcHandlerHWI::~IpcHandlerHWI()
      {

      }

      schi::ReturnCode IpcHandlerHWI::init(const std::string& path_to_config_file)
      {
        if(path_to_config_file.empty())
        {
          return schi::ReturnCode::Error;
        }
        const auto ipcInfo = parseConfigFile(path_to_config_file);
        if(!ipcInfo)
        {
          return schi::ReturnCode::Error;
        }

        return configureSharedObjects(path_to_config_file);
      }

      schi::ReturnCode IpcHandlerHWI::configureSharedObjects(const std::string& path_to_config_file)
      {
        const auto ipcInfoOpt = parseConfigFile(path_to_config_file);
        if(!ipcInfoOpt)
        {
          return schi::ReturnCode::Error;
        }
        
        const std::vector<std::string> ipcInfo = ipcInfoOpt.value();
        m_ShSegmentName = ipcInfo[0];

        m_ShMemory = boost::interprocess::managed_shared_memory(
          boost::interprocess::open_only,
          m_ShSegmentName.c_str()/* ,
          ((ipcInfo.size() - 1) * sizeof(schi::joint::Joint)) */
        );

        for(std::vector<std::string>::const_iterator ipcInfoIt = ipcInfo.begin() + 1; ipcInfoIt != ipcInfo.end(); ipcInfoIt++)
        {
          auto currentObjName = *ipcInfoIt;
          m_ShJointMap.insert(
            std::make_pair(
            currentObjName,
            std::make_unique<schi::joint::Joint>(
              m_ShMemory.find<schi::joint::Joint>((*ipcInfoIt).c_str()).first
            ))
          );
        }


      }

    } // End of namespace hardware_interface

    namespace controller_interface
    {
      IpcHandlerCI::IpcHandlerCI()
      {

      }

      IpcHandlerCI::~IpcHandlerCI()
      {

      }

      schi::ReturnCode IpcHandlerCI::init(const std::string& path_to_config_file)
      {
        if(path_to_config_file.empty())
        {
          return schi::ReturnCode::Error;
        }
        const auto ipcInfo = parseConfigFile(path_to_config_file);
        if(!ipcInfo)
        {
          return schi::ReturnCode::Error;
        }

        return configureSharedObjects(path_to_config_file);
      }

      schi::ReturnCode IpcHandlerCI::configureSharedObjects(const std::string& path_to_config_file)
      {

      }
    } // End of namespace controller_interface

  } // End of namespace ipc_handler

} // End of namespace schi