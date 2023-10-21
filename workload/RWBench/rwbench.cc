
#include "rwbench.h"

bool rwbench::read_lock(int addr, int thread_id, struct Node* data) {
  if (type == RWLOCK_TYPE::Lease || type == RWLOCK_TYPE::OCC) {
    memcpy(data, &rwdata[addr], sizeof(Node));
    if (data->wlock == false) {
      // wlock
      return false;
    }
  } else if (type == RWLOCK_TYPE::Prwlock) {
    // set read lock

    // check write lock
  }
  return true;
}
bool rwbench::read_unlock(int addr, int version, long long end_time,
                          int thread_id) {
  if (type == RWLOCK_TYPE::Lease) {
    // check lease
    if (likely((end_time - start_time) <= lease)) {
      return true;
    }
    // validate
    if (unlikely(version != rwdata[addr].version)) {
      return false;
    }
  } else if (type == RWLOCK_TYPE::OCC) {
    // validate
    if (unlikely(version != rwdata[addr].version)) {
      return false;
    }
  } else if (type == RWLOCK_TYPE::Prwlock) {
    // set read lock to 0
  }
  return true;
}
bool rwbench::write_lock(int addr) {
  // cas
  bool not_locked = false;
  if (type == RWLOCK_TYPE::Lease || type == RWLOCK_TYPE::OCC) {
    auto wlock = (std::atomic<bool>*)&rwdata[addr].wlock;
    while (!wlock->compare_exchange_strong(not_locked, true)) {
    }
    return true;
  } else if (type == RWLOCK_TYPE::Prwlock) {
  }
  return true;
}
bool rwbench::write_unlock(int addr) {
  // cas
  if (type == RWLOCK_TYPE::Lease || type == RWLOCK_TYPE::OCC) {
    // if (rwdata[addr].wlock.compare_exchange_strong(false, true)) {
    //   return true;
    // }
  } else if (type == RWLOCK_TYPE::Prwlock) {
  }
  return true;
}

void rwbench::run(int thread_id) {
  //
  struct Node data1;
  struct Node data2;
  while (true) {
    auto addr1 = FastRand(&seed);
    auto addr2 = FastRand(&seed);
    auto readonly = true;

    if (readonly) {
      // read only
      if (read_lock(addr1, thread_id, &data1) &&
          read_lock(addr2, thread_id, &data2)) {
        // read data success free read lock
        auto end_time = get_clock_sys_time_ns();
        if (read_unlock(addr1, data1.version, end_time, thread_id) &&
            read_unlock(addr2, data2.version, end_time, thread_id)) {
          // commit
        }

      } else {
        // write
        if (write_lock(addr1)) {
          if (write_lock(addr2)) {
            continue;
          }
        } else {
          continue;
        }
      }
    }
  }