#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <sstream>
#include <cstdarg>

#define MAX_LOG_MESSAGE_LENGTH 1024

std::mutex g_stdout_mutex;

void log(const char* format, ...) {
  char buffer[MAX_LOG_MESSAGE_LENGTH];

  // reformat the format
  std::stringstream ss;
  ss << "[" << std::this_thread::get_id() << "] " << format;

  // put given args into buffer with the modified format
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), ss.str().c_str(), args);
  va_end(args);

  // mutex lock - to avoid corruption of output
  g_stdout_mutex.lock();
  std::cout << buffer << std::endl;
  g_stdout_mutex.unlock();
}

typedef struct {
  int id;
} worker_ctx;

void worker(worker_ctx ctx) {
  log("%s - enter (id=%d)", __func__, ctx.id);

  log("%s - exit (id=%d)", __func__, ctx.id);
}

void usage(const char* cmd) {
  cmd = strrchr(cmd, '/') ? strrchr(cmd, '/') + 1 : cmd;
  std::cout << "usage: " << std::endl;
  std::cout << "  " << cmd << " <num of threads>" << std::endl;
}

int main(int argc, char* argv[]) {

  // get parameter from argv
  if (argc != 2) {
    usage(argv[0]);
    return -1;
  }
  int num_threads = atoi(argv[1]);
  if (!num_threads) {
    usage(argv[0]);
  }

  // create threads and put them into vector
  std::vector<std::thread*> threads;
  for (int i = 0; i < num_threads; ++i) {
    worker_ctx ctx { i };
    std::thread* t = new std::thread(worker, ctx);
    threads.push_back(t);
  }

  // join
  for (int i = 0; i < num_threads; ++i) {
    threads[i]->join();
    delete threads[i];
  }

  // clean up
  threads.clear();
  return 0;
}
