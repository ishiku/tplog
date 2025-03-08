#include <gtest/gtest.h>

#include <tplog/tplog.h>

TEST(dummy, should_pass) {
    EXPECT_EQ( tplog::foo(), TPLOG_VERSION );
}