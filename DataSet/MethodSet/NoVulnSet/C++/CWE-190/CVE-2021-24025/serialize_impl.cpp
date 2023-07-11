ALWAYS_INLINE String serialize_impl(const Variant& value,
                                    const SerializeOptions& opts) {
  switch (value.getType()) {
    case KindOfClass:
    case KindOfLazyClass:
    case KindOfPersistentString:
    case KindOfString: {
      auto const str =
        isStringType(value.getType()) ? value.getStringData() :
        isClassType(value.getType()) ? classToStringHelper(value.toClassVal()) :
        lazyClassToStringHelper(value.toLazyClassVal());
      auto const size = str->size();
      if (size >= RuntimeOption::MaxSerializedStringSize) {
        throw Exception("Size of serialized string (%ld) exceeds max", size);
      }
      StringBuffer sb;
      sb.append("s:");
      sb.append(size);
      sb.append(":\"");
      sb.append(str->data(), size);
      sb.append("\";");
      return sb.detach();
    }
    case KindOfResource:
      return s_Res;

    case KindOfUninit:
    case KindOfNull:
    case KindOfBoolean:
    case KindOfInt64:
    case KindOfFunc:
    case KindOfPersistentVec:
    case KindOfVec:
    case KindOfPersistentDict:
    case KindOfDict:
    case KindOfPersistentKeyset:
    case KindOfKeyset:
    case KindOfPersistentDArray:
    case KindOfDArray:
    case KindOfPersistentVArray:
    case KindOfVArray:
    case KindOfDouble:
    case KindOfObject:
    case KindOfClsMeth:
    case KindOfRClsMeth:
    case KindOfRFunc:
    case KindOfRecord:
      break;
  }
  VariableSerializer vs(VariableSerializer::Type::Serialize);
  if (opts.keepDVArrays)        vs.keepDVArrays();
  if (opts.forcePHPArrays)      vs.setForcePHPArrays();
  if (opts.warnOnHackArrays)    vs.setHackWarn();
  if (opts.warnOnPHPArrays)     vs.setPHPWarn();
  if (opts.ignoreLateInit)      vs.setIgnoreLateInit();
  if (opts.serializeProvenanceAndLegacy) vs.setSerializeProvenanceAndLegacy();
  // Keep the count so recursive calls to serialize() embed references properly.
  return vs.serialize(value, true, true);
}