/**
 * @file ipc_handler.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef IPC_HANDLER_HPP_
#define IPC_HANDLER_HPP_

#include <vector>
#include <string>
#include <optional>
#include <memory>
#include <unordered_map>

#include <yaml-cpp/yaml.h>

#include <boost/interprocess/managed_shared_memory.hpp>

#include "shared_control_hardware_interface/joint.hpp"
#include "shared_control_hardware_interface/common_defs.hpp"

namespace schi
{

  /**
   * @brief Parses the configuration (.yaml) file to gather information about the shared joint objects to be created.
   * @brief This function can either be used by a hardware interface or a controller interface object.
   * 
   * @param path_to_config_file 
   * @return std::optional<std::vector<std::string>> 
   * : The returning vectors first element is the memory region name to be created, the rest is joint object names.
   */
  std::optional<std::vector<std::string>> parseConfigFile(const std::string& path_to_config_file);

  namespace ipc_handler
  { 

    class IpcHandler
    {
      public:

      IpcHandler() = default;

      virtual ~IpcHandler();

      virtual schi::ReturnCode init(const std::string& path_to_config_file) = 0;

      virtual schi::ReturnCode configureSharedObjects(const std::string& path_to_config_file) = 0;

      std::optional<double> getSharedObjectVariable(
        const std::string& joint_name,
        const joint::JointVariables& joint_var
      );

      ReturnCode setSharedObjectVariable(
        const std::string& joint_name,
        const joint::JointVariables& joint_var,
        const double value
      );

      protected:

      std::unordered_map<std::string, std::unique_ptr<schi::joint::Joint>> m_ShJointMap;

      std::string m_ShSegmentName;

      boost::interprocess::managed_shared_memory m_ShMemory;

      std::optional<double> getValueFromShMemory(
        const std::string& joint_name,
        const joint::JointVariables& joint_var
      );

      ReturnCode setValueToShMemory(
        const std::string& joint_name,
        const joint::JointVariables& joint_var,
        const double value
      );

    };

    namespace hardware_interface
    {
      class IpcHandlerHWI : public IpcHandler
      {
        public:

        IpcHandlerHWI();

        ~IpcHandlerHWI();

        schi::ReturnCode init(const std::string& path_to_config_file) override;

        schi::ReturnCode configureSharedObjects(const std::string& path_to_config_file) override;

        private:

        

      };

    }

    namespace controller_interface
    {
      class IpcHandlerCI : public IpcHandler
      {
        public:

        IpcHandlerCI();

        ~IpcHandlerCI();

        schi::ReturnCode init(const std::string& path_to_config_file) override;

        schi::ReturnCode configureSharedObjects(const std::string& path_to_config_file) override;

        private:


      };
    }

    
  } // End of namespace ipc_handler

} // End of namespace schi 

#endif // IPC_HANDLER_HPP_