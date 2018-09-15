#include "KeyPool.h"
#include <stdlib.h>     /* atexit */

using namespace crypto;

KeyPool *KeyPool::m_pInstance = nullptr;
bool KeyPool::m_bDestroyed = false;
std::recursive_mutex KeyPool::m_singletonMutex;

KeyPool &KeyPool::getInstance()
{
    if (!m_pInstance)
    {
        std::lock_guard<std::recursive_mutex> lock(m_singletonMutex);
        if (!m_pInstance)
        {
            if (m_bDestroyed)
            {
                onDeadReference();
            }
            else
            {
                create();
            }
        }
    }
    return *m_pInstance;
}

void KeyPool::create()
{
  static KeyPool theInstance;
  m_pInstance = &theInstance;
}

void KeyPool::onDeadReference()
{
  create();

  new(m_pInstance) KeyPool;
  atexit(killPhoenix);

  m_bDestroyed = false;
}

void KeyPool::killPhoenix()
{
  // turn Phoenix to ashes!
  m_pInstance->~KeyPool();
}

KeyPool::KeyPool(size_t decayCount, size_t keyPoolSize)
    : m_decayCount(decayCount),
      m_keyPoolSize(keyPoolSize)
{
}

KeyPool::~KeyPool()
{
    m_pInstance = nullptr;
    m_bDestroyed = true;
}

std::pair<KeyPool::Key, KeyPool::Key> KeyPool::getEncryptionKey(const EncryptionKey *kek)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    if (!m_activeEncryptionKey || !m_activeEncryptionKey->isValid())
    {
        // create a new encryption key
        m_activeEncryptionKey = std::make_shared<KeyPool::DecayingEncryptionKey>(KeyPool::DecayingEncryptionKey(kek, m_decayCount));

        // get the uuid to store it in the decryption key pool
        std::string uuid = m_activeEncryptionKey->getUuid();

        this->addDecryptionKey(uuid, m_activeEncryptionKey->getDecryptionKey());
    }

    // return the keys (usage counter will increase)
    return m_activeEncryptionKey->useKeys();
}

void KeyPool::addDecryptionKey(const std::string &uuid, KeyPool::Key decryptionKey)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    // insert the decryption key into the key pool
    m_decryptionKeyPool.insert(std::make_pair(uuid, decryptionKey));

    // add the uuid into the active decryption key list
    m_activeDecryptionKeyIds.push_back(uuid);

    // check if list exeeds limit
    if (m_activeDecryptionKeyIds.size() > m_keyPoolSize)
    {
        std::string keyId = m_activeDecryptionKeyIds.front();
        m_activeDecryptionKeyIds.pop_front();
        m_decryptionKeyPool.erase(keyId);
    }
}

KeyPool::Key KeyPool::getDecryptionKey(const AesEncryptionKey *encrypted_key, const EncryptionKey *kek)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    KeyPool::Key decryptionKey = nullptr;

    auto it = m_decryptionKeyPool.find(encrypted_key->hexData());
    if (it != m_decryptionKeyPool.end())
    {
        // key was found in decrypted state
        // use this one
        decryptionKey = it->second;
    }
    else
    {
        // decrypt the content encryption key using the key encryption key and
        // add the stored initialization vector
        decryptionKey = std::make_shared<AesEncryptionKey>(AesEncryptionKey(kek->unwrap(encrypted_key->data(), encrypted_key->size()),
                                                                            encrypted_key->iv()));

        this->addDecryptionKey(encrypted_key->hexData(), decryptionKey);
    }

    return decryptionKey;
}
