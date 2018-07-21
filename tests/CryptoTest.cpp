#include "gtest/gtest.h"
#include <string>
#include <iostream>
#include <fstream>
#include <array>

#include <Crypto.h>
#include <AzureKeyVaultClient.h>

const std::string inputFileName = "/Users/scm/gitlab_tests.pdf";
const std::string encryptedFileName = "/Users/scm/gitlab_tests_encrypted.pdf";
const std::string recoveredFileName = "/Users/scm/gitlab_tests_recovered.pdf";

TEST(CryptoTests, AzureKeyVault)
{
    utility::string_t baseUrl = _XPLATSTR("https://always-encrypted-vault.vault.azure.net/");
    utility::string_t keyVaultName = _XPLATSTR("always-encrypted-vault");
    utility::string_t clientId = _XPLATSTR("");
    utility::string_t clientSecret = _XPLATSTR("");

    az::AzureKeyVaultClient kvc(baseUrl);
    kvc.Authenticate(clientId, clientSecret, keyVaultName).wait();


    utility::string_t wrap_value = kvc.wrapKey("Matthias Schmieder", "OrthancKek", "3b3f64de68ab4c43abcd42085786b6dd");
    std::cout<<wrap_value<<std::endl;

    utility::string_t unwrap_value = kvc.unwrapKey(wrap_value, "OrthancKek", "3b3f64de68ab4c43abcd42085786b6dd");
    std::cout<<unwrap_value<<std::endl;

    utility::string_t enc_value = kvc.encrypt("Matthias Schmieder", "OrthancKek", "3b3f64de68ab4c43abcd42085786b6dd");
    std::cout<<enc_value<<std::endl;

    utility::string_t dec_value = kvc.decrypt(enc_value, "OrthancKek", "3b3f64de68ab4c43abcd42085786b6dd");
    std::cout<<dec_value<<std::endl;

    //kvc.unwarpKey("Matthias Schmieder", "OrthancKek", "3b3f64de68ab4c43abcd42085786b6dd");

}

// TEST(CryptoTests, EncryptBlob)
// {
//   std::ifstream ifs(inputFileName, std::ios::binary);
//   std::vector<uint8_t> vec;
//   size_t numBytes;
//   if (ifs)
//   {
//     ifs.seekg(0, std::ios::end);
//     numBytes = ifs.tellg();
//     ifs.seekg(0, std::ios::beg);

//     vec.resize(numBytes);
//     ifs.read((char*)vec.data(), numBytes);
//   }
//   ifs.close();

//   crypto::AesEncryptionKey kek;
//   kek.generate();

//   std::vector<uint8_t> encrypedData = crypto::Encryption::encrypt(&kek, vec.data(), vec.size());
//   std::vector<uint8_t> decryptedData = crypto::Decryption::decrypt(&kek, encrypedData.data(), encrypedData.size());

//   std::ofstream ofs_recovered(recoveredFileName, std::ios::binary);
//   if (ofs_recovered)
//   {
//     ofs_recovered.write((const char*)decryptedData.data(), decryptedData.size());
//   }
//   ofs_recovered.close();
// }


int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
