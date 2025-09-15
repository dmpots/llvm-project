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

namespace lldb_private {
namespace lldb_server {
struct DbgApiWaveInfo {
  amd_dbgapi_wave_state_t state;
  amd_dbgapi_wave_stop_reasons_t stop_reason;
  amd_dbgapi_workgroup_id_t workgroup_id;
  amd_dbgapi_dispatch_id_t dispatch_id;
  amd_dbgapi_queue_id_t queue_id;
  amd_dbgapi_agent_id_t agent_id;
  amd_dbgapi_process_id_t process_id;
  amd_dbgapi_architecture_id_t architecture_id;
  amd_dbgapi_global_address_t pc;
  uint64_t exec_mask;
  uint32_t workgroup_coord[3];
  uint32_t index_in_workgroup;
  size_t num_lanes_supported;
};

class WaveAMDGPU {
  public:
  explicit WaveAMDGPU(amd_dbgapi_wave_id_t wave_id) : m_wave_id(wave_id) {}

  void AddThreadsToList(std::vector<std::unique_ptr<NativeThreadProtocol>> &threads) const;
  void SetDbgApiInfo(const DbgApiWaveInfo &wave_info);

  //RegisterContextAMDGPU &GetRegisterContext();

  private:
  amd_dbgapi_wave_id_t m_wave_id;
  //RegisterContextAMDGPU m_reg_context;
  DbgApiWaveInfo m_wave_info;
};
}}
#endif
