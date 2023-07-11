void UnicodeStringTest::TestLargeAppend() {
    if(quick) return;

    IcuTestErrorCode status(*this, "TestLargeAppend");
    // Make a large UnicodeString
    int32_t len = 0xAFFFFFF;
    UnicodeString str;
    char16_t *buf = str.getBuffer(len);
    // A fast way to set buffer to valid Unicode.
    // 4E4E is a valid unicode character
    uprv_memset(buf, 0x4e, len * 2);
    str.releaseBuffer(len);
    UnicodeString dest;
    // Append it 16 times
    // 0xAFFFFFF times 16 is 0xA4FFFFF1,
    // which is greater than INT32_MAX, which is 0x7FFFFFFF.
    int64_t total = 0;
    for (int32_t i = 0; i < 16; i++) {
        dest.append(str);
        total += len;
        if (total <= INT32_MAX) {
            assertFalse("dest is not bogus", dest.isBogus());
        } else {
            assertTrue("dest should be bogus", dest.isBogus());
        }
    }
    dest.remove();
    total = 0;
    for (int32_t i = 0; i < 16; i++) {
        dest.append(str);
        total += len;
        if (total + len <= INT32_MAX) {
            assertFalse("dest is not bogus", dest.isBogus());
        } else if (total <= INT32_MAX) {
            // Check that a string of exactly the maximum size works
            UnicodeString str2;
            int32_t remain = INT32_MAX - total;
            char16_t *buf2 = str2.getBuffer(remain);
            if (buf2 == nullptr) {
                // if somehow memory allocation fail, return the test
                return;
            }
            uprv_memset(buf2, 0x4e, remain * 2);
            str2.releaseBuffer(remain);
            dest.append(str2);
            total += remain;
            assertEquals("When a string of exactly the maximum size works", (int64_t)INT32_MAX, total);
            assertEquals("When a string of exactly the maximum size works", INT32_MAX, dest.length());
            assertFalse("dest is not bogus", dest.isBogus());

            // Check that a string size+1 goes bogus
            str2.truncate(1);
            dest.append(str2);
            total++;
            assertTrue("dest should be bogus", dest.isBogus());
        } else {
            assertTrue("dest should be bogus", dest.isBogus());
        }
    }
}