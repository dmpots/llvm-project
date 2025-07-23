#!/usr/bin/env python3
# This is a helper script to generate data for the register tests.
# It generates two things:
#
#   1. Python lists of expected values.
#   2. The inline assembly to write the values to registers.
#
# The python lists will be included in the python test file and the inline
# assembly will be included in the hip file.
import random
from collections import namedtuple
from typing import List, Tuple, TypeAlias
from pprint import pp
from io import StringIO

RegClass = namedtuple("RegClass", ["gpr", "mov"])
GprValuesList: TypeAlias = List[int]
RegClassesList: TypeAlias = List[Tuple[RegClass, GprValuesList]]


def generate_gpr_values(count) -> GprValuesList:
    """Generate a list of random 32-bit integer values."""
    actual_values = []
    for i in range(count):
        actual_values.append(random.randint(0, 0xFFFFFFFF))
    return actual_values


def generate_gpr_asm(reg_classes: RegClassesList) -> str:
    """Generate the inline assembly string to write the values to the GPRs.
    Example Output:
        asm volatile(
            "v_mov_b32_e32   v0, 0xa3b1799d\n"
            "v_mov_b32_e32   v1, 0x46685257\n"
            : : : "v0", "v1");
    """
    asm_lines = []
    regs = []
    for rc, values in reg_classes:
        for i in range(len(values)):
            reg = f"{rc.gpr}{i}"
            regs.append(reg)
            asm_lines.append(f'  "{rc.mov} {reg:>4}, 0x{values[i]:08X}\\n"')
    clobbers = [f'"{reg}"' for reg in regs]
    lines = ["asm volatile("] + asm_lines + [": : : " + ", ".join(clobbers) + ");"]
    return "\n".join(lines)


def generate_expected_values(reg_classes: RegClassesList) -> str:
    """Generate a string containting python lists of expected values."""
    outs = StringIO()
    expected_values = []
    for reg_class, values in reg_classes:
        print(f"{reg_class.gpr.upper()}GPR_VALUES = [", file=outs)
        for value in values:
            print(f"    0x{value:08X},", file=outs)
        print("]", file=outs)

    return outs.getvalue()


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Generate GPR values and assembly.")
    parser.add_argument("--seed", type=int, default=42, help="Random seed")
    parser.add_argument("--asm", action="store_true", help="Generate inline assembly")
    parser.add_argument(
        "--values", action="store_true", help="Generate python expected values"
    )
    args = parser.parse_args()

    random.seed(args.seed)
    vgpr_values = generate_gpr_values(256)
    sgpr_values = generate_gpr_values(102)
    reg_classes = [
        (RegClass("v", "v_mov_b32"), vgpr_values),
        (RegClass("s", "s_mov_b32"), sgpr_values),
    ]
    if args.asm:
        asm = generate_gpr_asm(reg_classes)
        print(asm)

    if args.values:
        python = generate_expected_values(reg_classes)
        print(python)
