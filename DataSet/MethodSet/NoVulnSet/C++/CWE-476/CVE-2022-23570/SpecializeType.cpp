StatusOr<FullTypeDef> SpecializeType(const AttrSlice& attrs,
                                     const OpDef& op_def) {
  FullTypeDef ft;
  ft.set_type_id(TFT_PRODUCT);

  for (int i = 0; i < op_def.output_arg_size(); i++) {
    auto* t = ft.add_args();

    *t = op_def.output_arg(i).experimental_full_type();

    // Resolve dependent types. The convention for op registrations is to use
    // attributes as type variables.
    // See https://www.tensorflow.org/guide/create_op#type_polymorphism.
    // Once the op signature can be defined entirely in FullType, this
    // convention can be deprecated.
    //
    // Note: While this code performs some basic verifications, it generally
    // assumes consistent op defs and attributes. If more complete
    // verifications are needed, they should be done by separately, and in a
    // way that can be reused for type inference.
    for (int j = 0; j < t->args_size(); j++) {
      auto* arg = t->mutable_args(i);
      if (arg->type_id() == TFT_VAR) {
        const auto* attr = attrs.Find(arg->s());
        if (attr == nullptr) {
          return Status(
              error::INVALID_ARGUMENT,
              absl::StrCat("Could not find an attribute for key ", arg->s()));
        }
        if (attr->value_case() == AttrValue::kList) {
          const auto& attr_list = attr->list();
          arg->set_type_id(TFT_PRODUCT);
          for (int i = 0; i < attr_list.type_size(); i++) {
            map_dtype_to_tensor(attr_list.type(i), arg->add_args());
          }

        } else if (attr->value_case() == AttrValue::kType) {
          map_dtype_to_tensor(attr->type(), arg);

        } else {
          return Status(error::UNIMPLEMENTED,
                        absl::StrCat("unknown attribute type",
                                     attrs.DebugString(), " key=", arg->s()));
        }

        arg->clear_s();
      }
    }
  }

  return ft;
}