# take from TianXiao
from clang import cindex
import os
import glob
import re

cindex.Config.set_library_path('/Library/Developer/CommandLineTools/usr/lib')

def extract_all_function_code(file_path):
    """批处理提取c文件

    Arguments:
        file_path {_type_} -- 文件路径
    """
    index = cindex.Index.create()
    tu = index.parse(file_path)

    functions = []
    function_names = []
    for node in tu.cursor.walk_preorder():
        if node.kind == cindex.CursorKind.FUNCTION_DECL:
            # 提取函数名字
            function_names.append(node.spelling)
            # 提取函数
            start_loc = node.extent.start
            end_loc = node.extent.end
            with open(file_path, 'rb') as file:
                file.seek(start_loc.offset)
                function_code = file.read(end_loc.offset - start_loc.offset)
            if b'\n' not in function_code:
                function_code += b';'
            functions.append(function_code)
    return functions, function_names
