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
    /* jointNames.reserve(1); */

    std::string memoryRegionName = configFileRoot["shared_object_config"]["memory_region_name"].as<std::string>(); 
    jointNames.push_back(memoryRegionName);

    auto jointNameArray = configFileRoot["shared_object_config"]["joint_names"].as<std::vector<std::string>>();

    jointNames.insert(
      jointNames.end(),
      jointNameArray.begin(),
      jointNameArray.end()
    );

    return jointNames;
  }

  namespace ipc_handler
  {
    /* IpcHandler::IpcHandler()
    {

    } */

    namespace hardware_interface
    {

      IpcHandlerHWI::IpcHandlerHWI()
      {

      }

      IpcHandlerHWI::~IpcHandlerHWI()
      {

        if(!m_SharedObjectNames.empty())
        {
          for(std::vector<std::string>::const_iterator jointNameIt = m_SharedObjectNames.begin(); jointNameIt != m_SharedObjectNames.end(); jointNameIt++)
          {
            m_ShMemory.destroy<joint::Joint>((*jointNameIt).c_str());
          }
        }
        
        boost::interprocess::shared_memory_object::remove(
          m_ShSegmentName.c_str()
        );
      }

      schi::ReturnCode IpcHandlerHWI::init(const std::vector<std::string> ipc_config_info)
      {
        return configureSharedObjects(ipc_config_info);
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

        const auto ipcInfoOpt = parseConfigFile(path_to_config_file);
        if(!ipcInfoOpt)
        {
          return schi::ReturnCode::Error;
        }

        return init(ipcInfoOpt.value());
      }

      schi::ReturnCode IpcHandlerHWI::configureSharedObjects(const std::vector<std::string> ipc_config_info)
      {
        
        m_ShSegmentName = ipc_config_info[0];
        m_SharedObjectNames.reserve(ipc_config_info.size() - 1);
        m_SharedObjectNames.insert(m_SharedObjectNames.end(), ipc_config_info.begin() + 1, ipc_config_info.end());
        try
        {
          std::size_t requiredSize = (ipc_config_info.size() - 1) * sizeof(schi::joint::Joint);
          if(requiredSize < MINIMUM_SHM_SIZE)
          {
            requiredSize = MINIMUM_SHM_SIZE;
          }
          std::cout << "Required memory size: " << requiredSize << std::endl;
          m_ShMemory = boost::interprocess::managed_shared_memory(
            boost::interprocess::create_only,
            m_ShSegmentName.c_str(),
            requiredSize
          );
          
        }
        catch(const boost::interprocess::interprocess_exception& e)
        {
          std::cout << e.what() << " " << e.get_error_code() << std::endl;
          return ReturnCode::SharedSegmentCreateError;
        }

        for(std::vector<std::string>::const_iterator ipcInfoIt = ipc_config_info.begin() + 1; ipcInfoIt != ipc_config_info.end(); ipcInfoIt++)
        {
          auto currentObjName = *ipcInfoIt;
          m_ShJointMap.insert(
            std::make_pair(
            currentObjName,
            /* std::make_unique<schi::joint::Joint>(
              m_ShMemory.construct<schi::joint::Joint>((*ipcInfoIt).c_str())()
            ) */
            m_ShMemory.construct<schi::joint::Joint>((*ipcInfoIt).c_str())()
            )
          );

          // Check if the current key and value pair is inserted and that the Shared Object pointer is not nullptr.
          if(auto findRes = m_ShJointMap.find(*ipcInfoIt); findRes == m_ShJointMap.end() || !findRes->second)
          {
            return ReturnCode::SharedSegmentNamedObjectSearchError;
          }

          std::cout << "Created shared object " << currentObjName << std::endl;
        }

        return ReturnCode::Success;
      }

      std::optional<double> IpcHandlerHWI::getSharedObjectVariable(
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

    std::optional<double> IpcHandlerHWI::getValueFromShMemory(
        const std::string& joint_name,
        const joint::JointVariables& joint_var
      )
      {
        auto& jointIt = m_ShJointMap.at(joint_name);

        std::optional<double> retVal;
        using namespace joint;
        switch (joint_var)
        {
        case JointVariables::TargetVelocity:
          retVal = jointIt->targetVelocity;
          break;
        case JointVariables::TargetPosition:
          retVal = jointIt->targetPosition;
          break;
        case JointVariables::TargetAcceleration:
          retVal = jointIt->targetAcceleration;
          break;
        case JointVariables::TargetEffort:
          retVal = jointIt->targetEffort;
          break;
        default:
          return std::nullopt;
          break;
        }

        return retVal;
      }

    ReturnCode IpcHandlerHWI::setSharedObjectVariable(
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

    ReturnCode IpcHandlerHWI::setValueToShMemory(
        const std::string& joint_name,
        const joint::JointVariables& joint_var,
        const double value
      )
      {
        auto& jointIt = m_ShJointMap.at(joint_name);

        using namespace joint;
        switch (joint_var)
        {
        case JointVariables::ActualVelocity:
          jointIt->actualVelocity = value;
          break;
        case JointVariables::ActualPosition:
          jointIt->actualPosition = value;
          break;
        case JointVariables::ActualAcceleration:
          jointIt->actualAcceleration = value; 
          break;
        case JointVariables::ActualEffort:
          jointIt->actualEffort = value; 
          break;
        default:
          return ReturnCode::Error;
        }

        return ReturnCode::Success; 
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

      schi::ReturnCode IpcHandlerCI::init(const std::vector<std::string> ipc_config_info)
      {
        return configureSharedObjects(ipc_config_info);
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

        return init(ipcInfo.value());
      }

      schi::ReturnCode IpcHandlerCI::configureSharedObjects(const std::vector<std::string> ipc_config_info)
      {
        
        m_ShSegmentName = ipc_config_info[0];

        try{
          m_ShMemory = boost::interprocess::managed_shared_memory(
          boost::interprocess::open_only,
          m_ShSegmentName.c_str()
        );
        }
        catch(boost::interprocess::interprocess_exception& ex)
        {
          return ReturnCode::SharedSegmentOpenError;
        }
        
        for(std::vector<std::string>::const_iterator ipcInfoIt = ipc_config_info.begin() + 1; ipcInfoIt != ipc_config_info.end(); ipcInfoIt++)
        {
          auto currentObjName = *ipcInfoIt;
          m_ShJointMap.insert(
            std::make_pair(
              *ipcInfoIt,
              m_ShMemory.find<schi::joint::Joint>(
                (*ipcInfoIt).c_str()
              ).first
            )
          );

          // Check if the current key and value pair is inserted and that the Shared Object pointer is not nullptr.
          if(auto findRes = m_ShJointMap.find(*ipcInfoIt); findRes == m_ShJointMap.end() || !findRes->second)
          {
            return ReturnCode::SharedSegmentNamedObjectSearchError;
          }
        
        }

        return ReturnCode::Success;
      }

      std::optional<double> IpcHandlerCI::getSharedObjectVariable(
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

      std::optional<double> IpcHandlerCI::getValueFromShMemory(
        const std::string& joint_name,
        const joint::JointVariables& joint_var
        )
        {
          auto& jointIt = m_ShJointMap.at(joint_name);

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

      ReturnCode IpcHandlerCI::setSharedObjectVariable(
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

      ReturnCode IpcHandlerCI::setValueToShMemory(
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
            jointIt->targetVelocity = value;
            break;
          case JointVariables::TargetPosition:
            jointIt->targetPosition = value;
            break;
          case JointVariables::TargetAcceleration:
            jointIt->targetAcceleration = value; 
            break;
          case JointVariables::TargetEffort:
            jointIt->targetEffort = value; 
            break;
          default:
            return ReturnCode::Error;
          }
  
          return ReturnCode::Success; 
        }

    } // End of namespace controller_interface

  } // End of namespace ipc_handler

} // End of namespace schi