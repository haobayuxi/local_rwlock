
#include "rwbench.h"

bool rwbench::read_lock(int addr, int thread_id) {
  if (type == RWLOCK_TYPE::Lease || type == RWLOCK_TYPE::OCC) {
    struct Node node = rwdata[addr];
    // memcpy(&node, rwdata[addr], sizeof(Node));
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
  bool cmp = 0;
  //   if (type == RWLOCK_TYPE::Lease) {
  //     if (lease_rwlock_data[addr].wlock.compare_exchange_strong(false, true))
  //     {
  //       return true;
  //     }
  //   } else if (type == RWLOCK_TYPE::OCC) {
  //     occ_data[addr].wlock.compare_exchange_strong();
  //   }
  return true;
}
bool rwbench::write_unlock(int addr) {
  // cas
  //   if (type == RWLOCK_TYPE::Lease) {
  //     lease_rwlock_data[addr].wlock.compare_exchange_stron();
  //   } else if (type == RWLOCK_TYPE::OCC) {
  //     occ_data[addr].wlock.compare_exchange_strong();
  //   }
  return true;
}

void rwbench::run(int thread_id) {
  //
  while (true) {
    auto addr1 = FastRand(&seed);
    auto addr2 = FastRand(&seed);
    auto readonly = true;

    if (readonly) {
      // read only
      if (read_lock(addr1, thread_id)) {
        if (read_lock(addr2, thread_id)) {
          // read data success free read lock
          if (read_unlock(addr1, thread_id)) {
          } else {
            continue;
          }
        } else {
          continue;
        }
      } else {
        continue;
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