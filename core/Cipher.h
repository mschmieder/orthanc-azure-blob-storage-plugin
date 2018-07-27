#ifndef __CIPHER_H__
#define __CIPHER_H__

#include <vector>
#include <string>

namespace crypto
{
  /**
   * @brief      Class for cipher.
   */
  class Cipher
  {
    public:
      /**
       * @brief      Constructs the object.
       *
       * @param[in]  cipherData      The cipher data
       * @param[in]  cipherMetaData  The cipher meta data
       */
      Cipher(const std::vector<uint8_t>& cipherData,
             const std::string& cipherMetaData);

      /**
       * @brief      Constructs the object.
       */
      Cipher();

      /**
       * @brief      Destroys the object.
       */
      ~Cipher();

      /**
       * @brief      returns the cipher data reference
       *
       * @return     reference to the cipher data
       */
      const std::vector<uint8_t>& data() const;

      /**
       * @brief      returns the meta data json string
       *
       * @return     json strings
       */
      const std::string& metaData() const;

    private:
      std::vector<uint8_t> m_cipherData;
      std::string m_cipherMetaData;
  };
}

#endif // __CIPHER_H__
