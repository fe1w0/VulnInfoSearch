void NumberFormatTest::Test20037_ScientificIntegerOverflow() {
    IcuTestErrorCode status(*this, "Test20037_ScientificIntegerOverflow");

    LocalPointer<NumberFormat> nf(NumberFormat::createInstance(status));
    if (U_FAILURE(status)) {
        dataerrln("Unable to create NumberFormat instance.");
        return;
    }
    Formattable result;

    // Test overflow of exponent
    nf->parse(u"1E-2147483648", result, status);
    StringPiece sp = result.getDecimalNumber(status);
    assertEquals(u"Should snap to zero",
                 u"0",
                 {sp.data(), sp.length(), US_INV});

    // Test edge case overflow of exponent
    result = Formattable();
    nf->parse(u"1E-2147483647E-1", result, status);
    sp = result.getDecimalNumber(status);
    assertEquals(u"Should not overflow and should parse only the first exponent",
                 u"1E-2147483647",
                 {sp.data(), sp.length(), US_INV});

    // Test edge case overflow of exponent
    result = Formattable();
    nf->parse(u".0003e-2147483644", result, status);
    sp = result.getDecimalNumber(status);
    assertEquals(u"Should not overflow",
                 u"3E-2147483648",
                 {sp.data(), sp.length(), US_INV});
}