"""
Register tests for the AMDGPU plugin.
"""


import lldb
from amdgpu_testcase import AmdGpuTestCaseBase
import lldbsuite.test.lldbutil as lldbutil
from lldbsuite.test.lldbtest import *

# Expected size of a wavefront in threads.
WAVE_SIZE=64

# These values should match the ones written to the registers in the test program.
# You can use `gen.py` in this directory to regenerate this list if needed.
VGPR_VALUES = [
    0xa3b1799d,
    0x46685257,
    0x392456de,
    0xbc8960a9,
    0x6c031199,
    0x07a0ca6e,
    0x37f8a88b,
    0x8b8148f6,
    0x386ecbe0,
    0x96da1dac,
    0xce4a2bbd,
    0xb2b9437a,
    0x571aa876,
    0x27cd8130,
    0x562b0f79,
    0x17be3111,
    0x18c26797,
    0xd8f56413,
    0x9a8dca03,
    0xce9ff57f,
    0xbacfb3d0,
    0x89463e85,
    0x60e7a113,
    0x8d5288f1,
    0xdc98d2c1,
    0x93cd59bf,
    0xb45ed1f0,
    0x19db3ad0,
    0x47294739,
    0x5d65a441,
    0x5ec42e08,
    0xa5e5a5ab,
    0xbaa80dd4,
    0x29d4beef,
    0x6123fdf7,
    0x8e944239,
    0xaf42e12f,
    0xc6a7ee39,
    0x50c187fc,
    0x448aaa9e,
    0x508ebad7,
    0xa7cad415,
    0x757750a9,
    0x43cf2fde,
    0x95a76d79,
    0x663f1c97,
    0xff5e9ff0,
    0x827050a8,
    0x1c11f735,
    0xa0a04dc4,
    0x10435a10,
    0xff01cf99,
    0x877409a9,
    0xb88139b9,
    0xa4161293,
    0x1c8eaee9,
    0x6f4cc69a,
    0x74273ca3,
    0xe9a1fa6f,
    0x9be578c7,
    0x2720797d,
    0xc333e861,
    0x52fbe43b,
    0x04fc6d82,
    0xedd96831,
    0x4eb93eff,
    0x0ed42f1a,
    0xf26b4776,
    0xc40db9b4,
    0x8cbfedb0,
    0x4fcca39a,
    0xa65e688e,
    0x847fd9b4,
    0x1efa2197,
    0x3985c3cf,
    0x568cc69b,
    0x38602ab6,
    0xa18ff6b6,
    0x3a9bedd4,
    0xe7c99b26,
    0xdc1110c1,
    0x3ceddf2d,
    0xab4220a7,
    0x7900f7f9,
    0xc8dcd19f,
    0xceb81f9d,
    0x30beb45f,
    0x6e595ed3,
    0x6c6fa611,
    0xbaa4b71a,
    0x1931e9ee,
    0xdc96925e,
    0x3fa7f104,
    0x72d8567d,
    0x6c006f61,
    0x474ebc19,
    0xec5b227c,
    0x8ce21ea3,
    0xd605e770,
    0xf8102383,
    0xd9441fa5,
    0x2a935d62,
    0x7c52fa17,
    0x0f02bad0,
    0x610461e3,
    0xfc3d3348,
    0x747b6dba,
    0xb7e99aca,
    0x27a0c3d7,
    0x4bf50b52,
    0xf7fd5646,
    0x8acd4e10,
    0xbf7b539b,
    0x0ea2622b,
    0x958ca9ba,
    0x284d82e5,
    0x2f923996,
    0x98543881,
    0x3c365296,
    0x98326856,
    0x9e8fc965,
    0x85d51695,
    0xef48e8d5,
    0xb758588d,
    0x3d1a85dd,
    0x655238a6,
    0x4ccc9bc2,
    0x12922f83,
    0xff002d4d,
    0x43e42caf,
    0xeeea163e,
    0xe1805081,
    0xe117dac3,
    0x5e9953d2,
    0x286218b8,
    0xb41b3143,
    0xa9d3d7c7,
    0x2260e70f,
    0x8da01097,
    0x45b89cd9,
    0x9ad620ab,
    0xb7b56ea7,
    0x7d106c60,
    0xd89a40c0,
    0x46d483f3,
    0x00e85ece,
    0xc56811cd,
    0x430f801d,
    0xbdc14f1f,
    0x0279b6a6,
    0xe767dcea,
    0x8babce3b,
    0xd5a804eb,
    0x25e97977,
    0x20a04502,
    0x4eea04e7,
    0xdc570131,
    0xe61fecc0,
    0x1a50aec3,
    0xee0caeb5,
    0xdd56cc94,
    0xcf8ebc5a,
    0xe1a47e10,
    0x0658663a,
    0xee49f329,
    0xcf8d446a,
    0x444d610b,
    0x1bac27a7,
    0xdf465290,
    0xdbccc477,
    0x38f16a81,
    0x75d66ed4,
    0x3a43b2ba,
    0x3170f437,
    0x5408f9ac,
    0xdd463c09,
    0x4774bc58,
    0x89456f27,
    0xf071d879,
    0xf86c2ca2,
    0x43f59a85,
    0x6f3f920c,
    0x504d281f,
    0x82ec9f2d,
    0x939b462d,
    0x41357e8c,
    0xb572f3d0,
    0xabae4f43,
    0x5d3d9e56,
    0xd9178793,
    0x688c7015,
    0x2095eef6,
    0xf0bbac67,
    0xe71e43a6,
    0x4d0b0d1a,
    0x001a9a8b,
    0x49732d6c,
    0xa79ac9aa,
    0x77097749,
    0x15b52908,
    0x55cee5db,
    0xc04a96c4,
    0xac3c5640,
    0x32fa2de8,
    0x0640be0f,
    0x12a4def0,
    0xb24445a7,
    0x7e8f8095,
    0x3e75c3b4,
    0x6cd66193,
    0x8498e113,
    0xd92c9227,
    0x74daaebf,
    0xcd29a36f,
    0x986f9025,
    0xe4347d51,
    0x81392443,
    0x8c41561b,
    0xe5af6e39,
    0x79844388,
    0xa33dc7af,
    0x8573e793,
    0xa07295e9,
    0x464c04af,
    0x49257af1,
    0x458f1f19,
    0x8a476a87,
    0x236c7b87,
    0x3b33f3d8,
    0xb1a6b1f1,
    0xb4d7e28e,
    0x10714d51,
    0x68586eba,
    0x8ae8905b,
    0x6a702e2f,
    0x6b8e869f,
    0xb20dcb6e,
    0x6160a6b4,
    0x5a0cdd7c,
    0xc0e3befd,
    0x3875394c,
    0x382c043f,
    0x6f92f25e,
    0x076e2bba,
    0x06f028ff,
    0xa48b3dbe,
    0x7631de9d,
    0x0cdf742b,
    0x610cf373,
    0x362f5e5c,
    0x53ac2ab9,
    0x610e6a64,
    0xd4f8fd72,
    0x14f7ce8d,
    0x8a175dfe,
    0x59970043,
]
SGPR_VALUES = [
    0xa66fd7f7,
    0xa6d964a3,
    0xc1156d6d,
    0xf319c125,
    0x2702878b,
    0x20500494,
    0xab61a7b1,
    0x37cc863b,
    0xb31022f0,
    0xc4536f1d,
    0xd1bdb8c0,
    0xf6f7f0cc,
    0xf54ad0a2,
    0xb70b3420,
    0xa092f52a,
    0xc5c14eb4,
    0xc85aca46,
    0x5eddbbbf,
    0x575aed2c,
    0xd97dc9cd,
    0xd284476c,
    0x1b049863,
    0xf5f62c97,
    0xd4262982,
    0xb5122df8,
    0x6f7c15ea,
    0x7bc67e1f,
    0x44b591f7,
    0xda09dfa0,
    0x162f8a24,
    0xe1b294de,
    0x6105716b,
    0x0758e201,
    0xd9d80b8d,
    0x2e8d0e87,
    0x364d7c87,
    0xcc3ebdde,
    0x57207246,
    0xf2b43abf,
    0x15eabb27,
    0xb856d035,
    0xc2171429,
    0xb0cbc61f,
    0x7da67785,
    0xcafda613,
    0x17d2582e,
    0x38ba8abc,
    0xb118f68d,
    0x94e0d3ba,
    0x87ea7ff5,
    0x54aebd1b,
    0xb3ee4d3b,
    0x455ac762,
    0x405bfdc9,
    0x314d3441,
    0x2f65fafa,
    0x7bf47042,
    0x31b1b099,
    0x3a3c563e,
    0x2defe193,
    0x88bd13d1,
    0x4639447b,
    0xf96b648a,
    0x8da8eee4,
    0x7daa39f0,
    0xdf6a8f93,
    0x92f5df7b,
    0x782a65e0,
    0x70c2903f,
    0x0d270659,
    0x7a4c75d4,
    0xd2762bdc,
    0x6694c343,
    0x0db95301,
    0x4dc82a1e,
    0xfe716b14,
    0xc3b290d0,
    0x85c7504b,
    0x715629ee,
    0xfd72b050,
    0x9efba58b,
    0xbd767e35,
    0x3605bf54,
    0xa911d192,
    0x2834e4c0,
    0x1337739e,
    0x00af5b3a,
    0x980402a2,
    0x4a8ff810,
    0x3b4206c5,
    0xb4fb0eb9,
    0x743b65a2,
    0xaff8754d,
    0xec856f37,
    0xef04e57d,
    0x6cd5e859,
    0x8b6870b5,
    0xa5cb63a2,
    0xe88da719,
    0xd39e198b,
    0x1247ea4e,
    0x49e2623d,
]

