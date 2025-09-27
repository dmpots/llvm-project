//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains helpers for working with the amd-dbgapi.
///
//===----------------------------------------------------------------------===//

#ifndef LLDB_TOOLS_LLDB_SERVER_AMDDBGAPIHELPERS_H
#define LLDB_TOOLS_LLDB_SERVER_AMDDBGAPIHELPERS_H
#include "Plugins/Process/gdb-remote/ProcessGDBRemoteLog.h"
#include "lldb/Utility/Log.h"
#include "llvm/Support/Error.h"
#include <algorithm>
#include <amd-dbgapi/amd-dbgapi.h>
#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace lldb_private {
namespace lldb_server {
static const char *AmdDbgApiEventKindToString(amd_dbgapi_event_kind_t kind) {
  switch (kind) {
  case AMD_DBGAPI_EVENT_KIND_NONE:
    return "NONE";

  case AMD_DBGAPI_EVENT_KIND_WAVE_STOP:
    return "WAVE_STOP";

  case AMD_DBGAPI_EVENT_KIND_WAVE_COMMAND_TERMINATED:
    return "WAVE_COMMAND_TERMINATED";

  case AMD_DBGAPI_EVENT_KIND_CODE_OBJECT_LIST_UPDATED:
    return "CODE_OBJECT_LIST_UPDATED";

  case AMD_DBGAPI_EVENT_KIND_BREAKPOINT_RESUME:
    return "BREAKPOINT_RESUME";

  case AMD_DBGAPI_EVENT_KIND_RUNTIME:
    return "RUNTIME";

  case AMD_DBGAPI_EVENT_KIND_QUEUE_ERROR:
    return "QUEUE_ERROR";
  }
  assert(false && "unhandled amd_dbgapi_event_kind_t value");
}

inline const char *
AmdDbgApiWaveInfoKindToString(amd_dbgapi_wave_info_t info_kind) {
  switch (info_kind) {
  case AMD_DBGAPI_WAVE_INFO_STATE:
    return "AMD_DBGAPI_WAVE_INFO_STATE";

  case AMD_DBGAPI_WAVE_INFO_STOP_REASON:
    return "AMD_DBGAPI_WAVE_INFO_STOP_REASON";

  case AMD_DBGAPI_WAVE_INFO_WATCHPOINTS:
    return "AMD_DBGAPI_WAVE_INFO_WATCHPOINTS";

  case AMD_DBGAPI_WAVE_INFO_WORKGROUP:
    return "AMD_DBGAPI_WAVE_INFO_WORKGROUP";

  case AMD_DBGAPI_WAVE_INFO_DISPATCH:
    return "AMD_DBGAPI_WAVE_INFO_DISPATCH";

  case AMD_DBGAPI_WAVE_INFO_QUEUE:
    return "AMD_DBGAPI_WAVE_INFO_QUEUE";

  case AMD_DBGAPI_WAVE_INFO_AGENT:
    return "AMD_DBGAPI_WAVE_INFO_AGENT";

  case AMD_DBGAPI_WAVE_INFO_PROCESS:
    return "AMD_DBGAPI_WAVE_INFO_PROCESS";

  case AMD_DBGAPI_WAVE_INFO_ARCHITECTURE:
    return "AMD_DBGAPI_WAVE_INFO_ARCHITECTURE";

  case AMD_DBGAPI_WAVE_INFO_PC:
    return "AMD_DBGAPI_WAVE_INFO_PC";

  case AMD_DBGAPI_WAVE_INFO_EXEC_MASK:
    return "AMD_DBGAPI_WAVE_INFO_EXEC_MASK";

  case AMD_DBGAPI_WAVE_INFO_WORKGROUP_COORD:
    return "AMD_DBGAPI_WAVE_INFO_WORKGROUP_COORD";

  case AMD_DBGAPI_WAVE_INFO_WAVE_NUMBER_IN_WORKGROUP:
    return "AMD_DBGAPI_WAVE_INFO_WAVE_NUMBER_IN_WORKGROUP";

  case AMD_DBGAPI_WAVE_INFO_LANE_COUNT:
    return "AMD_DBGAPI_WAVE_INFO_LANE_COUNT";
  }
  assert(false && "unhandled amd_dbgapi_wave_info_t value");
}

inline const char *AmdDbgApiStatusToString(amd_dbgapi_status_t status) {
  switch (status) {
  case AMD_DBGAPI_STATUS_SUCCESS:
    return "AMD_DBGAPI_STATUS_SUCCESS";

  case AMD_DBGAPI_STATUS_ERROR:
    return "AMD_DBGAPI_STATUS_ERROR";

  case AMD_DBGAPI_STATUS_FATAL:
    return "AMD_DBGAPI_STATUS_FATAL";

  case AMD_DBGAPI_STATUS_ERROR_NOT_IMPLEMENTED:
    return "AMD_DBGAPI_STATUS_ERROR_NOT_IMPLEMENTED";

  case AMD_DBGAPI_STATUS_ERROR_NOT_AVAILABLE:
    return "AMD_DBGAPI_STATUS_ERROR_NOT_AVAILABLE";

  case AMD_DBGAPI_STATUS_ERROR_NOT_SUPPORTED:
    return "AMD_DBGAPI_STATUS_ERROR_NOT_SUPPORTED";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_ARGUMENT:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_ARGUMENT";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_ARGUMENT_COMPATIBILITY:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_ARGUMENT_COMPATIBILITY";

  case AMD_DBGAPI_STATUS_ERROR_ALREADY_INITIALIZED:
    return "AMD_DBGAPI_STATUS_ERROR_ALREADY_INITIALIZED";

  case AMD_DBGAPI_STATUS_ERROR_NOT_INITIALIZED:
    return "AMD_DBGAPI_STATUS_ERROR_NOT_INITIALIZED";

  case AMD_DBGAPI_STATUS_ERROR_RESTRICTION:
    return "AMD_DBGAPI_STATUS_ERROR_RESTRICTION";

  case AMD_DBGAPI_STATUS_ERROR_ALREADY_ATTACHED:
    return "AMD_DBGAPI_STATUS_ERROR_ALREADY_ATTACHED";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_ARCHITECTURE_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_ARCHITECTURE_ID";

  case AMD_DBGAPI_STATUS_ERROR_ILLEGAL_INSTRUCTION:
    return "AMD_DBGAPI_STATUS_ERROR_ILLEGAL_INSTRUCTION";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_CODE_OBJECT_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_CODE_OBJECT_ID";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_ELF_AMDGPU_MACHINE:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_ELF_AMDGPU_MACHINE";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_PROCESS_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_PROCESS_ID";

  case AMD_DBGAPI_STATUS_ERROR_PROCESS_EXITED:
    return "AMD_DBGAPI_STATUS_ERROR_PROCESS_EXITED";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_AGENT_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_AGENT_ID";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_QUEUE_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_QUEUE_ID";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_DISPATCH_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_DISPATCH_ID";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_WAVE_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_WAVE_ID";

  case AMD_DBGAPI_STATUS_ERROR_WAVE_NOT_STOPPED:
    return "AMD_DBGAPI_STATUS_ERROR_WAVE_NOT_STOPPED";

  case AMD_DBGAPI_STATUS_ERROR_WAVE_STOPPED:
    return "AMD_DBGAPI_STATUS_ERROR_WAVE_STOPPED";

  case AMD_DBGAPI_STATUS_ERROR_WAVE_OUTSTANDING_STOP:
    return "AMD_DBGAPI_STATUS_ERROR_WAVE_OUTSTANDING_STOP";

  case AMD_DBGAPI_STATUS_ERROR_WAVE_NOT_RESUMABLE:
    return "AMD_DBGAPI_STATUS_ERROR_WAVE_NOT_RESUMABLE";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_DISPLACED_STEPPING_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_DISPLACED_STEPPING_ID";

  case AMD_DBGAPI_STATUS_ERROR_DISPLACED_STEPPING_BUFFER_NOT_AVAILABLE:
    return "AMD_DBGAPI_STATUS_ERROR_DISPLACED_STEPPING_BUFFER_NOT_AVAILABLE";

  case AMD_DBGAPI_STATUS_ERROR_DISPLACED_STEPPING_ACTIVE:
    return "AMD_DBGAPI_STATUS_ERROR_DISPLACED_STEPPING_ACTIVE";

  case AMD_DBGAPI_STATUS_ERROR_RESUME_DISPLACED_STEPPING:
    return "AMD_DBGAPI_STATUS_ERROR_RESUME_DISPLACED_STEPPING";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_WATCHPOINT_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_WATCHPOINT_ID";

  case AMD_DBGAPI_STATUS_ERROR_NO_WATCHPOINT_AVAILABLE:
    return "AMD_DBGAPI_STATUS_ERROR_NO_WATCHPOINT_AVAILABLE";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_REGISTER_CLASS_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_REGISTER_CLASS_ID";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_REGISTER_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_REGISTER_ID";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_LANE_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_LANE_ID";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_ADDRESS_CLASS_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_ADDRESS_CLASS_ID";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_ADDRESS_SPACE_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_ADDRESS_SPACE_ID";

  case AMD_DBGAPI_STATUS_ERROR_MEMORY_ACCESS:
    return "AMD_DBGAPI_STATUS_ERROR_MEMORY_ACCESS";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_ADDRESS_SPACE_CONVERSION:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_ADDRESS_SPACE_CONVERSION";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_EVENT_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_EVENT_ID";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_BREAKPOINT_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_BREAKPOINT_ID";

  case AMD_DBGAPI_STATUS_ERROR_CLIENT_CALLBACK:
    return "AMD_DBGAPI_STATUS_ERROR_CLIENT_CALLBACK";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_CLIENT_PROCESS_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_CLIENT_PROCESS_ID";

  case AMD_DBGAPI_STATUS_ERROR_SYMBOL_NOT_FOUND:
    return "AMD_DBGAPI_STATUS_ERROR_SYMBOL_NOT_FOUND";

  case AMD_DBGAPI_STATUS_ERROR_REGISTER_NOT_AVAILABLE:
    return "AMD_DBGAPI_STATUS_ERROR_REGISTER_NOT_AVAILABLE";

  case AMD_DBGAPI_STATUS_ERROR_INVALID_WORKGROUP_ID:
    return "AMD_DBGAPI_STATUS_ERROR_INVALID_WORKGROUP_ID";

  case AMD_DBGAPI_STATUS_ERROR_INCOMPATIBLE_PROCESS_STATE:
    return "AMD_DBGAPI_STATUS_ERROR_INCOMPATIBLE_PROCESS_STATE";

  case AMD_DBGAPI_STATUS_ERROR_PROCESS_FROZEN:
    return "AMD_DBGAPI_STATUS_ERROR_PROCESS_FROZEN";

  case AMD_DBGAPI_STATUS_ERROR_PROCESS_ALREADY_FROZEN:
    return "AMD_DBGAPI_STATUS_ERROR_PROCESS_ALREADY_FROZEN";

  case AMD_DBGAPI_STATUS_ERROR_PROCESS_NOT_FROZEN:
    return "AMD_DBGAPI_STATUS_ERROR_PROCESS_NOT_FROZEN";
  }
  assert(false && "unhandled amd_dbgapi_status_t value");
}

struct AmdDbgApiEventSet {
  AmdDbgApiEventSet() = default;

