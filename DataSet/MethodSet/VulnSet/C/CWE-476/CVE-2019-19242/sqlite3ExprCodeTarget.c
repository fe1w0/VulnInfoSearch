int sqlite3ExprCodeTarget(Parse *pParse, Expr *pExpr, int target){
  Vdbe *v = pParse->pVdbe;  /* The VM under construction */
  int op;                   /* The opcode being coded */
  int inReg = target;       /* Results stored in register inReg */
  int regFree1 = 0;         /* If non-zero free this temporary register */
  int regFree2 = 0;         /* If non-zero free this temporary register */
  int r1, r2;               /* Various register numbers */
  Expr tempX;               /* Temporary expression node */
  int p5 = 0;

  assert( target>0 && target<=pParse->nMem );
  if( v==0 ){
    assert( pParse->db->mallocFailed );
    return 0;
  }

expr_code_doover:
  if( pExpr==0 ){
    op = TK_NULL;
  }else{
    op = pExpr->op;
  }
  switch( op ){
    case TK_AGG_COLUMN: {
      AggInfo *pAggInfo = pExpr->pAggInfo;
      struct AggInfo_col *pCol = &pAggInfo->aCol[pExpr->iAgg];
      if( !pAggInfo->directMode ){
        assert( pCol->iMem>0 );
        return pCol->iMem;
      }else if( pAggInfo->useSortingIdx ){
        sqlite3VdbeAddOp3(v, OP_Column, pAggInfo->sortingIdxPTab,
                              pCol->iSorterColumn, target);
        return target;
      }
      /* Otherwise, fall thru into the TK_COLUMN case */
    }
    case TK_COLUMN: {
      int iTab = pExpr->iTable;
      if( ExprHasProperty(pExpr, EP_FixedCol) ){
        /* This COLUMN expression is really a constant due to WHERE clause
        ** constraints, and that constant is coded by the pExpr->pLeft
        ** expresssion.  However, make sure the constant has the correct
        ** datatype by applying the Affinity of the table column to the
        ** constant.
        */
        int iReg = sqlite3ExprCodeTarget(pParse, pExpr->pLeft,target);
        int aff = sqlite3TableColumnAffinity(pExpr->y.pTab, pExpr->iColumn);
        if( aff>SQLITE_AFF_BLOB ){
          static const char zAff[] = "B\000C\000D\000E";
          assert( SQLITE_AFF_BLOB=='A' );
          assert( SQLITE_AFF_TEXT=='B' );
          if( iReg!=target ){
            sqlite3VdbeAddOp2(v, OP_SCopy, iReg, target);
            iReg = target;
          }
          sqlite3VdbeAddOp4(v, OP_Affinity, iReg, 1, 0,
                            &zAff[(aff-'B')*2], P4_STATIC);
        }
        return iReg;
      }
      if( iTab<0 ){
        if( pParse->iSelfTab<0 ){
          /* Other columns in the same row for CHECK constraints or
          ** generated columns or for inserting into partial index.
          ** The row is unpacked into registers beginning at
          ** 0-(pParse->iSelfTab).  The rowid (if any) is in a register
          ** immediately prior to the first column.
          */
          Column *pCol;
          Table *pTab = pExpr->y.pTab;
          int iSrc;
          int iCol = pExpr->iColumn;
          assert( pTab!=0 );
          assert( iCol>=XN_ROWID );
          assert( iCol<pExpr->y.pTab->nCol );
          if( iCol<0 ){
            return -1-pParse->iSelfTab;
          }
          pCol = pTab->aCol + iCol;
          testcase( iCol!=sqlite3TableColumnToStorage(pTab,iCol) );
          iSrc = sqlite3TableColumnToStorage(pTab, iCol) - pParse->iSelfTab;
#ifndef SQLITE_OMIT_GENERATED_COLUMNS
          if( pCol->colFlags & COLFLAG_GENERATED ){
            if( pCol->colFlags & COLFLAG_BUSY ){
              sqlite3ErrorMsg(pParse, "generated column loop on \"%s\"",
                              pCol->zName);
              return 0;
            }
            pCol->colFlags |= COLFLAG_BUSY;
            if( pCol->colFlags & COLFLAG_NOTAVAIL ){
              sqlite3ExprCodeGeneratedColumn(pParse, pCol, iSrc);
            }
            pCol->colFlags &= ~(COLFLAG_BUSY|COLFLAG_NOTAVAIL);
            return iSrc;
          }else
#endif /* SQLITE_OMIT_GENERATED_COLUMNS */
          if( pCol->affinity==SQLITE_AFF_REAL ){
            sqlite3VdbeAddOp2(v, OP_SCopy, iSrc, target);
            sqlite3VdbeAddOp1(v, OP_RealAffinity, target);
            return target;
          }else{
            return iSrc;
          }
        }else{
          /* Coding an expression that is part of an index where column names
          ** in the index refer to the table to which the index belongs */
          iTab = pParse->iSelfTab - 1;
        }
      }
      return sqlite3ExprCodeGetColumn(pParse, pExpr->y.pTab,
                               pExpr->iColumn, iTab, target,
                               pExpr->op2);
    }
    case TK_INTEGER: {
      codeInteger(pParse, pExpr, 0, target);
      return target;
    }
    case TK_TRUEFALSE: {
      sqlite3VdbeAddOp2(v, OP_Integer, sqlite3ExprTruthValue(pExpr), target);
      return target;
    }
#ifndef SQLITE_OMIT_FLOATING_POINT
    case TK_FLOAT: {
      assert( !ExprHasProperty(pExpr, EP_IntValue) );
      codeReal(v, pExpr->u.zToken, 0, target);
      return target;
    }
#endif
    case TK_STRING: {
      assert( !ExprHasProperty(pExpr, EP_IntValue) );
      sqlite3VdbeLoadString(v, target, pExpr->u.zToken);
      return target;
    }
    case TK_NULL: {
      sqlite3VdbeAddOp2(v, OP_Null, 0, target);
      return target;
    }
#ifndef SQLITE_OMIT_BLOB_LITERAL
    case TK_BLOB: {
      int n;
      const char *z;
      char *zBlob;
      assert( !ExprHasProperty(pExpr, EP_IntValue) );
      assert( pExpr->u.zToken[0]=='x' || pExpr->u.zToken[0]=='X' );
      assert( pExpr->u.zToken[1]=='\'' );
      z = &pExpr->u.zToken[2];
      n = sqlite3Strlen30(z) - 1;
      assert( z[n]=='\'' );
      zBlob = sqlite3HexToBlob(sqlite3VdbeDb(v), z, n);
      sqlite3VdbeAddOp4(v, OP_Blob, n/2, target, 0, zBlob, P4_DYNAMIC);
      return target;
    }
#endif
    case TK_VARIABLE: {
      assert( !ExprHasProperty(pExpr, EP_IntValue) );
      assert( pExpr->u.zToken!=0 );
      assert( pExpr->u.zToken[0]!=0 );
      sqlite3VdbeAddOp2(v, OP_Variable, pExpr->iColumn, target);
      if( pExpr->u.zToken[1]!=0 ){
        const char *z = sqlite3VListNumToName(pParse->pVList, pExpr->iColumn);
        assert( pExpr->u.zToken[0]=='?' || strcmp(pExpr->u.zToken, z)==0 );
        pParse->pVList[0] = 0; /* Indicate VList may no longer be enlarged */
        sqlite3VdbeAppendP4(v, (char*)z, P4_STATIC);
      }
      return target;
    }
    case TK_REGISTER: {
      return pExpr->iTable;
    }
#ifndef SQLITE_OMIT_CAST
    case TK_CAST: {
      /* Expressions of the form:   CAST(pLeft AS token) */
      inReg = sqlite3ExprCodeTarget(pParse, pExpr->pLeft, target);
      if( inReg!=target ){
        sqlite3VdbeAddOp2(v, OP_SCopy, inReg, target);
        inReg = target;
      }
      sqlite3VdbeAddOp2(v, OP_Cast, target,
                        sqlite3AffinityType(pExpr->u.zToken, 0));
      return inReg;
    }
#endif /* SQLITE_OMIT_CAST */
    case TK_IS:
    case TK_ISNOT:
      op = (op==TK_IS) ? TK_EQ : TK_NE;
      p5 = SQLITE_NULLEQ;
      /* fall-through */
    case TK_LT:
    case TK_LE:
    case TK_GT:
    case TK_GE:
    case TK_NE:
    case TK_EQ: {
      Expr *pLeft = pExpr->pLeft;
      if( sqlite3ExprIsVector(pLeft) ){
        codeVectorCompare(pParse, pExpr, target, op, p5);
      }else{
        r1 = sqlite3ExprCodeTemp(pParse, pLeft, &regFree1);
        r2 = sqlite3ExprCodeTemp(pParse, pExpr->pRight, &regFree2);
        codeCompare(pParse, pLeft, pExpr->pRight, op,
            r1, r2, inReg, SQLITE_STOREP2 | p5,
            ExprHasProperty(pExpr,EP_Commuted));
        assert(TK_LT==OP_Lt); testcase(op==OP_Lt); VdbeCoverageIf(v,op==OP_Lt);
        assert(TK_LE==OP_Le); testcase(op==OP_Le); VdbeCoverageIf(v,op==OP_Le);
        assert(TK_GT==OP_Gt); testcase(op==OP_Gt); VdbeCoverageIf(v,op==OP_Gt);
        assert(TK_GE==OP_Ge); testcase(op==OP_Ge); VdbeCoverageIf(v,op==OP_Ge);
        assert(TK_EQ==OP_Eq); testcase(op==OP_Eq); VdbeCoverageIf(v,op==OP_Eq);
        assert(TK_NE==OP_Ne); testcase(op==OP_Ne); VdbeCoverageIf(v,op==OP_Ne);
        testcase( regFree1==0 );
        testcase( regFree2==0 );
      }
      break;
    }
    case TK_AND:
    case TK_OR:
    case TK_PLUS:
    case TK_STAR:
    case TK_MINUS:
    case TK_REM:
    case TK_BITAND:
    case TK_BITOR:
    case TK_SLASH:
    case TK_LSHIFT:
    case TK_RSHIFT: 
    case TK_CONCAT: {
      assert( TK_AND==OP_And );            testcase( op==TK_AND );
      assert( TK_OR==OP_Or );              testcase( op==TK_OR );
      assert( TK_PLUS==OP_Add );           testcase( op==TK_PLUS );
      assert( TK_MINUS==OP_Subtract );     testcase( op==TK_MINUS );
      assert( TK_REM==OP_Remainder );      testcase( op==TK_REM );
      assert( TK_BITAND==OP_BitAnd );      testcase( op==TK_BITAND );
      assert( TK_BITOR==OP_BitOr );        testcase( op==TK_BITOR );
      assert( TK_SLASH==OP_Divide );       testcase( op==TK_SLASH );
      assert( TK_LSHIFT==OP_ShiftLeft );   testcase( op==TK_LSHIFT );
      assert( TK_RSHIFT==OP_ShiftRight );  testcase( op==TK_RSHIFT );
      assert( TK_CONCAT==OP_Concat );      testcase( op==TK_CONCAT );
      r1 = sqlite3ExprCodeTemp(pParse, pExpr->pLeft, &regFree1);
      r2 = sqlite3ExprCodeTemp(pParse, pExpr->pRight, &regFree2);
      sqlite3VdbeAddOp3(v, op, r2, r1, target);
      testcase( regFree1==0 );
      testcase( regFree2==0 );
      break;
    }
    case TK_UMINUS: {
      Expr *pLeft = pExpr->pLeft;
      assert( pLeft );
      if( pLeft->op==TK_INTEGER ){
        codeInteger(pParse, pLeft, 1, target);
        return target;
#ifndef SQLITE_OMIT_FLOATING_POINT
      }else if( pLeft->op==TK_FLOAT ){
        assert( !ExprHasProperty(pExpr, EP_IntValue) );
        codeReal(v, pLeft->u.zToken, 1, target);
        return target;
#endif
      }else{
        tempX.op = TK_INTEGER;
        tempX.flags = EP_IntValue|EP_TokenOnly;
        tempX.u.iValue = 0;
        r1 = sqlite3ExprCodeTemp(pParse, &tempX, &regFree1);
        r2 = sqlite3ExprCodeTemp(pParse, pExpr->pLeft, &regFree2);
        sqlite3VdbeAddOp3(v, OP_Subtract, r2, r1, target);
        testcase( regFree2==0 );
      }
      break;
    }
    case TK_BITNOT:
    case TK_NOT: {
      assert( TK_BITNOT==OP_BitNot );   testcase( op==TK_BITNOT );
      assert( TK_NOT==OP_Not );         testcase( op==TK_NOT );
      r1 = sqlite3ExprCodeTemp(pParse, pExpr->pLeft, &regFree1);
      testcase( regFree1==0 );
      sqlite3VdbeAddOp2(v, op, r1, inReg);
      break;
    }
    case TK_TRUTH: {
      int isTrue;    /* IS TRUE or IS NOT TRUE */
      int bNormal;   /* IS TRUE or IS FALSE */
      r1 = sqlite3ExprCodeTemp(pParse, pExpr->pLeft, &regFree1);
      testcase( regFree1==0 );
      isTrue = sqlite3ExprTruthValue(pExpr->pRight);
      bNormal = pExpr->op2==TK_IS;
      testcase( isTrue && bNormal);
      testcase( !isTrue && bNormal);
      sqlite3VdbeAddOp4Int(v, OP_IsTrue, r1, inReg, !isTrue, isTrue ^ bNormal);
      break;
    }
    case TK_ISNULL:
    case TK_NOTNULL: {
      int addr;
      assert( TK_ISNULL==OP_IsNull );   testcase( op==TK_ISNULL );
      assert( TK_NOTNULL==OP_NotNull ); testcase( op==TK_NOTNULL );
      sqlite3VdbeAddOp2(v, OP_Integer, 1, target);
      r1 = sqlite3ExprCodeTemp(pParse, pExpr->pLeft, &regFree1);
      testcase( regFree1==0 );
      addr = sqlite3VdbeAddOp1(v, op, r1);
      VdbeCoverageIf(v, op==TK_ISNULL);
      VdbeCoverageIf(v, op==TK_NOTNULL);
      sqlite3VdbeAddOp2(v, OP_Integer, 0, target);
      sqlite3VdbeJumpHere(v, addr);
      break;
    }
    case TK_AGG_FUNCTION: {
      AggInfo *pInfo = pExpr->pAggInfo;
      if( pInfo==0 ){
        assert( !ExprHasProperty(pExpr, EP_IntValue) );
        sqlite3ErrorMsg(pParse, "misuse of aggregate: %s()", pExpr->u.zToken);
      }else{
        return pInfo->aFunc[pExpr->iAgg].iMem;
      }
      break;
    }
    case TK_FUNCTION: {
      ExprList *pFarg;       /* List of function arguments */
      int nFarg;             /* Number of function arguments */
      FuncDef *pDef;         /* The function definition object */
      const char *zId;       /* The function name */
      u32 constMask = 0;     /* Mask of function arguments that are constant */
      int i;                 /* Loop counter */
      sqlite3 *db = pParse->db;  /* The database connection */
      u8 enc = ENC(db);      /* The text encoding used by this database */
      CollSeq *pColl = 0;    /* A collating sequence */

#ifndef SQLITE_OMIT_WINDOWFUNC
      if( ExprHasProperty(pExpr, EP_WinFunc) ){
        return pExpr->y.pWin->regResult;
      }
#endif

      if( ConstFactorOk(pParse) && sqlite3ExprIsConstantNotJoin(pExpr) ){
        /* SQL functions can be expensive. So try to move constant functions
        ** out of the inner loop, even if that means an extra OP_Copy. */
        return sqlite3ExprCodeAtInit(pParse, pExpr, -1);
      }
      assert( !ExprHasProperty(pExpr, EP_xIsSelect) );
      if( ExprHasProperty(pExpr, EP_TokenOnly) ){
        pFarg = 0;
      }else{
        pFarg = pExpr->x.pList;
      }
      nFarg = pFarg ? pFarg->nExpr : 0;
      assert( !ExprHasProperty(pExpr, EP_IntValue) );
      zId = pExpr->u.zToken;
      pDef = sqlite3FindFunction(db, zId, nFarg, enc, 0);
#ifdef SQLITE_ENABLE_UNKNOWN_SQL_FUNCTION
      if( pDef==0 && pParse->explain ){
        pDef = sqlite3FindFunction(db, "unknown", nFarg, enc, 0);
      }
#endif
      if( pDef==0 || pDef->xFinalize!=0 ){
        sqlite3ErrorMsg(pParse, "unknown function: %s()", zId);
        break;
      }

      /* Attempt a direct implementation of the built-in COALESCE() and
      ** IFNULL() functions.  This avoids unnecessary evaluation of
      ** arguments past the first non-NULL argument.
      */
      if( pDef->funcFlags & SQLITE_FUNC_COALESCE ){
        int endCoalesce = sqlite3VdbeMakeLabel(pParse);
        assert( nFarg>=2 );
        sqlite3ExprCode(pParse, pFarg->a[0].pExpr, target);
        for(i=1; i<nFarg; i++){
          sqlite3VdbeAddOp2(v, OP_NotNull, target, endCoalesce);
          VdbeCoverage(v);
          sqlite3ExprCode(pParse, pFarg->a[i].pExpr, target);
        }
        sqlite3VdbeResolveLabel(v, endCoalesce);
        break;
      }

      /* The UNLIKELY() function is a no-op.  The result is the value
      ** of the first argument.
      */
      if( pDef->funcFlags & SQLITE_FUNC_UNLIKELY ){
        assert( nFarg>=1 );
        return sqlite3ExprCodeTarget(pParse, pFarg->a[0].pExpr, target);
      }

#ifdef SQLITE_DEBUG
      /* The AFFINITY() function evaluates to a string that describes
      ** the type affinity of the argument.  This is used for testing of
      ** the SQLite type logic.
      */
      if( pDef->funcFlags & SQLITE_FUNC_AFFINITY ){
        const char *azAff[] = { "blob", "text", "numeric", "integer", "real" };
        char aff;
        assert( nFarg==1 );
        aff = sqlite3ExprAffinity(pFarg->a[0].pExpr);
        sqlite3VdbeLoadString(v, target, 
                (aff<=SQLITE_AFF_NONE) ? "none" : azAff[aff-SQLITE_AFF_BLOB]);
        return target;
      }
#endif

      for(i=0; i<nFarg; i++){
        if( i<32 && sqlite3ExprIsConstant(pFarg->a[i].pExpr) ){
          testcase( i==31 );
          constMask |= MASKBIT32(i);
        }
        if( (pDef->funcFlags & SQLITE_FUNC_NEEDCOLL)!=0 && !pColl ){
          pColl = sqlite3ExprCollSeq(pParse, pFarg->a[i].pExpr);
        }
      }
      if( pFarg ){
        if( constMask ){
          r1 = pParse->nMem+1;
          pParse->nMem += nFarg;
        }else{
          r1 = sqlite3GetTempRange(pParse, nFarg);
        }

        /* For length() and typeof() functions with a column argument,
        ** set the P5 parameter to the OP_Column opcode to OPFLAG_LENGTHARG
        ** or OPFLAG_TYPEOFARG respectively, to avoid unnecessary data
        ** loading.
        */
        if( (pDef->funcFlags & (SQLITE_FUNC_LENGTH|SQLITE_FUNC_TYPEOF))!=0 ){
          u8 exprOp;
          assert( nFarg==1 );
          assert( pFarg->a[0].pExpr!=0 );
          exprOp = pFarg->a[0].pExpr->op;
          if( exprOp==TK_COLUMN || exprOp==TK_AGG_COLUMN ){
            assert( SQLITE_FUNC_LENGTH==OPFLAG_LENGTHARG );
            assert( SQLITE_FUNC_TYPEOF==OPFLAG_TYPEOFARG );
            testcase( pDef->funcFlags & OPFLAG_LENGTHARG );
            pFarg->a[0].pExpr->op2 = 
                  pDef->funcFlags & (OPFLAG_LENGTHARG|OPFLAG_TYPEOFARG);
          }
        }

        sqlite3ExprCodeExprList(pParse, pFarg, r1, 0,
                                SQLITE_ECEL_DUP|SQLITE_ECEL_FACTOR);
      }else{
        r1 = 0;
      }
#ifndef SQLITE_OMIT_VIRTUALTABLE
      /* Possibly overload the function if the first argument is
      ** a virtual table column.
      **
      ** For infix functions (LIKE, GLOB, REGEXP, and MATCH) use the
      ** second argument, not the first, as the argument to test to
      ** see if it is a column in a virtual table.  This is done because
      ** the left operand of infix functions (the operand we want to
      ** control overloading) ends up as the second argument to the
      ** function.  The expression "A glob B" is equivalent to 
      ** "glob(B,A).  We want to use the A in "A glob B" to test
      ** for function overloading.  But we use the B term in "glob(B,A)".
      */
      if( nFarg>=2 && ExprHasProperty(pExpr, EP_InfixFunc) ){
        pDef = sqlite3VtabOverloadFunction(db, pDef, nFarg, pFarg->a[1].pExpr);
      }else if( nFarg>0 ){
        pDef = sqlite3VtabOverloadFunction(db, pDef, nFarg, pFarg->a[0].pExpr);
      }
#endif
      if( pDef->funcFlags & SQLITE_FUNC_NEEDCOLL ){
        if( !pColl ) pColl = db->pDfltColl; 
        sqlite3VdbeAddOp4(v, OP_CollSeq, 0, 0, 0, (char *)pColl, P4_COLLSEQ);
      }
#ifdef SQLITE_ENABLE_OFFSET_SQL_FUNC
      if( pDef->funcFlags & SQLITE_FUNC_OFFSET ){
        Expr *pArg = pFarg->a[0].pExpr;
        if( pArg->op==TK_COLUMN ){
          sqlite3VdbeAddOp3(v, OP_Offset, pArg->iTable, pArg->iColumn, target);
        }else{
          sqlite3VdbeAddOp2(v, OP_Null, 0, target);
        }
      }else
#endif
      {
        sqlite3VdbeAddFunctionCall(pParse, constMask, r1, target, nFarg,
                                   pDef, pExpr->op2);
      }
      if( nFarg && constMask==0 ){
        sqlite3ReleaseTempRange(pParse, r1, nFarg);
      }
      return target;
    }
#ifndef SQLITE_OMIT_SUBQUERY
    case TK_EXISTS:
    case TK_SELECT: {
      int nCol;
      testcase( op==TK_EXISTS );
      testcase( op==TK_SELECT );
      if( op==TK_SELECT && (nCol = pExpr->x.pSelect->pEList->nExpr)!=1 ){
        sqlite3SubselectError(pParse, nCol, 1);
      }else{
        return sqlite3CodeSubselect(pParse, pExpr);
      }
      break;
    }
    case TK_SELECT_COLUMN: {
      int n;
      if( pExpr->pLeft->iTable==0 ){
        pExpr->pLeft->iTable = sqlite3CodeSubselect(pParse, pExpr->pLeft);
      }
      assert( pExpr->iTable==0 || pExpr->pLeft->op==TK_SELECT );
      if( pExpr->iTable!=0
       && pExpr->iTable!=(n = sqlite3ExprVectorSize(pExpr->pLeft))
      ){
        sqlite3ErrorMsg(pParse, "%d columns assigned %d values",
                                pExpr->iTable, n);
      }
      return pExpr->pLeft->iTable + pExpr->iColumn;
    }
    case TK_IN: {
      int destIfFalse = sqlite3VdbeMakeLabel(pParse);
      int destIfNull = sqlite3VdbeMakeLabel(pParse);
      sqlite3VdbeAddOp2(v, OP_Null, 0, target);
      sqlite3ExprCodeIN(pParse, pExpr, destIfFalse, destIfNull);
      sqlite3VdbeAddOp2(v, OP_Integer, 1, target);
      sqlite3VdbeResolveLabel(v, destIfFalse);
      sqlite3VdbeAddOp2(v, OP_AddImm, target, 0);
      sqlite3VdbeResolveLabel(v, destIfNull);
      return target;
    }
#endif /* SQLITE_OMIT_SUBQUERY */


    /*
    **    x BETWEEN y AND z
    **
    ** This is equivalent to
    **
    **    x>=y AND x<=z
    **
    ** X is stored in pExpr->pLeft.
    ** Y is stored in pExpr->pList->a[0].pExpr.
    ** Z is stored in pExpr->pList->a[1].pExpr.
    */
    case TK_BETWEEN: {
      exprCodeBetween(pParse, pExpr, target, 0, 0);
      return target;
    }
    case TK_SPAN:
    case TK_COLLATE: 
    case TK_UPLUS: {
      pExpr = pExpr->pLeft;
      goto expr_code_doover; /* 2018-04-28: Prevent deep recursion. OSSFuzz. */
    }

    case TK_TRIGGER: {
      /* If the opcode is TK_TRIGGER, then the expression is a reference
      ** to a column in the new.* or old.* pseudo-tables available to
      ** trigger programs. In this case Expr.iTable is set to 1 for the
      ** new.* pseudo-table, or 0 for the old.* pseudo-table. Expr.iColumn
      ** is set to the column of the pseudo-table to read, or to -1 to
      ** read the rowid field.
      **
      ** The expression is implemented using an OP_Param opcode. The p1
      ** parameter is set to 0 for an old.rowid reference, or to (i+1)
      ** to reference another column of the old.* pseudo-table, where 
      ** i is the index of the column. For a new.rowid reference, p1 is
      ** set to (n+1), where n is the number of columns in each pseudo-table.
      ** For a reference to any other column in the new.* pseudo-table, p1
      ** is set to (n+2+i), where n and i are as defined previously. For
      ** example, if the table on which triggers are being fired is
      ** declared as:
      **
      **   CREATE TABLE t1(a, b);
      **
      ** Then p1 is interpreted as follows:
      **
      **   p1==0   ->    old.rowid     p1==3   ->    new.rowid
      **   p1==1   ->    old.a         p1==4   ->    new.a
      **   p1==2   ->    old.b         p1==5   ->    new.b       
      */
      Table *pTab = pExpr->y.pTab;
      int iCol = pExpr->iColumn;
      int p1 = pExpr->iTable * (pTab->nCol+1) + 1 
                     + (iCol>=0 ? sqlite3TableColumnToStorage(pTab, iCol) : -1);

      assert( pExpr->iTable==0 || pExpr->iTable==1 );
      assert( iCol>=-1 && iCol<pTab->nCol );
      assert( pTab->iPKey<0 || iCol!=pTab->iPKey );
      assert( p1>=0 && p1<(pTab->nCol*2+2) );

      sqlite3VdbeAddOp2(v, OP_Param, p1, target);
      VdbeComment((v, "r[%d]=%s.%s", target,
        (pExpr->iTable ? "new" : "old"),
        (pExpr->iColumn<0 ? "rowid" : pExpr->y.pTab->aCol[iCol].zName)
      ));

#ifndef SQLITE_OMIT_FLOATING_POINT
      /* If the column has REAL affinity, it may currently be stored as an
      ** integer. Use OP_RealAffinity to make sure it is really real.
      **
      ** EVIDENCE-OF: R-60985-57662 SQLite will convert the value back to
      ** floating point when extracting it from the record.  */
      if( iCol>=0 && pTab->aCol[iCol].affinity==SQLITE_AFF_REAL ){
        sqlite3VdbeAddOp1(v, OP_RealAffinity, target);
      }
#endif
      break;
    }

    case TK_VECTOR: {
      sqlite3ErrorMsg(pParse, "row value misused");
      break;
    }

    /* TK_IF_NULL_ROW Expr nodes are inserted ahead of expressions
    ** that derive from the right-hand table of a LEFT JOIN.  The
    ** Expr.iTable value is the table number for the right-hand table.
    ** The expression is only evaluated if that table is not currently
    ** on a LEFT JOIN NULL row.
    */
    case TK_IF_NULL_ROW: {
      int addrINR;
      u8 okConstFactor = pParse->okConstFactor;
      addrINR = sqlite3VdbeAddOp1(v, OP_IfNullRow, pExpr->iTable);
      /* Temporarily disable factoring of constant expressions, since
      ** even though expressions may appear to be constant, they are not
      ** really constant because they originate from the right-hand side
      ** of a LEFT JOIN. */
      pParse->okConstFactor = 0;
      inReg = sqlite3ExprCodeTarget(pParse, pExpr->pLeft, target);
      pParse->okConstFactor = okConstFactor;
      sqlite3VdbeJumpHere(v, addrINR);
      sqlite3VdbeChangeP3(v, addrINR, inReg);
      break;
    }

    /*
    ** Form A:
    **   CASE x WHEN e1 THEN r1 WHEN e2 THEN r2 ... WHEN eN THEN rN ELSE y END
    **
    ** Form B:
    **   CASE WHEN e1 THEN r1 WHEN e2 THEN r2 ... WHEN eN THEN rN ELSE y END
    **
    ** Form A is can be transformed into the equivalent form B as follows:
    **   CASE WHEN x=e1 THEN r1 WHEN x=e2 THEN r2 ...
    **        WHEN x=eN THEN rN ELSE y END
    **
    ** X (if it exists) is in pExpr->pLeft.
    ** Y is in the last element of pExpr->x.pList if pExpr->x.pList->nExpr is
    ** odd.  The Y is also optional.  If the number of elements in x.pList
    ** is even, then Y is omitted and the "otherwise" result is NULL.
    ** Ei is in pExpr->pList->a[i*2] and Ri is pExpr->pList->a[i*2+1].
    **
    ** The result of the expression is the Ri for the first matching Ei,
    ** or if there is no matching Ei, the ELSE term Y, or if there is
    ** no ELSE term, NULL.
    */
    default: assert( op==TK_CASE ); {
      int endLabel;                     /* GOTO label for end of CASE stmt */
      int nextCase;                     /* GOTO label for next WHEN clause */
      int nExpr;                        /* 2x number of WHEN terms */
      int i;                            /* Loop counter */
      ExprList *pEList;                 /* List of WHEN terms */
      struct ExprList_item *aListelem;  /* Array of WHEN terms */
      Expr opCompare;                   /* The X==Ei expression */
      Expr *pX;                         /* The X expression */
      Expr *pTest = 0;                  /* X==Ei (form A) or just Ei (form B) */
      Expr *pDel = 0;
      sqlite3 *db = pParse->db;

      assert( !ExprHasProperty(pExpr, EP_xIsSelect) && pExpr->x.pList );
      assert(pExpr->x.pList->nExpr > 0);
      pEList = pExpr->x.pList;
      aListelem = pEList->a;
      nExpr = pEList->nExpr;
      endLabel = sqlite3VdbeMakeLabel(pParse);
      if( (pX = pExpr->pLeft)!=0 ){
        pDel = sqlite3ExprDup(db, pX, 0);
        if( db->mallocFailed ){
          sqlite3ExprDelete(db, pDel);
          break;
        }
        testcase( pX->op==TK_COLUMN );
        exprToRegister(pDel, exprCodeVector(pParse, pDel, &regFree1));
        testcase( regFree1==0 );
        memset(&opCompare, 0, sizeof(opCompare));
        opCompare.op = TK_EQ;
        opCompare.pLeft = pDel;
        pTest = &opCompare;
        /* Ticket b351d95f9cd5ef17e9d9dbae18f5ca8611190001:
        ** The value in regFree1 might get SCopy-ed into the file result.
        ** So make sure that the regFree1 register is not reused for other
        ** purposes and possibly overwritten.  */
        regFree1 = 0;
      }
      for(i=0; i<nExpr-1; i=i+2){
        if( pX ){
          assert( pTest!=0 );
          opCompare.pRight = aListelem[i].pExpr;
        }else{
          pTest = aListelem[i].pExpr;
        }
        nextCase = sqlite3VdbeMakeLabel(pParse);
        testcase( pTest->op==TK_COLUMN );
        sqlite3ExprIfFalse(pParse, pTest, nextCase, SQLITE_JUMPIFNULL);
        testcase( aListelem[i+1].pExpr->op==TK_COLUMN );
        sqlite3ExprCode(pParse, aListelem[i+1].pExpr, target);
        sqlite3VdbeGoto(v, endLabel);
        sqlite3VdbeResolveLabel(v, nextCase);
      }
      if( (nExpr&1)!=0 ){
        sqlite3ExprCode(pParse, pEList->a[nExpr-1].pExpr, target);
      }else{
        sqlite3VdbeAddOp2(v, OP_Null, 0, target);
      }
      sqlite3ExprDelete(db, pDel);
      sqlite3VdbeResolveLabel(v, endLabel);
      break;
    }
#ifndef SQLITE_OMIT_TRIGGER
    case TK_RAISE: {
      assert( pExpr->affExpr==OE_Rollback 
           || pExpr->affExpr==OE_Abort
           || pExpr->affExpr==OE_Fail
           || pExpr->affExpr==OE_Ignore
      );
      if( !pParse->pTriggerTab ){
        sqlite3ErrorMsg(pParse,
                       "RAISE() may only be used within a trigger-program");
        return 0;
      }
      if( pExpr->affExpr==OE_Abort ){
        sqlite3MayAbort(pParse);
      }
      assert( !ExprHasProperty(pExpr, EP_IntValue) );
      if( pExpr->affExpr==OE_Ignore ){
        sqlite3VdbeAddOp4(
            v, OP_Halt, SQLITE_OK, OE_Ignore, 0, pExpr->u.zToken,0);
        VdbeCoverage(v);
      }else{
        sqlite3HaltConstraint(pParse, SQLITE_CONSTRAINT_TRIGGER,
                              pExpr->affExpr, pExpr->u.zToken, 0, 0);
      }

      break;
    }
#endif
  }
  sqlite3ReleaseTempReg(pParse, regFree1);
  sqlite3ReleaseTempReg(pParse, regFree2);
  return inReg;
}