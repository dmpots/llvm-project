//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the implementation of the WaveAMDGPU class, which is used
/// to represent a wave on the GPU.
///
//===----------------------------------------------------------------------===//

#include "WaveAMDGPU.h"
#include "ThreadAMDGPU.h"
#include "llvm/ADT/bit.h"
#include <memory>

using namespace lldb_private;
using namespace lldb_server;
#include <atomic>

// Global atomic variable for thread-safe incrementing of llvm::pid_t.
// This is used to have a unique thread ID for each thread.
// It is thread safe in case we want to support handling gpu connection
// on a separate thread at some point.
static std::atomic<lldb::pid_t> g_atomic_tid{2};

void WaveAMDGPU::AddThreadsToList(
    ProcessAMDGPU &process,
    std::vector<std::unique_ptr<NativeThreadProtocol>> &threads) {

  // Reserve a contiguous range of thread IDs for this wave.
  // TODO: the num_lanes is not always correct because we are taking the
  // current exec_mask instead of the exec_mask on wave launch. This means
  // we could be undercounting the number of threads if we first learn about
  // a wave when not all lanes are active. Need to use the dispatch info to
  // figure out the number of threads in the wave.
  size_t num_lanes = llvm::popcount(m_wave_info.exec_mask);
  lldb::pid_t tid_base = g_atomic_tid.fetch_add(num_lanes);

  // Create a thread for each lane in the wave.
  for (size_t i = 0; i < num_lanes; ++i) {
    lldb::pid_t tid = tid_base + i;
    threads.push_back(
        std::make_unique<ThreadAMDGPU>(process, tid, shared_from_this()));
  }
}
