#include "KeyPool.h"

using namespace crypto;

std::atomic<KeyPool*> KeyPool::m_instance;
std::mutex KeyPool::m_singletonMutex;

KeyPool* KeyPool::getInstance()
{
    KeyPool* tmp = m_instance.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    if (tmp == nullptr) {
        std::lock_guard<std::mutex> lock(m_singletonMutex);
        tmp = m_instance.load(std::memory_order_relaxed);
        if (tmp == nullptr) {
            tmp = new KeyPool();
            std::atomic_thread_fence(std::memory_order_release);
            m_instance.store(tmp, std::memory_order_relaxed);
        }
    }
    return tmp;
}

KeyPool::KeyPool(size_t decayCount, size_t keyPoolSize)
: m_decayCount(decayCount),
m_keyPoolSize(keyPoolSize)
{
}

KeyPool::~KeyPool() {
}


std::pair<KeyPool::Key, KeyPool::Key> KeyPool::getEncryptionKey(const EncryptionKey *kek)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    if(!m_activeEncryptionKey->isValid())
    {
        // create a new encryption key
        m_activeEncryptionKey = std::make_shared<KeyPool::DecayingEncryptionKey>(KeyPool::DecayingEncryptionKey(kek));

        // get the uuid to store it in the decryption key pool
        std::string uuid = m_activeEncryptionKey->getUuid();

        this->addDecryptionKey(uuid, m_activeEncryptionKey->getDecryptionKey());
    }

    // return the keys (usage counter will increase)
    return m_activeEncryptionKey->getKeys();
}

void KeyPool::addDecryptionKey(const std::string& uuid, KeyPool::Key decryptionKey) {
    // insert the decryption key into the key pool
    m_decryptionKeyPool.insert(std::make_pair(uuid, decryptionKey));

    // add the uuid into the active decryption key list
    m_activeDecryptionKeyIds.push_back(uuid);

    // check if list exeeds limit
    if (m_activeDecryptionKeyIds.size() > m_keyPoolSize) {
        std::string keyId = m_activeDecryptionKeyIds.front();
        m_activeDecryptionKeyIds.pop_front();
        m_decryptionKeyPool.erase(keyId);
    }
}

KeyPool::Key KeyPool::getDecryptionKey(const AesEncryptionKey* encrypted_key, const EncryptionKey* kek)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    KeyPool::Key decryptionKey = nullptr;

    auto it = m_decryptionKeyPool.find(encrypted_key->hexData());
    if (it != m_decryptionKeyPool.end()){
        // key was found in decrypted state
        // use this one
        decryptionKey = it->second;
    }
    else {
        // decrypt the content encryption key using the key encryption key and
        // add the stored initialization vector
        decryptionKey = std::make_shared<AesEncryptionKey>(AesEncryptionKey(kek->unwrap(encrypted_key->data(), encrypted_key->size()),
                                            encrypted_key->iv()));

        this->addDecryptionKey(encrypted_key->hexData(), decryptionKey);
    }

    return decryptionKey;
}