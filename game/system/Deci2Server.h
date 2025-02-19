#pragma once

#include "common/cross_sockets/XSocketServer.h"

#include "deci_common.h"
#include <condition_variable>

/// @brief Basic implementation of a DECI2 server.
/// Works with deci2.cpp(sceDeci2) to implement the networking on target
class Deci2Server : public XSocketServer {
 public:
  using XSocketServer::XSocketServer;
  virtual ~Deci2Server();

  void post_init() override;

  void read_data();
  void send_data(void* buf, u16 len);

  bool is_client_connected();
  void wait_for_protos_ready();
  void send_proto_ready(Deci2Driver* drivers, int* driver_count);

  void lock();
  void unlock();

 protected:
  void accept_thread_func();

 private:
  bool protocols_ready = false;
  std::condition_variable cv;
  Deci2Driver* d2_drivers = nullptr;
  int* d2_driver_count = nullptr;

  int accepted_socket = -1;
  bool kill_accept_thread = false;
  bool accept_thread_running = false;

  std::thread accept_thread;
  std::mutex server_mutex;

  bool client_connected = false;
};
