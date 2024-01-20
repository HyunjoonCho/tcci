#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(HelloTest2, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("Hey", "ya");
  // Expect equality.
  EXPECT_EQ(8 * 6, 48);
}