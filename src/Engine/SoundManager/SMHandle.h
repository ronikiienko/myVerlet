#pragma once

#include <memory>

class SoundManagerImpl;
class SMHandle {
private:
    int m_key = -1;
    std::weak_ptr<SoundManagerImpl> m_soundManagerPtr;

public:
    SMHandle() = default;
    SMHandle(std::weak_ptr<SoundManagerImpl> soundManagerPtr, int key);

    void remove();
    bool isOwner();
    ~SMHandle();

    SMHandle (const SMHandle&) = delete;
    SMHandle& operator= (const SMHandle&) = delete;

    SMHandle(SMHandle&&) noexcept;
    SMHandle& operator= (SMHandle&&) noexcept;
};