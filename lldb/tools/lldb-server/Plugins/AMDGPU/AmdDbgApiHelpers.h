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
#include <amd-dbgapi/amd-dbgapi.h>
#include <bitset>
#include <memory>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <cassert>

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

struct AmdDbgApiEventSet {
  AmdDbgApiEventSet() = default;

  void AddEvent(amd_dbgapi_event_kind_t event_kind) {
    assert(event_kind < m_events.size());
    m_events.set(event_kind);
  }

  bool HasEvent(amd_dbgapi_event_kind_t event_kind) const {
    assert(event_kind < m_events.size());
    return m_events.test(event_kind);
  }

  bool HasWaveStopEvent() const {
    return HasEvent(AMD_DBGAPI_EVENT_KIND_WAVE_STOP);
  }

  bool HasBreakpointResumeEvent() const {
    return HasEvent(AMD_DBGAPI_EVENT_KIND_BREAKPOINT_RESUME);
  }

  std::string ToString() const {
    std::string s;
    for (size_t i = 0; i < m_events.size(); ++i) {
      if (m_events[i]) {
        if (!s.empty())
          s += "|";
        s +=
            AmdDbgApiEventKindToString(static_cast<amd_dbgapi_event_kind_t>(i));
      }
    }
    return s;
  }

private:
  // Currently we only have 7 event kinds.
  // Allocate a bit of extra space so that we can accommodate more events
  // without modifying this size. 32 is a nice even number.
  std::bitset<32> m_events;
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
  std::size_t operator()(const amd_dbgapi_wave_id_t& wave_id) const noexcept {
    return std::hash<uint64_t>{}(wave_id.handle);
  }
};

// Convenient typedef for unordered_set of wave IDs with custom hasher.
using WaveIdSet = std::unordered_set<amd_dbgapi_wave_id_t, WaveIdHash>;

// Convenient typedef for unordered_map of wave IDs with custom hasher.
template <typename T>
using WaveIdMap = std::unordered_map<amd_dbgapi_wave_id_t, T, WaveIdHash>;

} // namespace lldb_server
} // namespace lldb_private

// Comparison operator for amd_dbgapi_wave_id_t to enable sorting
inline bool operator<(const amd_dbgapi_wave_id_t& lhs, const amd_dbgapi_wave_id_t& rhs) {
  return lhs.handle < rhs.handle;
}
inline bool operator==(const amd_dbgapi_wave_id_t& lhs, const amd_dbgapi_wave_id_t& rhs) {
  return lhs.handle == rhs.handle;
}
#endif
