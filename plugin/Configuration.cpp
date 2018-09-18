#include "Configuration.h"
#include <fstream>
#include <json/reader.h>
#include <memory>

// ATTENTION: cpprestsdk pollutes the workspace with the U-macro
//            that create compile time errors in combination with boost
//            as well as cryptopp. Therefore we undefine the macro here
#if defined(U)
# undef U
#endif

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <KeyPool.h>

namespace OrthancPlugins
{

  bool ReadConfiguration(Json::Value& configuration, OrthancPluginContext* context)
  {
    std::string s;

    char* tmp = OrthancPluginGetConfiguration(context);
    if (tmp == NULL)
    {
      OrthancPluginLogError(context, "Error while retrieving the configuration from Orthanc");
      return false;
    }

    s.assign(tmp);
    OrthancPluginFreeString(context, tmp);

    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();

    std::string errors;
    if (!reader->parse(s.c_str(), s.c_str() + s.size(), &configuration, &errors)){
      delete reader;
      OrthancPluginLogError(context, "Unable to parse the configuration");
      return false;
    }

    delete reader;
    return true;
  }


  std::string GetStringValue(const Json::Value& configuration, const std::string& key, const std::string& defaultValue)
  {
    if (configuration.type() != Json::objectValue || !configuration.isMember(key) ||
        configuration[key].type() != Json::stringValue)
    {
      return defaultValue;
    }
    else
    {
      return configuration[key].asString();
    }
  }


  int GetIntegerValue(const Json::Value& configuration, const std::string& key, int defaultValue)
  {
    if (configuration.type() != Json::objectValue || !configuration.isMember(key) ||
        configuration[key].type() != Json::intValue)
    {
      return defaultValue;
    }
    else
    {
      return configuration[key].asInt();
    }
  }


  bool GetBooleanValue(const Json::Value& configuration, const std::string& key, bool defaultValue)
  {
    if (configuration.type() != Json::objectValue || !configuration.isMember(key) ||
        configuration[key].type() != Json::booleanValue)
    {
      return defaultValue;
    }
    else
    {
      return configuration[key].asBool();
    }
  }


   AzureBlobStorageConnection* CreateConnection(OrthancPluginContext* context, const Json::Value& configuration)
   {
     std::unique_ptr<AzureBlobStorageConnection> connection = std::make_unique<AzureBlobStorageConnection>(context);
     if (configuration.isMember("AzureBlobStorage"))
     {
        Json::Value c = configuration["AzureBlobStorage"];
        if (c.isMember("ConnectionString"))
        {
          connection->setConnectionString(c["ConnectionString"].asString());
        }
        if (c.isMember("ContainerName"))
        {
          connection->setContainerName(c["ContainerName"].asString());
        }
        if (c.isMember("EncryptionEnabled"))
        {
          connection->enableEncryption(c["EncryptionEnabled"].asBool());
        }
        if (c.isMember("KeyDecayCount"))
        {
          crypto::KeyPool::getInstance().setKeyDecayCount(c["KeyDecayCount"].asInt());
        }
        if (c.isMember("KeyPoolSize"))
        {
          crypto::KeyPool::getInstance().setDecryptionKeyPoolSize(c["KeyPoolSize"].asInt());
        }
        if (c.isMember("AzureKeyVault"))
        {
          Json::Value jsonVault = c["AzureKeyVault"];
          connection->setKeyVaultBaseUrl(jsonVault["baseUrl"].asString());
          connection->setKeyVaultClientId(jsonVault["clientId"].asString());
          connection->setKeyVaultClientSecret(jsonVault["clientSecret"].asString());
          connection->setKeyVaultKeyId(jsonVault["kid"].asString());
        }
     }

     connection->establishConnection();
     return connection.release();
   }


  std::string GenerateUuid()
  {
    boost::uuids::random_generator generator;
    return boost::uuids::to_string(generator());
  }


  bool isFlagInCommandLineArguments(OrthancPluginContext* context, const std::string& flag)
  {
    uint32_t count = OrthancPluginGetCommandLineArgumentsCount(context);

    for (uint32_t i = 0; i < count; i++)
    {
      char* tmp = OrthancPluginGetCommandLineArgument(context, i);
      std::string arg(tmp);
      OrthancPluginFreeString(context, tmp);

      if (arg == flag)
      {
        return true;
      }
    }

    return false;
  }

}
