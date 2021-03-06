#ifndef __KEY_POOL_H__

#include "EncryptionKey.h"
#include "AesEncryptionKey.h"
#include "CryptoMetaData.h"

#include <unordered_map>
#include <memory>
#include <list>
#include <utility>
#include <mutex>
#include <atomic>

namespace crypto
{
class KeyPool
{
  public:
    typedef std::shared_ptr<AesEncryptionKey> Key;

     // delete copy and move constructors and assign operators
    KeyPool(KeyPool const&) = delete;             // Copy construct
    KeyPool(KeyPool&&) = delete;                  // Move construct
    KeyPool& operator=(KeyPool const&) = delete;  // Copy assign
    KeyPool& operator=(KeyPool &&) = delete;      // Move assign

    static KeyPool& getInstance();

    std::pair<Key, Key> getEncryptionKey(const EncryptionKey *kek);
    Key getDecryptionKey(const AesEncryptionKey* encrypted_key, const EncryptionKey* kek);

    void setKeyDecayCount(size_t count) {
      m_decayCount = count;
    }

    void setDecryptionKeyPoolSize(size_t size) {
      m_keyPoolSize = size;
    }


  private:
    KeyPool(size_t decayCount = 1, size_t keyPoolSize=10);
    ~KeyPool();

    static void create();
    static void onDeadReference();
    static void killPhoenix();

    void addDecryptionKey(const std::string& uuid, KeyPool::Key);

    class DecayingEncryptionKey
    {
      public:
        typedef std::pair<KeyPool::Key, KeyPool::Key> KeyPair;
        DecayingEncryptionKey(const EncryptionKey *kek, size_t decayCount = 1)
         : m_decayCount(decayCount)
        {
            // generate one time content encryption key
            m_key = std::make_shared<AesEncryptionKey>(AesEncryptionKey());
            m_key->generate();

            // encrypt one time content encryption key (cek) with key encryption key (kek)
            m_encryptedKey = std::make_shared<AesEncryptionKey>(AesEncryptionKey(kek->wrap(m_key->data(), m_key->size()), m_key->iv()));
        }

        bool isValid() const {
            return (m_decayCount > 0);
        }

        KeyPair useKeys()
        {
          m_decayCount = m_decayCount -1;
          return std::make_pair(m_key, m_encryptedKey);
        }

        KeyPool::Key getDecryptionKey()
        {
          return m_key;
        }

        std::string getUuid() const {
          return m_key->hexData();
        }

      private:
        std::shared_ptr<AesEncryptionKey> m_encryptedKey;
        std::shared_ptr<AesEncryptionKey> m_key;

        size_t m_decayCount;
    };

    std::recursive_mutex m_mutex;

    size_t m_decayCount;
    size_t m_keyPoolSize;
    std::unordered_map<std::string, KeyPool::Key > m_decryptionKeyPool;
    std::list<std::string> m_activeDecryptionKeyIds;

    std::shared_ptr<KeyPool::DecayingEncryptionKey> m_activeEncryptionKey;

    // singelton data
    static KeyPool* m_pInstance;
    static bool m_bDestroyed;
    static std::recursive_mutex m_singletonMutex;
};
} // namespace crypto

#endif //__KEY_POOL_H__
