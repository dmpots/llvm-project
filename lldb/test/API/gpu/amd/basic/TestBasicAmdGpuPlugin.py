"""
Basic tests for the AMDGPU plugin.
"""


import lldb
import lldbsuite.test.lldbutil as lldbutil
from lldbsuite.test.lldbtest import *


class BasicAmdGpuTestCase(TestBase):
    # If your test case doesn't stress debug info, then
    # set this to true.  That way it won't be run once for
    # each debug info format.
    NO_DEBUG_INFO_TESTCASE = True

    def test_process_launch(self):
        """There can be many tests in a test case - describe this test here."""
        self.build()
        cpu_target = self.createTestTarget()
        self.assertTrue(cpu_target.IsValid(), "Target is valid")
        self.assertEqual(self.dbg.GetNumTargets(), 1, "There is one target")
        self.assertEqual(self.dbg.GetTargetAtIndex(0), cpu_target)

        # Launch the process and let it run until it is stopped.
        # It should stop because we gpu plugin is hit.
        cpu_process = cpu_target.LaunchSimple(
            None, None, self.get_process_working_directory()
        )
        self.assertTrue(cpu_process.IsValid(), "CPU process is valid")
        self.assertEqual(cpu_process.GetState(), lldb.eStateStopped, PROCESS_STOPPED)

        #self.dbg.SetAsync(True)

        # Get the GPU target
        self.assertEqual(self.dbg.GetNumTargets(), 2, "There are two targets")
        gpu_target = self.dbg.GetTargetAtIndex(1)
        self.assertEqual(self.dbg.GetSelectedTarget(), gpu_target, "GPU target is selected")

        # Get the GPU process
        gpu_process = gpu_target.GetProcess()
        self.assertTrue(gpu_process.IsValid(), "GPU process is valid")
        self.assertEqual(gpu_process.GetState(), lldb.eStateStopped, PROCESS_STOPPED)

        # Get the GPU thread
        gpu_thread = gpu_process.GetThreadAtIndex(0)
        self.assertTrue(gpu_thread.IsValid(), "GPU thread is valid")
        self.assertEqual(gpu_thread.GetName(), "AMD Native Shadow Thread", "GPU thread has the right name")

        gpu_process.Continue()
        self.assertEqual(gpu_process.GetState(), lldb.eStateRunning, "Process is running")
