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
#include "AmdDbgApiHelpers.h"
#include "Plugins/Process/gdb-remote/ProcessGDBRemoteLog.h"
#include "ThreadAMDGPU.h"
#include "lldb/Utility/Log.h"
#include "llvm/ADT/bit.h"
#include "llvm/Support/MathExtras.h"
#include <amd-dbgapi/amd-dbgapi.h>
#include <cstddef>
#include <cstdint>
#include <memory>

using namespace lldb_private;
using namespace lldb_server;
using namespace lldb_private::process_gdb_remote;
#include <atomic>

template <typename T>
static llvm::Error QueryDispatchInfo(amd_dbgapi_dispatch_id_t dispatch_id,
                                     amd_dbgapi_dispatch_info_t info_type,
                                     T *dest) {
  amd_dbgapi_status_t status =
      amd_dbgapi_dispatch_get_info(dispatch_id, info_type, sizeof(*dest), dest);
  if (status != AMD_DBGAPI_STATUS_SUCCESS) {
    return llvm::createStringError(
        llvm::inconvertibleErrorCode(),
        "Failed to get %s for dispatch %" PRIu64 ": status=%s",
        AmdDbgApiDispatchInfoKindToString(info_type), dispatch_id.handle,
        AmdDbgApiStatusToString(status));
  }
  return llvm::Error::success();
}

static llvm::Expected<size_t>
ComputeNumLanesInWave(const DbgApiWaveInfo &wave_info) {
  uint16_t workgroup_sizes[3];
  if (llvm::Error err = QueryDispatchInfo(
          wave_info.dispatch_id, AMD_DBGAPI_DISPATCH_INFO_WORKGROUP_SIZES,
          &workgroup_sizes))
    return err;

  const size_t wave_size = wave_info.num_lanes_supported;
  const size_t total_num_lanes =
      workgroup_sizes[0] * workgroup_sizes[1] * workgroup_sizes[2];
  const size_t num_waves =
      std::max(llvm::divideCeil(total_num_lanes, wave_size), size_t(1));

  // Most waves have a full wave_size worth of lanes.
  size_t num_lanes = wave_size;

  // If this is the last wave, it may not have a full wave_size worth of lanes.
  const bool is_last_wave = wave_info.index_in_workgroup == num_waves - 1;
  if (is_last_wave) {
    const size_t num_lanes_with_full_waves = (num_waves - 1) * wave_size;
    num_lanes = total_num_lanes - num_lanes_with_full_waves;
  }

  return num_lanes;
}

// Global atomic variable for thread-safe incrementing of llvm::pid_t.
// This is used to have a unique thread ID for each thread.
// It is thread safe in case we want to support handling gpu connection
// on a separate thread at some point.
static std::atomic<lldb::pid_t> g_atomic_tid{2};

void WaveAMDGPU::AddThreadsToList(
    ProcessAMDGPU &process,
    std::vector<std::unique_ptr<NativeThreadProtocol>> &threads) {
  Log *log = GetLog(GDBRLog::Plugin);

  // Reserve a contiguous range of thread IDs for this wave.
  // TODO: the num_lanes is not always correct because we are taking the
  // current exec_mask instead of the exec_mask on wave launch. This means
  // we could be undercounting the number of threads if we first learn about
  // a wave when not all lanes are active. Need to use the dispatch info to
  // figure out the number of threads in the wave.
  llvm::Expected<size_t> num_lanes = ComputeNumLanesInWave(m_wave_info);
  if (!num_lanes) {
    LLDB_LOG_ERROR(log, num_lanes.takeError(),
                   "Failed to compute number of lanes for wave {1}. {0}.",
                   m_wave_id.handle);
    return;
  }
  lldb::pid_t tid_base = g_atomic_tid.fetch_add(*num_lanes);

  // Create a thread for each lane in the wave.
  LLDB_LOGF(log, "Creating %zu threads for wave %lu", *num_lanes,
            m_wave_id.handle);
  for (size_t i = 0; i < *num_lanes; ++i) {
    lldb::pid_t tid = tid_base + i;
    threads.push_back(
        std::make_unique<ThreadAMDGPU>(process, tid, shared_from_this()));
  }
}
