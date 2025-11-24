"""
Variable tests for the AMDGPU plugin.
"""

import lldb
import lldbsuite.test.lldbutil as lldbutil
from lldbsuite.test.lldbtest import *
from amdgpu_testcase import *

SOURCE = "variables.hip"
NUM_THREADS = 32

class VariablesAmdGpuTestCase(AmdGpuTestCaseBase):
    def run_to_first_gpu_breakpoint(self):
        """Helper to run to common gpu breakpoint"""
        self.build()

        gpu_threads = self.run_to_gpu_breakpoint(
            SOURCE, "// GPU BREAKPOINT", "// CPU BREAKPOINT - BEFORE LAUNCH"
        )
        self.assertNotEqual(None, gpu_threads, "GPU should be stopped at breakpoint")
        self.assertEqual(len(gpu_threads), NUM_THREADS, f"Expected {NUM_THREADS} threads to be stopped")
        self.select_gpu()
        return gpu_threads
    
    def check_thread_local_variable(self, thread, name, expected_value):
        """Helper to check the value of a thread local variable."""
        self.gpu_process.SetSelectedThread(thread)
        self.expect(
            f"frame variable {name}",
            substrs=[expected_value],
        )

    def test_thread_local(self):
        """Test reading from the thread local variables."""
        gpu_threads = self.run_to_first_gpu_breakpoint()
        lane_0, lane_1 = gpu_threads[0], gpu_threads[1]
        self.check_thread_local_variable(lane_0, "thread_scalar", "(int) thread_scalar = 0")
        self.check_thread_local_variable(lane_1, "thread_scalar", "(int) thread_scalar = 2")
