CharString *Formattable::internalGetCharString(UErrorCode &status) {
    if(fDecimalStr == NULL) {
      if (fDecimalQuantity == NULL) {
        // No decimal number for the formattable yet.  Which means the value was
        // set directly by the user as an int, int64 or double.  If the value came
        // from parsing, or from the user setting a decimal number, fDecimalNum
        // would already be set.
        //
        LocalPointer<DecimalQuantity> dq(new DecimalQuantity(), status);
        if (U_FAILURE(status)) { return nullptr; }
        populateDecimalQuantity(*dq, status);
        if (U_FAILURE(status)) { return nullptr; }
        fDecimalQuantity = dq.orphan();
      }

      fDecimalStr = new CharString();
      if (fDecimalStr == NULL) {
        status = U_MEMORY_ALLOCATION_ERROR;
        return NULL;
      }
      // Older ICUs called uprv_decNumberToString here, which is not exactly the same as
      // DecimalQuantity::toScientificString(). The biggest difference is that uprv_decNumberToString does
      // not print scientific notation for magnitudes greater than -5 and smaller than some amount (+5?).
      if (fDecimalQuantity->isZero()) {
        fDecimalStr->append("0", -1, status);
      } else if (fDecimalQuantity->getMagnitude() != INT32_MIN && std::abs(fDecimalQuantity->getMagnitude()) < 5) {
        fDecimalStr->appendInvariantChars(fDecimalQuantity->toPlainString(), status);
      } else {
        fDecimalStr->appendInvariantChars(fDecimalQuantity->toScientificString(), status);
      }
    }
    return fDecimalStr;
}