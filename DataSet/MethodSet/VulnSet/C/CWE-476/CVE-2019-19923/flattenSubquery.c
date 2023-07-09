static int flattenSubquery(
  Parse *pParse,       /* Parsing context */
  Select *p,           /* The parent or outer SELECT statement */
  int iFrom,           /* Index in p->pSrc->a[] of the inner subquery */
  int isAgg            /* True if outer SELECT uses aggregate functions */
){
  const char *zSavedAuthContext = pParse->zAuthContext;
  Select *pParent;    /* Current UNION ALL term of the other query */
  Select *pSub;       /* The inner query or "subquery" */
  Select *pSub1;      /* Pointer to the rightmost select in sub-query */
  SrcList *pSrc;      /* The FROM clause of the outer query */
  SrcList *pSubSrc;   /* The FROM clause of the subquery */
  int iParent;        /* VDBE cursor number of the pSub result set temp table */
  int iNewParent = -1;/* Replacement table for iParent */
  int isLeftJoin = 0; /* True if pSub is the right side of a LEFT JOIN */    
  int i;              /* Loop counter */
  Expr *pWhere;                    /* The WHERE clause */
  struct SrcList_item *pSubitem;   /* The subquery */
  sqlite3 *db = pParse->db;

  /* Check to see if flattening is permitted.  Return 0 if not.
  */
  assert( p!=0 );
  assert( p->pPrior==0 );
  if( OptimizationDisabled(db, SQLITE_QueryFlattener) ) return 0;
  pSrc = p->pSrc;
  assert( pSrc && iFrom>=0 && iFrom<pSrc->nSrc );
  pSubitem = &pSrc->a[iFrom];
  iParent = pSubitem->iCursor;
  pSub = pSubitem->pSelect;
  assert( pSub!=0 );

#ifndef SQLITE_OMIT_WINDOWFUNC
  if( p->pWin || pSub->pWin ) return 0;                  /* Restriction (25) */
#endif

  pSubSrc = pSub->pSrc;
  assert( pSubSrc );
  /* Prior to version 3.1.2, when LIMIT and OFFSET had to be simple constants,
  ** not arbitrary expressions, we allowed some combining of LIMIT and OFFSET
  ** because they could be computed at compile-time.  But when LIMIT and OFFSET
  ** became arbitrary expressions, we were forced to add restrictions (13)
  ** and (14). */
  if( pSub->pLimit && p->pLimit ) return 0;              /* Restriction (13) */
  if( pSub->pLimit && pSub->pLimit->pRight ) return 0;   /* Restriction (14) */
  if( (p->selFlags & SF_Compound)!=0 && pSub->pLimit ){
    return 0;                                            /* Restriction (15) */
  }
  if( pSubSrc->nSrc==0 ) return 0;                       /* Restriction (7)  */
  if( pSub->selFlags & SF_Distinct ) return 0;           /* Restriction (4)  */
  if( pSub->pLimit && (pSrc->nSrc>1 || isAgg) ){
     return 0;         /* Restrictions (8)(9) */
  }
  if( p->pOrderBy && pSub->pOrderBy ){
     return 0;                                           /* Restriction (11) */
  }
  if( isAgg && pSub->pOrderBy ) return 0;                /* Restriction (16) */
  if( pSub->pLimit && p->pWhere ) return 0;              /* Restriction (19) */
  if( pSub->pLimit && (p->selFlags & SF_Distinct)!=0 ){
     return 0;         /* Restriction (21) */
  }
  if( pSub->selFlags & (SF_Recursive) ){
    return 0; /* Restrictions (22) */
  }

  /*
  ** If the subquery is the right operand of a LEFT JOIN, then the
  ** subquery may not be a join itself (3a). Example of why this is not
  ** allowed:
  **
  **         t1 LEFT OUTER JOIN (t2 JOIN t3)
  **
  ** If we flatten the above, we would get
  **
  **         (t1 LEFT OUTER JOIN t2) JOIN t3
  **
  ** which is not at all the same thing.
  **
  ** If the subquery is the right operand of a LEFT JOIN, then the outer
  ** query cannot be an aggregate. (3c)  This is an artifact of the way
  ** aggregates are processed - there is no mechanism to determine if
  ** the LEFT JOIN table should be all-NULL.
  **
  ** See also tickets #306, #350, and #3300.
  */
  if( (pSubitem->fg.jointype & JT_OUTER)!=0 ){
    isLeftJoin = 1;
    if( pSubSrc->nSrc>1 || isAgg || IsVirtual(pSubSrc->a[0].pTab) ){
      /*  (3a)             (3c)     (3b) */
      return 0;
    }
  }
#ifdef SQLITE_EXTRA_IFNULLROW
  else if( iFrom>0 && !isAgg ){
    /* Setting isLeftJoin to -1 causes OP_IfNullRow opcodes to be generated for
    ** every reference to any result column from subquery in a join, even
    ** though they are not necessary.  This will stress-test the OP_IfNullRow 
    ** opcode. */
    isLeftJoin = -1;
  }
#endif

  /* Restriction (17): If the sub-query is a compound SELECT, then it must
  ** use only the UNION ALL operator. And none of the simple select queries
  ** that make up the compound SELECT are allowed to be aggregate or distinct
  ** queries.
  */
  if( pSub->pPrior ){
    if( pSub->pOrderBy ){
      return 0;  /* Restriction (20) */
    }
    if( isAgg || (p->selFlags & SF_Distinct)!=0 || pSrc->nSrc!=1 ){
      return 0; /* (17d1), (17d2), or (17d3) */
    }
    for(pSub1=pSub; pSub1; pSub1=pSub1->pPrior){
      testcase( (pSub1->selFlags & (SF_Distinct|SF_Aggregate))==SF_Distinct );
      testcase( (pSub1->selFlags & (SF_Distinct|SF_Aggregate))==SF_Aggregate );
      assert( pSub->pSrc!=0 );
      assert( pSub->pEList->nExpr==pSub1->pEList->nExpr );
      if( (pSub1->selFlags & (SF_Distinct|SF_Aggregate))!=0    /* (17b) */
       || (pSub1->pPrior && pSub1->op!=TK_ALL)                 /* (17a) */
       || pSub1->pSrc->nSrc<1                                  /* (17c) */
      ){
        return 0;
      }
      testcase( pSub1->pSrc->nSrc>1 );
    }

    /* Restriction (18). */
    if( p->pOrderBy ){
      int ii;
      for(ii=0; ii<p->pOrderBy->nExpr; ii++){
        if( p->pOrderBy->a[ii].u.x.iOrderByCol==0 ) return 0;
      }
    }
  }

  /* Ex-restriction (23):
  ** The only way that the recursive part of a CTE can contain a compound
  ** subquery is for the subquery to be one term of a join.  But if the
  ** subquery is a join, then the flattening has already been stopped by
  ** restriction (17d3)
  */
  assert( (p->selFlags & SF_Recursive)==0 || pSub->pPrior==0 );

  /***** If we reach this point, flattening is permitted. *****/
  SELECTTRACE(1,pParse,p,("flatten %u.%p from term %d\n",
                   pSub->selId, pSub, iFrom));

  /* Authorize the subquery */
  pParse->zAuthContext = pSubitem->zName;
  TESTONLY(i =) sqlite3AuthCheck(pParse, SQLITE_SELECT, 0, 0, 0);
  testcase( i==SQLITE_DENY );
  pParse->zAuthContext = zSavedAuthContext;

  /* If the sub-query is a compound SELECT statement, then (by restrictions
  ** 17 and 18 above) it must be a UNION ALL and the parent query must 
  ** be of the form:
  **
  **     SELECT <expr-list> FROM (<sub-query>) <where-clause> 
  **
  ** followed by any ORDER BY, LIMIT and/or OFFSET clauses. This block
  ** creates N-1 copies of the parent query without any ORDER BY, LIMIT or 
  ** OFFSET clauses and joins them to the left-hand-side of the original
  ** using UNION ALL operators. In this case N is the number of simple
  ** select statements in the compound sub-query.
  **
  ** Example:
  **
  **     SELECT a+1 FROM (
  **        SELECT x FROM tab
  **        UNION ALL
  **        SELECT y FROM tab
  **        UNION ALL
  **        SELECT abs(z*2) FROM tab2
  **     ) WHERE a!=5 ORDER BY 1
  **
  ** Transformed into:
  **
  **     SELECT x+1 FROM tab WHERE x+1!=5
  **     UNION ALL
  **     SELECT y+1 FROM tab WHERE y+1!=5
  **     UNION ALL
  **     SELECT abs(z*2)+1 FROM tab2 WHERE abs(z*2)+1!=5
  **     ORDER BY 1
  **
  ** We call this the "compound-subquery flattening".
  */
  for(pSub=pSub->pPrior; pSub; pSub=pSub->pPrior){
    Select *pNew;
    ExprList *pOrderBy = p->pOrderBy;
    Expr *pLimit = p->pLimit;
    Select *pPrior = p->pPrior;
    p->pOrderBy = 0;
    p->pSrc = 0;
    p->pPrior = 0;
    p->pLimit = 0;
    pNew = sqlite3SelectDup(db, p, 0);
    p->pLimit = pLimit;
    p->pOrderBy = pOrderBy;
    p->pSrc = pSrc;
    p->op = TK_ALL;
    if( pNew==0 ){
      p->pPrior = pPrior;
    }else{
      pNew->pPrior = pPrior;
      if( pPrior ) pPrior->pNext = pNew;
      pNew->pNext = p;
      p->pPrior = pNew;
      SELECTTRACE(2,pParse,p,("compound-subquery flattener"
                              " creates %u as peer\n",pNew->selId));
    }
    if( db->mallocFailed ) return 1;
  }

  /* Begin flattening the iFrom-th entry of the FROM clause 
  ** in the outer query.
  */
  pSub = pSub1 = pSubitem->pSelect;

  /* Delete the transient table structure associated with the
  ** subquery
  */
  sqlite3DbFree(db, pSubitem->zDatabase);
  sqlite3DbFree(db, pSubitem->zName);
  sqlite3DbFree(db, pSubitem->zAlias);
  pSubitem->zDatabase = 0;
  pSubitem->zName = 0;
  pSubitem->zAlias = 0;
  pSubitem->pSelect = 0;

  /* Defer deleting the Table object associated with the
  ** subquery until code generation is
  ** complete, since there may still exist Expr.pTab entries that
  ** refer to the subquery even after flattening.  Ticket #3346.
  **
  ** pSubitem->pTab is always non-NULL by test restrictions and tests above.
  */
  if( ALWAYS(pSubitem->pTab!=0) ){
    Table *pTabToDel = pSubitem->pTab;
    if( pTabToDel->nTabRef==1 ){
      Parse *pToplevel = sqlite3ParseToplevel(pParse);
      pTabToDel->pNextZombie = pToplevel->pZombieTab;
      pToplevel->pZombieTab = pTabToDel;
    }else{
      pTabToDel->nTabRef--;
    }
    pSubitem->pTab = 0;
  }

  /* The following loop runs once for each term in a compound-subquery
  ** flattening (as described above).  If we are doing a different kind
  ** of flattening - a flattening other than a compound-subquery flattening -
  ** then this loop only runs once.
  **
  ** This loop moves all of the FROM elements of the subquery into the
  ** the FROM clause of the outer query.  Before doing this, remember
  ** the cursor number for the original outer query FROM element in
  ** iParent.  The iParent cursor will never be used.  Subsequent code
  ** will scan expressions looking for iParent references and replace
  ** those references with expressions that resolve to the subquery FROM
  ** elements we are now copying in.
  */
  for(pParent=p; pParent; pParent=pParent->pPrior, pSub=pSub->pPrior){
    int nSubSrc;
    u8 jointype = 0;
    assert( pSub!=0 );
    pSubSrc = pSub->pSrc;     /* FROM clause of subquery */
    nSubSrc = pSubSrc->nSrc;  /* Number of terms in subquery FROM clause */
    pSrc = pParent->pSrc;     /* FROM clause of the outer query */

    if( pSrc ){
      assert( pParent==p );  /* First time through the loop */
      jointype = pSubitem->fg.jointype;
    }else{
      assert( pParent!=p );  /* 2nd and subsequent times through the loop */
      pSrc = sqlite3SrcListAppend(pParse, 0, 0, 0);
      if( pSrc==0 ) break;
      pParent->pSrc = pSrc;
    }

    /* The subquery uses a single slot of the FROM clause of the outer
    ** query.  If the subquery has more than one element in its FROM clause,
    ** then expand the outer query to make space for it to hold all elements
    ** of the subquery.
    **
    ** Example:
    **
    **    SELECT * FROM tabA, (SELECT * FROM sub1, sub2), tabB;
    **
    ** The outer query has 3 slots in its FROM clause.  One slot of the
    ** outer query (the middle slot) is used by the subquery.  The next
    ** block of code will expand the outer query FROM clause to 4 slots.
    ** The middle slot is expanded to two slots in order to make space
    ** for the two elements in the FROM clause of the subquery.
    */
    if( nSubSrc>1 ){
      pSrc = sqlite3SrcListEnlarge(pParse, pSrc, nSubSrc-1,iFrom+1);
      if( pSrc==0 ) break;
      pParent->pSrc = pSrc;
    }

    /* Transfer the FROM clause terms from the subquery into the
    ** outer query.
    */
    for(i=0; i<nSubSrc; i++){
      sqlite3IdListDelete(db, pSrc->a[i+iFrom].pUsing);
      assert( pSrc->a[i+iFrom].fg.isTabFunc==0 );
      pSrc->a[i+iFrom] = pSubSrc->a[i];
      iNewParent = pSubSrc->a[i].iCursor;
      memset(&pSubSrc->a[i], 0, sizeof(pSubSrc->a[i]));
    }
    pSrc->a[iFrom].fg.jointype = jointype;
  
    /* Now begin substituting subquery result set expressions for 
    ** references to the iParent in the outer query.
    ** 
    ** Example:
    **
    **   SELECT a+5, b*10 FROM (SELECT x*3 AS a, y+10 AS b FROM t1) WHERE a>b;
    **   \                     \_____________ subquery __________/          /
    **    \_____________________ outer query ______________________________/
    **
    ** We look at every expression in the outer query and every place we see
    ** "a" we substitute "x*3" and every place we see "b" we substitute "y+10".
    */
    if( pSub->pOrderBy ){
      /* At this point, any non-zero iOrderByCol values indicate that the
      ** ORDER BY column expression is identical to the iOrderByCol'th
      ** expression returned by SELECT statement pSub. Since these values
      ** do not necessarily correspond to columns in SELECT statement pParent,
      ** zero them before transfering the ORDER BY clause.
      **
      ** Not doing this may cause an error if a subsequent call to this
      ** function attempts to flatten a compound sub-query into pParent
      ** (the only way this can happen is if the compound sub-query is
      ** currently part of pSub->pSrc). See ticket [d11a6e908f].  */
      ExprList *pOrderBy = pSub->pOrderBy;
      for(i=0; i<pOrderBy->nExpr; i++){
        pOrderBy->a[i].u.x.iOrderByCol = 0;
      }
      assert( pParent->pOrderBy==0 );
      pParent->pOrderBy = pOrderBy;
      pSub->pOrderBy = 0;
    }
    pWhere = pSub->pWhere;
    pSub->pWhere = 0;
    if( isLeftJoin>0 ){
      sqlite3SetJoinExpr(pWhere, iNewParent);
    }
    pParent->pWhere = sqlite3ExprAnd(pParse, pWhere, pParent->pWhere);
    if( db->mallocFailed==0 ){
      SubstContext x;
      x.pParse = pParse;
      x.iTable = iParent;
      x.iNewTable = iNewParent;
      x.isLeftJoin = isLeftJoin;
      x.pEList = pSub->pEList;
      substSelect(&x, pParent, 0);
    }
  
    /* The flattened query is a compound if either the inner or the
    ** outer query is a compound. */
    pParent->selFlags |= pSub->selFlags & SF_Compound;
    assert( (pSub->selFlags & SF_Distinct)==0 ); /* restriction (17b) */
  
    /*
    ** SELECT ... FROM (SELECT ... LIMIT a OFFSET b) LIMIT x OFFSET y;
    **
    ** One is tempted to try to add a and b to combine the limits.  But this
    ** does not work if either limit is negative.
    */
    if( pSub->pLimit ){
      pParent->pLimit = pSub->pLimit;
      pSub->pLimit = 0;
    }
  }

  /* Finially, delete what is left of the subquery and return
  ** success.
  */
  sqlite3SelectDelete(db, pSub1);

#if SELECTTRACE_ENABLED
  if( sqlite3SelectTrace & 0x100 ){
    SELECTTRACE(0x100,pParse,p,("After flattening:\n"));
    sqlite3TreeViewSelect(0, p, 0);
  }
#endif

  return 1;
}