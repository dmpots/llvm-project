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
#include <string>

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
} // namespace lldb_server
} // namespace lldb_private
#endif
