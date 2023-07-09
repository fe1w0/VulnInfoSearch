static int whereIndexExprTransColumn(Walker *p, Expr *pExpr){
  if( pExpr->op==TK_COLUMN ){
    IdxExprTrans *pX = p->u.pIdxTrans;
    if( pExpr->iTable==pX->iTabCur && pExpr->iColumn==pX->iTabCol ){
      pExpr->iTable = pX->iIdxCur;
      pExpr->iColumn = pX->iIdxCol;
      pExpr->y.pTab = 0;
    }
  }
  return WRC_Continue;
}