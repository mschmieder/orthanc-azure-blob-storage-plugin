#include "gtest/gtest.h"


#include <AesEncryptionKey.h>
#include <azure/AzureKeyVaultClient.h>
#include <azure/AzureKeyVaultEncryptionKey.h>
#include <Crypto.h>

#include <string>
#include <iostream>
#include <fstream>
#include <array>

const std::string inputFileName = "testfile.data";
const std::string encryptedFileName = "testfile_encrypted.data";
const std::string encryptedFileNameMeta = "testfile_encrypted.info";
const std::string recoveredFileName = "testfile_recovered.data";

const utility::string_t baseUrl = _XPLATSTR("https://XXXXXXXXXX.azure.net/");
const utility::string_t clientId = _XPLATSTR("YYYYY-XXXXXX-WWWWWW-ZZZZZ");
const utility::string_t clientSecret = _XPLATSTR("XXXXXXXXXXXXXXXXXXXXXXXXXXXX");
const utility::string_t keyName = _XPLATSTR("OrthancKek");

 TEST(CryptoTests, EncryptBlob)
 {
   std::ifstream ifs(inputFileName, std::ios::binary);
   std::vector<uint8_t> vec;
   size_t numBytes;
   if (ifs)
   {
     ifs.seekg(0, std::ios::end);
     numBytes = ifs.tellg();
     ifs.seekg(0, std::ios::beg);

     vec.resize(numBytes);
     ifs.read((char*)vec.data(), numBytes);
   }
   ifs.close();


   az::AzureKeyVaultClient kvc(baseUrl, clientId, clientSecret);
   kvc.authenticate();

   az::AzureKeyVaultEncryptionKey kek = kvc.getKeyByName("keyName");
  
   crypto::Cipher encrypedData = crypto::Encryption::encrypt(&kek, vec.data(), vec.size());

   std::ofstream ofs_meta(encryptedFileNameMeta);
   if (ofs_meta)
   {
     ofs_meta<<encrypedData.metaData();
   }
   ofs_meta.close();

   std::vector<uint8_t> decryptedData = crypto::Decryption::decrypt(&kek, crypto::CryptoMetaData::fromJson(encrypedData.metaData()), encrypedData.data().data(), encrypedData.data().size());

   std::ofstream ofs_recovered(recoveredFileName, std::ios::binary);
   if (ofs_recovered)
   {
     ofs_recovered.write((const char*)decryptedData.data(), decryptedData.size());
   }
   ofs_recovered.close();
 }

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
