#include "gtest/gtest.h"
#include <string>
#include <iostream>
#include <fstream>
#include <array>

#include <Crypto.h>

const std::string inputFileName = "/Users/scm/gitlab_tests.pdf";
const std::string encryptedFileName = "/Users/scm/gitlab_tests_encrypted.pdf";
const std::string recoveredFileName = "/Users/scm/gitlab_tests_recovered.pdf";

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

  crypto::EncryptionKeyAes kek;
  kek.generate();

  std::vector<uint8_t> encrypedData = crypto::Encryption::encrypt(&kek, vec.data(), vec.size());
  std::vector<uint8_t> decryptedData = crypto::Decryption::decrypt(&kek, encrypedData.data(), encrypedData.size());

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
