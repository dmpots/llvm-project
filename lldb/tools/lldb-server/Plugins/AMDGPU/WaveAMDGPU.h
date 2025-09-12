//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the WaveAMDGPU class, which is used
/// to represent a wave on the GPU.
///
//===----------------------------------------------------------------------===//

#ifndef LLDB_TOOLS_LLDB_SERVER_WAVEAMDGPU_H
#define LLDB_TOOLS_LLDB_SERVER_WAVEAMDGPU_H
#include "RegisterContextAMDGPU.h"
#include "lldb/Host/common/NativeThreadProtocol.h"
#include <amd-dbgapi/amd-dbgapi.h>
#include <memory>

namespace lldb_private {
namespace lldb_server {
class ProcessAMDGPU;

struct DbgApiWaveInfo {
  amd_dbgapi_wave_state_t state = AMD_DBGAPI_WAVE_STATE_RUN;
  amd_dbgapi_wave_stop_reasons_t stop_reason = AMD_DBGAPI_WAVE_STOP_REASON_NONE;
  amd_dbgapi_workgroup_id_t workgroup_id = AMD_DBGAPI_WORKGROUP_NONE;
  amd_dbgapi_dispatch_id_t dispatch_id = AMD_DBGAPI_DISPATCH_NONE;
  amd_dbgapi_queue_id_t queue_id = AMD_DBGAPI_QUEUE_NONE;
  amd_dbgapi_agent_id_t agent_id = AMD_DBGAPI_AGENT_NONE;
  amd_dbgapi_process_id_t process_id = AMD_DBGAPI_PROCESS_NONE;
  amd_dbgapi_architecture_id_t architecture_id = AMD_DBGAPI_ARCHITECTURE_NONE;
  amd_dbgapi_global_address_t pc = 0;
  uint64_t exec_mask = 0;
  uint32_t workgroup_coord[3] = {0, 0, 0};
  uint32_t index_in_workgroup = 0;
  size_t num_lanes_supported = 0;
};

class WaveAMDGPU : public std::enable_shared_from_this<WaveAMDGPU> {
public:
  explicit WaveAMDGPU(amd_dbgapi_wave_id_t wave_id) : m_wave_id(wave_id) {}

  void
  AddThreadsToList(ProcessAMDGPU &process,
                   std::vector<std::unique_ptr<NativeThreadProtocol>> &threads);
  void SetDbgApiInfo(const DbgApiWaveInfo &wave_info) {
    m_wave_info = wave_info;
  }
  amd_dbgapi_wave_id_t GetWaveID() { return m_wave_id; }

private:
  amd_dbgapi_wave_id_t m_wave_id;
  DbgApiWaveInfo m_wave_info;
};
} // namespace lldb_server
} // namespace lldb_private
#endif
