#include "gtest/gtest.h"

#include <AesEncryptionKey.h>
#include <azure/AzureKeyVaultClient.h>
#include <azure/AzureKeyVaultEncryptionKey.h>
#include <Crypto.h>

#include <string>
#include <iostream>
#include <fstream>
#include <array>

const std::string inputFileName = "testdata/test.pdf";
const std::string encryptedFileName = "testdata/test_encrpyted.data";
const std::string encryptedFileNameMeta = "testdata/test_encrypted.info";
const std::string recoveredFileName = "testdata/test_recovered.pdf";

const utility::string_t baseUrl = _XPLATSTR("");
const utility::string_t clientId = _XPLATSTR("");
const utility::string_t clientSecret = _XPLATSTR("");
const utility::string_t keyName = _XPLATSTR("");

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
    ifs.read((char *)vec.data(), numBytes);
  }
  ifs.close();

  az::AzureKeyVaultClient kvc(baseUrl, clientId, clientSecret);
  ASSERT_TRUE(kvc.authenticate());

  az::AzureKeyVaultEncryptionKey kek;
  ASSERT_NO_THROW(kek = kvc.getKey(keyName));
  crypto::Cipher cipher;

  ASSERT_NO_THROW(cipher = crypto::Encryption::encrypt(&kek, vec.data(), vec.size()));
  std::string cipherMetaDataString = cipher.metaData();

  crypto::CryptoMetaData metaData = crypto::CryptoMetaData::fromJson(cipherMetaDataString);
  EXPECT_EQ(vec.size(), metaData.getDecryptedDataSize());
  EXPECT_EQ(cipher.data().size(), metaData.getEncryptedDataSize());

  std::ofstream ofs_meta(encryptedFileNameMeta);
  if (ofs_meta)
  {
    ofs_meta << cipherMetaDataString;
  }
  ofs_meta.close();

  //std::vector<uint8_t> decryptedData = crypto::Decryption::decrypt(&kek, crypto::CryptoMetaData::fromJson(encrypedData.metaData()), encrypedData.data().data(), encrypedData.data().size());

  size_t decryptedDataSize = metaData.getDecryptedDataSize();

  // malloc is required, new creates issues with free on orthanc's side
  void* content = malloc(decryptedDataSize);
  size_t size = decryptedDataSize;
  crypto::Decryption::decrypt(&kek,
                              metaData,
                              cipher.data().data(),
                              cipher.data().size(),
                              (uint8_t*)content,
                              decryptedDataSize);

  std::ofstream ofs_recovered(recoveredFileName, std::ios::binary);
  if (ofs_recovered)
  {
    ofs_recovered.write((char*)content, decryptedDataSize);
  }
  ofs_recovered.close();

  ASSERT_EQ( vec.size(), decryptedDataSize );
  for( size_t i = 0; i < decryptedDataSize; i++ ){
    ASSERT_EQ( vec.data()[i], ((uint8_t*)content)[i] );
  }

}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
