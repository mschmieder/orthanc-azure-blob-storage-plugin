#ifndef __CRYpTO_META_DATA_H__
#define __CRYpTO_META_DATA_H__

#include "EncryptionKey.h"
#include <string>

namespace crypto 
{
  /**
   * @brief      Class for crypto meta data.
   */
  class CryptoMetaData
  {
    public:
      ~CryptoMetaData();

      /**
       * @brief      create a meta data object
       *
       * @param[in]  kek                The kek
       * @param[in]  cek                The cek
       * @param[in]  encryptedDataSize  The encrypted data size
       * @param[in]  decryptedDataSize  The decrypted data size
       *
       * @return     object containing meta data
       */
      static CryptoMetaData create(const crypto::EncryptionKey* kek,
                                   const crypto::EncryptionKey* cek,
                                   const uint64_t encryptedDataSize,
                                   const uint64_t decryptedDataSize);

      /**
       * @brief      create an object from a json string
       *
       * @param[in]  value  The value
       *
       * @return     object containing meta data
       */
      static CryptoMetaData fromJson(const std::string& value);

      /**
       * @brief      serializes object into a json string
       *
       * @param      stream  The stream
       */
      //void serialize(std::ostream& stream) const;

      /**
       * @brief      reads json from a stream
       *
       * @param      istream  The input stream
       *
       * @return     object containing the meta data
       */
      //static CryptoMetaData deserialize(std::istream& istream);


      /**
       * @brief      Gets the decrypted data size.
       *
       * @return     The decrypted data size.
       */
      uint64_t getDecryptedDataSize() const;


      /**
       * @brief      Gets the encrypted data size.
       *
       * @return     The encrypted data size.
       */
      uint64_t getEncryptedDataSize() const;


      /**
       * @brief      Gets the cek meta data as json string.
       *
       * @return     The cek meta data.
       */
      const std::string& getCekMetaData() const {
          return m_metaCek;
      }

      /**
       * @brief      Gets the kek meta data as json string
       *
       * @return     The kek meta data.
       */
      const std::string& getKekMetaData() const {
          return m_metaKek;
      }


      /**
       * @brief      converts object to json string
       *
       * @return     { description_of_the_return_value }
       */
      std::string asJson() const;

    private:
      /**
       * @brief      Constructs the object.
       *
       * @param[in]  metaKek            The meta kek
       * @param[in]  metaCek            The meta cek
       * @param[in]  encryptedDataSize  The encrypted data size
       * @param[in]  decryptedDataSize  The decrypted data size
       */
      CryptoMetaData(const std::string& metaKek,
                     const std::string& metaCek,
                     const uint64_t encryptedDataSize,
                     const uint64_t decryptedDataSize);

      std::string m_metaKek;
      std::string m_metaCek;
      uint64_t m_decryptedDataSize;
      uint64_t m_encryptedDataSize;
  };
}

#endif // __CRYpTO_META_DATA_H__
