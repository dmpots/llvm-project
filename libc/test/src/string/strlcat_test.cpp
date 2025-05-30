//===-- Unittests for strlcat ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/string/strlcat.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcStrlcatTest, TooBig) {
  const char *str = "cd";
  char buf[4]{"ab"};
  EXPECT_EQ(LIBC_NAMESPACE::strlcat(buf, str, 3), size_t(4));
  EXPECT_STREQ(buf, "ab");
  EXPECT_EQ(LIBC_NAMESPACE::strlcat(buf, str, 4), size_t(4));
  EXPECT_STREQ(buf, "abc");
}

TEST(LlvmLibcStrlcatTest, Smaller) {
  const char *str = "cd";
  char buf[7]{"ab"};

  EXPECT_EQ(LIBC_NAMESPACE::strlcat(buf, str, 7), size_t(4));
  EXPECT_STREQ(buf, "abcd");
}

TEST(LlvmLibcStrlcatTest, SmallerNoOverwriteAfter0) {
  const char *str = "cd";
  char buf[8]{"ab\0\0efg"};

  EXPECT_EQ(LIBC_NAMESPACE::strlcat(buf, str, 8), size_t(4));
  EXPECT_STREQ(buf, "abcd");
  EXPECT_STREQ(buf + 5, "fg");
}

TEST(LlvmLibcStrlcatTest, No0) {
  const char *str = "cd";
  char buf[7]{"ab"};
  EXPECT_EQ(LIBC_NAMESPACE::strlcat(buf, str, 1), size_t(3));
  EXPECT_STREQ(buf, "ab");
  EXPECT_EQ(LIBC_NAMESPACE::strlcat(buf, str, 2), size_t(4));
  EXPECT_STREQ(buf, "ab");
}
