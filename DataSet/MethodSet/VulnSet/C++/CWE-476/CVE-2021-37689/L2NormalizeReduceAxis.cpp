bool L2NormalizeReduceAxis(Value sq_op, DenseElementsAttr axis) {
  if (sq_op.getType().cast<ShapedType>().getRank() - 1 ==
          *axis.getValues<int>().begin() ||
      *axis.getValues<int>().begin() == -1) {
    return true;
  }
  if (sq_op.getType().cast<ShapedType>().getRank() != axis.getNumElements()) {
    return false;
  }
  auto shape = sq_op.getType().cast<ShapedType>();
  SmallVector<int, 4> elems{axis.getValues<int>().begin(),
                            axis.getValues<int>().end()};
  for (int i = 0; i < shape.getRank(); ++i) {
    if (i != elems[i]) return false;
  }
  return true;
}