class RegisterAmdGpuTestCase(AmdGpuTestCaseBase):
    def run_to_reg_gpu_breakpoint(self, reg_base):
        source = "reg.hip"
        gpr = f"{reg_base.upper()}GPR"
        gpu_threads = self.run_to_gpu_breakpoint(
            source, f"// GPU {gpr} BREAKPOINT", "// CPU BREAKPOINT - BEFORE LAUNCH"
        )
        self.assertNotEqual(None, gpu_threads, "GPU should be stopped at breakpoint")
        self.select_gpu()

    def test_vgrp_read_write(self):
        self.do_reg_read_write_tests("v", VGPR_VALUES)

    def test_sgrp_read_write(self):
        self.do_reg_read_write_tests("s", SGPR_VALUES)

    def do_reg_read_write_tests(self, reg_base, known_values):
        """Verify we can read and write the values to registers.
           Values in `known_values` will be written to increasingly
           numbered gprs (e.g v0, v1, ...)."""
        self.build()
        self.run_to_reg_gpu_breakpoint(reg_base)

        for i,value in enumerate(known_values):
            reg = f"{reg_base}{i}"

            # First read the register and verify the expected value.
            self.reg_read(reg, value)

            # Now clear the register and verify the value is 0.
            self.verify_reg_write(reg, 0)

            # Now write the original value back the register.
            # This makes sure we are writing back all the bytes.
            self.verify_reg_write(reg, value)

    def test_vector_lane_read_write(self):
        self.build()
        self.run_to_reg_gpu_breakpoint("v")

        # First check that the register is initialized to the expected value.
        vgpr = [VGPR_VALUES[0]] * WAVE_SIZE
        self.reg_read("v0", vgpr)

        # Make sure we can modify only a few lanes of the register.
        vgpr[1] = 0x10101010
        vgpr[2] = 0x20202020
        vgpr[4] = 0x40404040
        self.verify_reg_write("v0", vgpr)

    def reg_read(self, gpr, expected):
        """Read a value from a register."""
        reg_value_str = f"{gpr} = " + self.get_reg_value_str(gpr, expected)
        self.expect(f"register read {gpr}", substrs=[reg_value_str])

    def reg_write(self, gpr, value):
        """Write a value to a register."""
        reg_value = self.get_reg_value_str(gpr, value)
        self.runCmd(f'register write {gpr} "{reg_value}"')

    def verify_reg_write(self, gpr, value):
        """Write the value to a register and verify we read back the same value."""
        self.reg_write(gpr, value)
        self.reg_read(gpr, value)

    def is_vector_reg(self, gpr):
        """Return true if the register is a vector register."""
        return gpr.startswith("v")

    def get_reg_value_str(self, gpr, value):
        """Return the string representation used by lldb for the value in the register.
           Expands the value to the correct width if needed for vector registers.
        """
        if self.is_vector_reg(gpr):
            value = self.expand_vector_reg_list(value)
            return self.get_vector_reg_str(value)
        else:
            return self.get_scalar_reg_str(value)

    def expand_vector_reg_list(self, value, fill=None):
        """Expand the values to a full wave of values.
            If the value is a scalar, then splat the remaining lanes with the value unless
            a fill value is provided.

            If the value is a list, then splat the remaining lanes with the fill value or
            0 if no fill value is provided.
        """
        if not isinstance(value, list):
            value = [value]
            # If a fill value was not provided then use the first value in the list
            # to splat across the remaining lanes.
            if fill is None:
                fill = value[0]

        # If no fill value was provided, then use 0.
        if fill is None:
            fill = 0

        # Generate the missing values for each lane in the vector register.
        lane_values = [fill] * WAVE_SIZE
        for i,lane in enumerate(value):
            lane_values[i] = lane
        return lane_values

    def get_vector_reg_str(self, lanes):
        """Return the string representation used for a vector register class. """
        self.assertTrue(isinstance(lanes, list), "lanes must be a list")
        self.assertTrue(len(lanes) == WAVE_SIZE, "lanes must be WAVE_SIZE long")
        reg_value = "{"
        reg_value += " ".join([self.convert_32bit_value_to_bytes(v) for v in lanes])
        reg_value += "}"

        return reg_value

    def get_scalar_reg_str(self, value):
        return f"0x{value:08x}"

    def convert_32bit_value_to_bytes(self, value):
        """Convert to a space separated list of bytes in little-endian order."""
        le_byte_values = [(value >> (8 * i)) & 0xFF for i in range(4)]
        return " ".join([f"0x{b:02x}" for b in le_byte_values])
