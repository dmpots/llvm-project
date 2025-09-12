//===-- ThreadAMDGPU.h --------------------------------------- -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLDB_TOOLS_LLDB_SERVER_THREADAMDGPU_H
#define LLDB_TOOLS_LLDB_SERVER_THREADAMDGPU_H

#include "Plugins/Utils/Utils.h"
#include "RegisterContextAMDGPU.h"
#include "WaveAMDGPU.h"
#include "lldb/Host/common/NativeThreadProtocol.h"
#include "lldb/lldb-private-forward.h"
#include <amd-dbgapi/amd-dbgapi.h>
#include <memory>
#include <string>

namespace lldb_private {
namespace lldb_server {
class ProcessAMDGPU;
class WaveAMDGPU;

class NativeProcessLinux;

class ThreadAMDGPU : public NativeThreadProtocol {
  friend class ProcessAMDGPU;

public:
  ThreadAMDGPU(ProcessAMDGPU &process, lldb::tid_t tid,
               std::shared_ptr<WaveAMDGPU> wave);

  static std::unique_ptr<ThreadAMDGPU>
  CreateGPUShadowThread(ProcessAMDGPU &process);

  // NativeThreadProtocol Interface
  std::string GetName() override;

  lldb::StateType GetState() override;

  bool GetStopReason(ThreadStopInfo &stop_info,
                     std::string &description) override;
  
  void SetStopReason(lldb::StopReason reason) {
    m_stop_info.reason = reason;
  }
  
  RegisterContextAMDGPU &GetRegisterContext() override {
    return m_reg_context;
  }

  Status SetWatchpoint(lldb::addr_t addr, size_t size, uint32_t watch_flags,
                       bool hardware) override;

  Status RemoveWatchpoint(lldb::addr_t addr) override;

  Status SetHardwareBreakpoint(lldb::addr_t addr, size_t size) override;

  Status RemoveHardwareBreakpoint(lldb::addr_t addr) override;

  ProcessAMDGPU &GetProcess();

  const ProcessAMDGPU &GetProcess() const;

  amd_dbgapi_wave_id_t GetWaveID() const {
    if (!m_wave)
      return AMD_DBGAPI_WAVE_NONE;
    return m_wave->GetWaveID();
  }

  WaveAMDGPU *GetWave() const { return m_wave.get(); }

  bool IsShadowThread() const { return m_wave == nullptr; }

private:
  // Member Variables
  lldb::StateType m_state;
  ThreadStopInfo m_stop_info;
  std::string m_description = "";
  RegisterContextAMDGPU m_reg_context;
  std::string m_stop_description;
  std::shared_ptr<WaveAMDGPU> m_wave;
};

using AMDGPUThreadRange = GPUThreadRange<ThreadAMDGPU>;
} // namespace lldb_server
} // namespace lldb_private

#endif // #ifndef LLDB_TOOLS_LLDB_SERVER_THREADAMDGPU_H
