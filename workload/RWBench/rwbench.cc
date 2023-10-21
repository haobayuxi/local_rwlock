
#include "rwbench.h"

extern uint64_t commits[100];

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
    if (wlock->compare_exchange_strong(not_locked, true)) {
      return true;
    } else {
      return false;
    }
  } else if (type == RWLOCK_TYPE::Prwlock) {
  }
  return true;
}
bool rwbench::write_unlock(int addr, long long* end_time) {
  // cas
  bool locked = true;
  auto wlock = (std::atomic<bool>*)&rwdata[addr].wlock;
  if (type == RWLOCK_TYPE::Lease) {
    // wait for lease
    while (!((*end_time - start_time) > lease)) {
      *end_time = get_clock_sys_time_ns();
    }
    if (wlock->compare_exchange_strong(locked, true)) {
      return true;
    } else {
      return false;
    }
  } else if (type == RWLOCK_TYPE::OCC) {
    if (wlock->compare_exchange_strong(locked, true)) {
      return true;
    } else {
      return false;
    }
  } else if (type == RWLOCK_TYPE::Prwlock) {
  }
  return true;
}

void run(rwbench bench, int thread_id) {
  //
  struct Node data1;
  struct Node data2;
  while (true) {
    auto addr1 = FastRand(&bench.seed);
    auto addr2 = FastRand(&bench.seed);
    auto readonly = addr1 % 2;

    if (readonly == 1) {
      // read only
      if (bench.read_lock(addr1, thread_id, &data1) &&
          bench.read_lock(addr2, thread_id, &data2)) {
        // read data success free read lock
        auto end_time = get_clock_sys_time_ns();
        if (bench.read_unlock(addr1, data1.version, end_time, thread_id) &&
            bench.read_unlock(addr2, data2.version, end_time, thread_id)) {
          // commit
          commits[thread_id] += 1;
        }

      } else {
        // write
        if (bench.write_lock(addr1) && bench.write_lock(addr2)) {
          // success
          bench.start_time = get_clock_sys_time_ns();
          rwdata[addr1].data += 1;
          rwdata[addr2].data -= 1;
          auto end_time = get_clock_sys_time_ns();
          if (bench.write_unlock(addr1, &end_time) &&
              bench.write_unlock(addr2, &end_time)) {
            commits[thread_id] += 1;
          }
        }
      }
    }
  }
}