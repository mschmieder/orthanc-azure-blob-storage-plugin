# Orthanc Plugin for storing in Azure Blob Storage
[![Build Status](https://travis-ci.org/mschmieder/orthanc-azure-blob-storage-plugin.svg?branch=master)](https://travis-ci.org/mschmieder/orthanc-azure-blob-storage-plugin)

<!-- TOC -->

- [Orthanc Plugin for storing in Azure Blob Storage](#orthanc-plugin-for-storing-in-azure-blob-storage)
  - [Encryption](#encryption)
  - [Configuration](#configuration)
    - [Enabling the Encryption](#enabling-the-encryption)
  - [Building](#building)
    - [Unix/MacOS](#unix-macos)
    - [Dockerized Environment](#dockerized-environment)

<!-- /TOC -->

This plugin provides Azure Blob Storage capabilities to the Orthanc server. Azure already provides means to secure data at rest by simply enabling KeyVault encryption. This is usually not enough to properly secure PHI that's why this plugin also provides a highly secure client-side AES-256 encryption of all data stored within the Azure Blob Storage.

## Encryption

The client-side enryption uses AES-256 CBC encryption combined with a envelop technique leveraging the AzureKeyVault for securely storing the **key encryption key** (kek)

Each data object uploaded to the blob storage will be encrypted following the principle of

  1. Creating a one-time **content encryption key** (cek)
  2. Encrypting the data with that **cek** 
  3. Encrypting the cek with a **key encryption key** provided by the Azure Key Vault
  4. Storing encryption meta data alongside the encrypted data blob

This mechanism makes sure that the actual **kek** is never available on the client-side. Wraping as well as unwraping is exectuted by the Azure Key Vault REST API providing only encrypted and decrypted content to the client-side.

## Configuration
To enable the plugin within your Orthanc server, setup the location of the plugin within the Orthanc configuration file.

Windows:
```json
"Plugins" : [
"OrthancAzureBlobStorage.dll"
]
```

Linux:
```json
"Plugins" : [
"OrthancAzureBlobStorage.so"
]
```

MacOS:
```json
"Plugins" : [
"OrthancAzureBlobStorage.dylib"
]
```

After that either add the plugin specific configuration into Orthanc's main configuration file, or setup Orthanc to parse all json files within a folder.

```json
{
  "AzureBlobStorage" : {
      "EnableStorage" : true,
      "ConnectionString" : "",
      "ContainerName" : "",
      "EncryptionEnabled" : true,
      "AzureKeyVault": {
        "baseUrl" : "https://xxx-yyyy.vault.azure.net/",
        "clientId" : "1a816e71-9ae0-4666-a6d1-cac05c5de767",
        "clientSecret" : "rwifi2d3456sCslZn40jDDzgZDuhpCp93qZjKX9Y4=",
        "kid" : "OrthancKek"
      }

  }
}
```

### Enabling the Encryption

In order to use the client-side encryption mechanism, add the following options to your json configuration within the plugin area

```json
"AzureBlobStorage" : {
  "EncryptionEnabled" : true,
  "AzureKeyVault": {
    "baseUrl" : "https://xxx-yyyy.vault.azure.net/",
    "clientId" : "1a816e71-9ae0-4666-a6d1-cac05c5de767",
    "clientSecret" : "rwifi2d3456sCslZn40jDDzgZDuhpCp93qZjKX9Y4=",
    "kid" : "https://xxx-yyyy.vault.azure.net/OrthancKek/3b3f64de68ab4c43abcd42085786b6dd"
  }
}
```
Make sure your client has the rights to execute **wrap** and **unwrap** functions on your AzureKeyVault instance, otherwise the encryption of the data will fail.


## Building
The plugin can be build on all major platforms. 

### Unix/MacOS
To build on a Unix/MacOS system use the provided scripts that are simply invoking cmake with the correct parameters

```bash
# bootstrap the system in order to retrieve necessary thridparties libraries as well as the orthanc sources
./bootstrap.sh
./build.sh
```

### Dockerized Environment
You can use docker to build the plugin for Unix. Make sure you have the latest `docker-compose` installed.

```bash
docker-compose run plugin-build
```

### Run Orthanc with AzureBlobStoragePlugin
The latest Orthanc with the plugin already installed is available on dockerhub. In order to have the plugin work correctly make sure you add a proper configuration when running your container.
Create a file containing the json content described as above and then mount that file into `/etc/orthanc/` on the docker environment. Also make sure you assign all ports correctly

```bash
docker run -p 8042:8042 -p 4242:4242 -v plugin-config.json:/etc/orthanc/plugin-config.json mschmieder/orthanc-plugins:latest
```

The default Orthanc config is being used. In order to access your Orthanc instance you will need to provide the user credentials

````
Username: orthanc
Password: orthanc
````

