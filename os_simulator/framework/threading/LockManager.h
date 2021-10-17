#ifndef FRAMEWORK_LOCKMANAGER_H
#define FRAMEWORK_LOCKMANAGER_H

#include <uuid/uuid.h>
#include <map>
#include <mutex>
#include "Lock.h"

using namespace std;
namespace Threading {
class ThreadManager;
class LockManager {
    friend class Threading::ThreadManager;

   private:
    map<const char*, boost::mutex*> locks;
    LockManager();
    ~LockManager();
    static LockManager* singleton;

   public:
    static LockManager* getInstance();
    const char* createLock();
    bool lock(const char* lockId);
    bool unlock(const char* lockId);
    void destroyLock(const char* lockId);
    bool isLocked(const char* lockId);
    bool lockExists(const char* lockId);
};
}  // namespace Threading

#endif  // FRAMEWORK_LOCKMANAGER_H
