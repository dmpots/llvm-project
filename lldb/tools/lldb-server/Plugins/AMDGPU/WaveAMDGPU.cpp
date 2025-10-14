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
#include "lldb/lldb-enumerations.h"
#include "llvm/ADT/bit.h"
#include <memory>

using namespace lldb_private;
using namespace lldb_server;

// Reserve a contiguous range of thread IDs for a wave.
// This is used to have a unique thread ID for each thread.
// Returns the base thread ID for the wave.
static lldb::pid_t ReserveTidsForWave(size_t num_lanes) {
  // Static counter for thread ids.
  // Skip over the id that is reserved for the shadow thread.
  // This is not a thread-safe variable, but that should be ok since we
  // only have a lldb-server single thread in the main loop.
  static lldb::pid_t g_tid = ThreadAMDGPU::AMDGPU_SHADOW_THREAD_ID + 1;

  lldb::pid_t tid_base = g_tid;
  g_tid += num_lanes;
  return tid_base;
}

void WaveAMDGPU::AddThreadsToList(
    ProcessAMDGPU &process,
    std::vector<std::unique_ptr<NativeThreadProtocol>> &threads) {

  size_t num_lanes = llvm::popcount(m_wave_info.exec_mask);
  lldb::pid_t tid_base = ReserveTidsForWave(num_lanes);

  // Create a thread for each lane in the wave.
  for (size_t i = 0; i < num_lanes; ++i) {
    lldb::pid_t tid = tid_base + i;
    threads.push_back(
        std::make_unique<ThreadAMDGPU>(process, tid, shared_from_this()));
  }
}

void WaveAMDGPU::SetDbgApiInfo(const DbgApiWaveInfo &wave_info) {
  m_wave_info = wave_info;
  UpdateStopReasonFromWaveInfo();
}

static lldb::StopReason
GetLldbStopReasonForDbgApiStopReason(amd_dbgapi_wave_stop_reasons_t reason) {
  // If none of the bits are set, then we explicitly stopped the wave with
  // a call to `amd_dbgapi_wave_stop`.
  if (reason == AMD_DBGAPI_WAVE_STOP_REASON_NONE)
    return lldb::StopReason::eStopReasonInterrupt;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_BREAKPOINT)
    return lldb::StopReason::eStopReasonBreakpoint;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_WATCHPOINT)
    return lldb::StopReason::eStopReasonWatchpoint;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_SINGLE_STEP)
    return lldb::StopReason::eStopReasonTrace;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_FP_INPUT_DENORMAL)
    return lldb::StopReason::eStopReasonException;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_FP_DIVIDE_BY_0)
    return lldb::StopReason::eStopReasonException;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_FP_OVERFLOW)
    return lldb::StopReason::eStopReasonException;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_FP_UNDERFLOW)
    return lldb::StopReason::eStopReasonException;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_FP_INEXACT)
    return lldb::StopReason::eStopReasonException;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_FP_INVALID_OPERATION)
    return lldb::StopReason::eStopReasonException;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_INT_DIVIDE_BY_0)
    return lldb::StopReason::eStopReasonException;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_DEBUG_TRAP)
    return lldb::StopReason::eStopReasonBreakpoint;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_ASSERT_TRAP)
    return lldb::StopReason::eStopReasonException;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_TRAP)
    return lldb::StopReason::eStopReasonException;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_MEMORY_VIOLATION)
    return lldb::StopReason::eStopReasonException;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_ADDRESS_ERROR)
    return lldb::StopReason::eStopReasonException;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_ILLEGAL_INSTRUCTION)
    return lldb::StopReason::eStopReasonException;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_ECC_ERROR)
    return lldb::StopReason::eStopReasonException;

  if (reason & AMD_DBGAPI_WAVE_STOP_REASON_FATAL_HALT)
    return lldb::StopReason::eStopReasonException;

  return lldb::StopReason::eStopReasonInvalid;
}

void WaveAMDGPU::UpdateStopReasonFromWaveInfo() {
  lldb::StopReason reason = lldb::StopReason::eStopReasonInvalid;
  switch (m_wave_info.state) {
  case AMD_DBGAPI_WAVE_STATE_RUN:
  case AMD_DBGAPI_WAVE_STATE_SINGLE_STEP:
    reason = lldb::StopReason::eStopReasonNone;
    break;
  case AMD_DBGAPI_WAVE_STATE_STOP:
    reason = GetLldbStopReasonForDbgApiStopReason(m_wave_info.stop_reason);
    break;
  }

  assert(reason != lldb::StopReason::eStopReasonInvalid);
  SetStopReason(reason);
}