  void SetLastEvent(amd_dbgapi_event_id_t event_id,
                    amd_dbgapi_event_kind_t event_kind) {
    m_last_event_id = event_id;
    m_events.push_back(event_kind);
  }

  amd_dbgapi_event_id_t GetLastEventID() const { return m_last_event_id; }

  bool HasEvent(amd_dbgapi_event_kind_t event_kind) const {
    return std::find(m_events.begin(), m_events.end(), event_kind) !=
           m_events.end();
  }

  bool HasWaveStopEvent() const {
    return HasEvent(AMD_DBGAPI_EVENT_KIND_WAVE_STOP);
  }

  bool HasBreakpointResumeEvent() const {
    return HasEvent(AMD_DBGAPI_EVENT_KIND_BREAKPOINT_RESUME);
  }

  std::string ToString() const {
    std::string s("[");
    for (size_t i = 0; i < m_events.size(); ++i) {
      if (i != 0)
        s += ", ";
      s += AmdDbgApiEventKindToString(m_events[i]);
    }
    return s + "]";
  }

private:
  // Keep track of events we have seen.
  // We use a vector to aid in debugging so that we can track both the
  // order of events and allow duplicates.
  std::vector<amd_dbgapi_event_kind_t> m_events;
  amd_dbgapi_event_id_t m_last_event_id = AMD_DBGAPI_EVENT_NONE;
};

// Custom deleter for std::unique_ptr that uses FreeDbgApiClientMemory
struct DbgApiClientMemoryDeleter {
  void operator()(void *ptr) const;
};

// Type alias for std::unique_ptr with the custom deleter
//
// Example usage:
// auto ptr =
// DbgApiClientMemoryPtr<SomeType>(static_cast<SomeType*>(raw_ptr_from_dbgapi));
// The memory will be automatically freed when ptr goes out of scope
template <typename T>
using DbgApiClientMemoryPtr = std::unique_ptr<T, DbgApiClientMemoryDeleter>;

// Custom hash function for amd_dbgapi_wave_id_t
struct WaveIdHash {
  std::size_t operator()(const amd_dbgapi_wave_id_t &wave_id) const noexcept {
    return std::hash<uint64_t>{}(wave_id.handle);
  }
};

// Run a command from the amd-dbgapi library and return an llvm::Error if not
// successful.
inline llvm::Error
RunAmdDbgApiCommand(std::function<amd_dbgapi_status_t()> func) {
  amd_dbgapi_status_t status = func();
  if (status != AMD_DBGAPI_STATUS_SUCCESS)
    return llvm::createStringError(llvm::inconvertibleErrorCode(),
                                   "AMD_DBGAPI_STATUS_ERROR: %s",
                                   AmdDbgApiStatusToString(status));
  return llvm::Error::success();
}

// Convenient typedef for unordered_set of wave IDs with custom hasher.
using WaveIdSet = std::unordered_set<amd_dbgapi_wave_id_t, WaveIdHash>;

// Convenient typedef for unordered_map of wave IDs with custom hasher.
template <typename T>
using WaveIdMap = std::unordered_map<amd_dbgapi_wave_id_t, T, WaveIdHash>;

// Convenient typedef for vector of wave IDs.
using WaveIdList = std::vector<amd_dbgapi_wave_id_t>;

} // namespace lldb_server
} // namespace lldb_private

// Comparison operator for amd_dbgapi_wave_id_t to enable sorting
inline bool operator<(const amd_dbgapi_wave_id_t &lhs,
                      const amd_dbgapi_wave_id_t &rhs) {
  return lhs.handle < rhs.handle;
}
inline bool operator==(const amd_dbgapi_wave_id_t &lhs,
                       const amd_dbgapi_wave_id_t &rhs) {
  return lhs.handle == rhs.handle;
}
#endif
