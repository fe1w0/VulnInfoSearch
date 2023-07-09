static ExprList *exprListAppendList(
  Parse *pParse,          /* Parsing context */
  ExprList *pList,        /* List to which to append. Might be NULL */
  ExprList *pAppend,      /* List of values to append. Might be NULL */
  int bIntToNull
){
  if( pAppend ){
    int i;
    int nInit = pList ? pList->nExpr : 0;
    for(i=0; i<pAppend->nExpr; i++){
      Expr *pDup = sqlite3ExprDup(pParse->db, pAppend->a[i].pExpr, 0);
      if( bIntToNull && pDup && pDup->op==TK_INTEGER ){
        pDup->op = TK_NULL;
        pDup->flags &= ~(EP_IntValue|EP_IsTrue|EP_IsFalse);
      }
      pList = sqlite3ExprListAppend(pParse, pList, pDup);
      if( pList ) pList->a[nInit+i].sortFlags = pAppend->a[i].sortFlags;
    }
  }
  return pList;
}