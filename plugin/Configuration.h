#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <json/value.h>
#include <orthanc/OrthancCPlugin.h>
#include "AzureBlobStorageConnection.h"

namespace OrthancPlugins
{

  /**
   * @brief      Reads a configuration.
   *
   * @param      configuration  The configuration
   * @param      context        The context
   *
   * @return     { description_of_the_return_value }
   */
  bool readConfiguration(Json::Value& configuration, OrthancPluginContext* context);
  

  /**
   * @brief createConnection
   * @param context
   * @param configuration
   * @return
   */
  AzureBlobStorageConnection* createConnection(OrthancPluginContext* context, const Json::Value& configuration);


  /**
   * @brief      Gets the string value.
   *
   * @param[in]  configuration  The configuration
   * @param[in]  key            The key
   * @param[in]  defaultValue   The default value
   *
   * @return     The string value.
   */
  std::string getStringValue(const Json::Value& configuration, const std::string& key, const std::string& defaultValue);
  
  /**
   * @brief      Gets the integer value.
   *
   * @param[in]  configuration  The configuration
   * @param[in]  key            The key
   * @param[in]  defaultValue   The default value
   *
   * @return     The integer value.
   */
  int getIntegerValue(const Json::Value& configuration, const std::string& key, int defaultValue);
  
  /**
   * @brief      Gets the boolean value.
   *
   * @param[in]  configuration  The configuration
   * @param[in]  key            The key
   * @param[in]  defaultValue   The default value
   *
   * @return     The boolean value.
   */
  bool getBooleanValue(const Json::Value& configuration, const std::string& key, bool defaultValue);
  
  /**
   * @brief      { function_description }
   *
   * @return     { description_of_the_return_value }
   */
  std::string generateUuid();


  /**
   * @brief      Determines if flag in command line arguments.
   *
   * @param      context  The context
   * @param[in]  flag     The flag
   *
   * @return     True if flag in command line arguments, False otherwise.
   */
  bool isFlagInCommandLineArguments(OrthancPluginContext* context, const std::string& flag);

} //namespace OrthancPlugins

#endif // CONFIGURATION_H
