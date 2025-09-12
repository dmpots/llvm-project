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
class WaveAMDGPU {
  public:
  explicit WaveAMDGPU(amd_dbgapi_wave_id_t wave_id) : m_wave_id(wave_id) {}

  void AddThreadsToList(std::vector<std::unique_ptr<NativeThreadProtocol>> &threads);

  //RegisterContextAMDGPU &GetRegisterContext();

  private:
  amd_dbgapi_wave_id_t m_wave_id;
  //RegisterContextAMDGPU m_reg_context;
};
}}
#endif
