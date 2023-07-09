# take from TianXiao
from clang import cindex
import os
import glob
import re

cindex.Config.set_library_path('/Library/Developer/CommandLineTools/usr/lib')

def extract_functions(file_path):
    """批处理提取c文件

    Arguments:
        file_path {_type_} -- 文件路径
    """
    index = cindex.Index.create()
    tu = index.parse(file_path)

    function_bodies = {}
    function_names = []
    
    for node in tu.cursor.walk_preorder():
        if node.kind == cindex.CursorKind.FUNCTION_DECL:
            # 提取函数名字
            function_name = node.spelling
            # 提取函数
            start_loc = node.extent.start
            end_loc = node.extent.end
            with open(file_path, 'rb') as file:
                file.seek(start_loc.offset)
                function_code = file.read(end_loc.offset - start_loc.offset)
            if b'\n' not in function_code:
                function_code += b';'
            function_body = function_code
            function_names.append(function_name)
            function_bodies[function_name] = {"function_name": function_name, "function_body": function_body}
            
            
    return function_bodies, function_names


# def test():
#     function_bodies, function_names = extract_functions("DataSet/CommitsCollection/C/krb5_krb5/fa62bd33a0c0889c083999c0289ffa81a5d51e7b/src_lib_krb5_krb_pac.c")
#     print(function_names)
    
# test()