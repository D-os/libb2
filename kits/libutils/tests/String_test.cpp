/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "String_test"
#include <utils/Log.h>
#include <utils/String.h>

#include <gtest/gtest.h>

namespace android {

class StringTest : public testing::Test {
protected:
    virtual void SetUp() {
    }

    virtual void TearDown() {
    }
};

} // namespace android

using namespace android;

TEST_F(StringTest, Cstr) {
    String tmp("Hello, world!");

    EXPECT_STREQ(tmp.string(), "Hello, world!");
}

TEST_F(StringTest, OperatorPlus) {
    String src1("Hello, ");

    // Test adding String + const char*
    const char* ccsrc2 = "world!";
    String dst1 = src1 + ccsrc2;
    EXPECT_STREQ(dst1.string(), "Hello, world!");
    EXPECT_STREQ(src1.string(), "Hello, ");
    EXPECT_STREQ(ccsrc2, "world!");

    // Test adding String + String
    String ssrc2("world!");
    String dst2 = src1 + ssrc2;
    EXPECT_STREQ(dst2.string(), "Hello, world!");
    EXPECT_STREQ(src1.string(), "Hello, ");
    EXPECT_STREQ(ssrc2.string(), "world!");
}

TEST_F(StringTest, OperatorPlusEquals) {
    String src1("My voice");

    // Testing String += String
    String src2(" is my passport.");
    src1 += src2;
    EXPECT_STREQ(src1.string(), "My voice is my passport.");
    EXPECT_STREQ(src2.string(), " is my passport.");

    // Adding const char* to the previous string.
    const char* src3 = " Verify me.";
    src1 += src3;
    EXPECT_STREQ(src1.string(), "My voice is my passport. Verify me.");
    EXPECT_STREQ(src2.string(), " is my passport.");
    EXPECT_STREQ(src3, " Verify me.");
}

TEST_F(StringTest, SetToSizeMaxReturnsNoMemory) {
    const char *in = "some string";
    EXPECT_EQ(NO_MEMORY, String("").setTo(in, SIZE_MAX));
}

// http://b/29250543
TEST_F(StringTest, CorrectInvalidSurrogate) {
    // d841d8 is an invalid start for a surrogate pair. Make sure this is handled by ignoring the
    // first character in the pair and handling the rest correctly.
    const char16_t string16[] = u"\xd841\xd841\xdc41\x0000";
    String String(string16);

    EXPECT_EQ(4U, String.length());
}

TEST_F(StringTest, CheckUtf32Conversion) {
    // Since bound checks were added, check the conversion can be done without fatal errors.
    // The utf8 lengths of these are chars are 1 + 2 + 3 + 4 = 10.
    const char32_t string32[] = U"\x0000007f\x000007ff\x0000911\x0010fffe";
    String String(string32);
    EXPECT_EQ(10U, String.length());
}
