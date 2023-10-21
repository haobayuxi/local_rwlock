
#include "server.h"

// using namespace std;

uint64_t commits[100];
int times = 10000000;

// // test for random cas and read in memory
// void run_test(int thread_id, int thread_num) {
//   //   random cas
//   auto offset = sizeof(uint64_t) * thread_id;
//   uint64_t exp = 0;
//   //   char* result = (char*)malloc(512);
//   uint64_t result_u64 = 0;
//   for (int i = 0; i < times; i++) {
//     auto ptr = (std::atomic<uint64_t>*)(test_memory + offset);
//     ptr->compare_exchange_strong(exp, 1);
//     // result_u64 = *(test_memory + offset);
//     // memcpy(&result_u64, test_memory + offset, 8);
//     offset = (offset + (100 + thread_num) * sizeof(uint64_t)) % mem_size;
//   }
//   //   std::cout << " thread " << thread_id << std::endl;
//   //   sleep(1);
// }

int main(int argc, char* argv[]) {
  std::string config_filepath = "config.json";
  auto json_config = JsonConfig::load_file(config_filepath);
  auto conf = json_config.get("config");
  int lease = (int)conf.get("lease").get_int64();
  int type = (int)conf.get("type").get_int64();
  int thread_num = (int)conf.get("thread_num").get_int64();
  int rw_ratio = (int)conf.get("ratio").get_int64();
  std::cout << "thread_num=" << thread_num << ", type=" << type
            << ", lease=" << lease << " ,rw_ratio=" << rw_ratio << std::endl;
  run_rwbench(thread_num, type, lease, rw_ratio);
  return 0;
}