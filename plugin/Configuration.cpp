#include "Configuration.h"
#include <fstream>
#include <json/reader.h>
#include <memory>


// For UUID generation
extern "C"
{
#ifdef WIN32
#include <rpc.h>
#else
#include <uuid/uuid.h>
#endif
}


namespace OrthancPlugins
{

  bool readConfiguration(Json::Value& configuration, OrthancPluginContext* context)
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

    Json::Reader reader;
    if (reader.parse(s, configuration))
    {
      return true;
    }
    else
    {
      OrthancPluginLogError(context, "Unable to parse the configuration");
      return false;
    }
  }


  std::string getStringValue(const Json::Value& configuration, const std::string& key, const std::string& defaultValue)
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


  int getIntegerValue(const Json::Value& configuration, const std::string& key, int defaultValue)
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


  bool getBooleanValue(const Json::Value& configuration, const std::string& key, bool defaultValue)
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


   AzureBlobStorageConnection* createConnection(OrthancPluginContext* context, const Json::Value& configuration)
   {
     std::unique_ptr<AzureBlobStorageConnection> connection = std::make_unique<AzureBlobStorageConnection>();
     if (configuration.isMember("AzureBlobStorage"))
     {
        Json::Value c = configuration["AzureBlobStorage"];
        if (c.isMember("ConnectionString"))
        {
          connection->setConnectionString(c["ConnectionString"].asString());
        }
       // if (c.isMember("ChunkSize"))
       // {
       //   connection->SetChunkSize(c["ChunkSize"].asInt());
       // }
       // if (c.isMember("host"))
       // {
       //   connection->SetHost(c["host"].asString());
       // }
       // if (c.isMember("port"))
       // {
       //   connection->SetTcpPort(c["port"].asInt());
       // }
       // if (c.isMember("database"))
       // {
       //   connection->SetDatabase(c["database"].asString());
       // }
       // if (c.isMember("user"))
       // {
       //   connection->SetUser(c["user"].asString());
       // }
       // if (c.isMember("password"))
       // {
       //   connection->SetPassword(c["password"].asString());
       // }
       // if (c.isMember("authenticationDatabase"))
       // {
       //   connection->SetAuthenticationDatabase(c["authenticationDatabase"].asString());
       // }
     }

     return connection.release();
   }


  std::string generateUuid()
  {
#ifdef WIN32
    UUID uuid;
    UuidCreate ( &uuid );

    unsigned char * str;
    UuidToStringA ( &uuid, &str );

    std::string s( ( char* ) str );

    RpcStringFreeA ( &str );
#else
    uuid_t uuid;
    uuid_generate_random ( uuid );
    char s[37];
    uuid_unparse ( uuid, s );
#endif
    return s;
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
