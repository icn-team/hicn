/*
 * Copyright (c) 2017-2019 Cisco and/or its affiliates.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <hicn/transport/config.h>
#include <hicn/transport/core/connector.h>
#include <hicn/transport/core/hicn_binary_api.h>
#include <hicn/transport/portability/portability.h>
#include <hicn/transport/utils/epoll_event_reactor.h>
#include <hicn/transport/utils/fd_deadline_timer.h>
#include <hicn/transport/utils/ring_buffer.h>

#include <asio.hpp>
#include <deque>
#include <mutex>
#include <thread>

#ifdef __vpp__

#define _Static_assert static_assert

extern "C" {
#include <memif/libmemif.h>
};

namespace transport {

namespace core {

typedef struct {
  uint16_t index;
  /* memif conenction handle */
  memif_conn_handle_t conn;
  /* transmit queue id */
  uint16_t tx_qid;
  /* tx buffers */
  memif_buffer_t *tx_bufs;
  /* allocated tx buffers counter */
  /* number of tx buffers pointing to shared memory */
  uint16_t tx_buf_num;
  /* rx buffers */
  memif_buffer_t *rx_bufs;
  /* allcoated rx buffers counter */
  /* number of rx buffers pointing to shared memory */
  uint16_t rx_buf_num;
  /* interface ip address */
  uint8_t ip_addr[4];
} memif_connection_t;

#define APP_NAME "libtransport"
#define IF_NAME "vpp_connection"

#define MAX_MEMIF_BUFS 1024
#define MEMIF_BUF_SIZE 2048
#define MEMIF_LOG2_RING_SIZE 11

class MemifConnector : public Connector {
 public:
  MemifConnector(PacketReceivedCallback &&receive_callback,
                 OnReconnect &&on_reconnect_callback,
                 asio::io_service &io_service,
                 std::string app_name = "Libtransport");

  ~MemifConnector() override;

  void send(const Packet::MemBufPtr &packet) override;

  void send(const uint8_t *packet, std::size_t len,
            const PacketSentCallback &packet_sent = 0) override;

  void close() override;

  void connect(uint32_t memif_id, long memif_mode);

  //  void runEventsLoop();

  void enableBurst() override;

  void state() override;

  TRANSPORT_ALWAYS_INLINE uint32_t getMemifId() { return memif_id_; };

 private:
  void init();

  int doSend();

  int createMemif(uint32_t index, uint8_t mode, char *s);

  uint32_t getMemifConfiguration();

  int deleteMemif();

  static int controlFdUpdate(int fd, uint8_t events);

  static int onConnect(memif_conn_handle_t conn, void *private_ctx);

  static int onDisconnect(memif_conn_handle_t conn, void *private_ctx);

  static int onInterrupt(memif_conn_handle_t conn, void *private_ctx,
                         uint16_t qid);

  void threadMain();

  int txBurst(uint16_t qid);

  int bufferAlloc(long n, uint16_t qid);

  void sendCallback(const std::error_code &ec);

  void processInputBuffer();

 private:
  static utils::EpollEventReactor main_event_reactor_;
  static std::unique_ptr<std::thread> main_worker_;

  int epfd;
  std::unique_ptr<std::thread> memif_worker_;
  utils::EpollEventReactor event_reactor_;
  volatile bool timer_set_;
  std::unique_ptr<utils::FdDeadlineTimer> send_timer_;
  asio::io_service &io_service_;
  std::unique_ptr<asio::io_service::work> work_;
  uint32_t packet_counter_;
  memif_connection_t memif_connection_;
  uint16_t tx_buf_counter_;

  PacketRing input_buffer_;
  volatile bool is_connecting_;
  volatile bool is_reconnection_;
  bool data_available_;
  bool enable_burst_;
  bool closed_;
  uint32_t memif_id_;
  uint8_t memif_mode_;
  std::string app_name_;
  uint16_t transmission_index_;
  PacketReceivedCallback receive_callback_;
  OnReconnect on_reconnect_callback_;
  utils::SpinLock write_msgs_lock_;
  std::string socket_filename_;

  static std::once_flag flag_;
};

}  // end namespace core

}  // end namespace transport

#endif  // __vpp__