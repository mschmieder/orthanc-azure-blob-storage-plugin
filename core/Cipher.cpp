#include "Cipher.h"

using namespace crypto;

Cipher::Cipher(const std::vector<uint8_t>& cipherData,
       const std::string& cipherMetaData)
  : m_cipherData(cipherData),
    m_cipherMetaData(cipherMetaData)
{
}

Cipher::Cipher()
{
}

Cipher::~Cipher()
{
}

const std::vector<uint8_t>& Cipher::data() const
{
  return m_cipherData;
}

const std::string& Cipher::metaData() const
{
  return m_cipherMetaData;
}

