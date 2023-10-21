
#include <atomic>
#include <iostream>

#include "../common.h"

struct Node {
  bool wlock;
  int version;
  int data;
};

struct prwlock {
  int data;
};

Node rwdata[64];

class rwbench {
 public:
  rwbench(int thread_id, int type, int lease) {
    seed = 0xdeadbeef + thread_id;
    type = type;
    lease = lease;
    thread_id = thread_id;
  }
  void start();
  bool read_lock(int addr, int thread_num, struct Node* data);
  bool read_unlock(int addr, int version, long long end_time, int thread_id);
  bool write_lock(int addr);
  bool write_unlock(int addr, long long* end_time);
  uint64_t seed;
  int type;
  long long start_time;
  int lease;
  int thread_id;
  int thread_num;

 private:
};

void rwbench::start() { start_time = get_clock_sys_time_ns(); }

void run(rwbench bench, int thread_id);