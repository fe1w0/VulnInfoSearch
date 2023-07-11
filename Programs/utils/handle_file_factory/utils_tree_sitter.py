from tree_sitter import Language, Parser


BUILD_LIBRARY = 'build/my-languages.so'

Language.build_library(
  # Store the library in the `build` directory
  BUILD_LIBRARY,

  # Include one or more languages
  [
    'tools/tree-sitter-grammers/tree-sitter-c',
    'tools/tree-sitter-grammers/tree-sitter-cpp',
    'tools/tree-sitter-grammers/tree-sitter-java',
    'tools/tree-sitter-grammers/tree-sitter-python'
  ]
)

C_LANGUAGE = Language(BUILD_LIBRARY, 'c')
CPP_LANGUAGE = Language(BUILD_LIBRARY, 'cpp')
JAVA_LANGUAGE = Language(BUILD_LIBRARY, 'java')
PY_LANGUAGE = Language(BUILD_LIBRARY, 'python')
