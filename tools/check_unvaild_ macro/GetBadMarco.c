#include <cstdio>
#include "clang-c/Index.h"

// 定义回调函数，当遍历AST时被调用
CXChildVisitResult visitorFunction(CXCursor cursor, CXCursor parent, CXClientData client_data) {
  // 判断是否为函数声明
  if (clang_getCursorKind(cursor) == CXCursor_FunctionDecl) {
    // 获取函数声明中的所有子节点
    CXCursor *children;
    unsigned int numChildren;
    clang_visitChildren(cursor, visitorFunction, &numChildren);
    children = new CXCursor[numChildren];
    clang_visitChildren(cursor, visitorFunction, children);

    // 遍历子节点，判断是否为宏定义
    for (unsigned int i = 0; i < numChildren; i++) {
      if (clang_getCursorKind(children[i]) == CXCursor_MacroDefinition) {
        // 输出失效的宏定义信息
        CXString macroName = clang_getCursorSpelling(children[i]);
        printf("失效的宏定义：%s\n", clang_getCString(macroName));
        clang_disposeString(macroName);
      }
    }

    delete[] children;
  }

  return CXChildVisit_Recurse;
}

int main() {
  const char *filename = "example.c";
  
  // 创建索引对象
  CXIndex index = clang_createIndex(0, 0);
  
  // 解析C文件
  CXTranslationUnit tu = clang_parseTranslationUnit(index, filename, nullptr, 0, nullptr, 0, CXTranslationUnit_None);

  // 获取根节点
  CXCursor rootCursor = clang_getTranslationUnitCursor(tu);

  // 遍历AST
  clang_visitChildren(rootCursor, visitorFunction, nullptr);

  // 释放资源
  clang_disposeTranslationUnit(tu);
  clang_disposeIndex(index);

  return 0;
}
