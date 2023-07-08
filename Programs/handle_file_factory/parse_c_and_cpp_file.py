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

# 搜索漏洞关键词
def search_function(name, arr, keywords):
    function = []
    function_name = []
    for i in range(len(arr)):
        # element = element.decode()
        # print(element)
        # print('-------')

        for keyword in keywords:
            if keyword in name[i]:
                function.append(arr[i])
                function_name.append(name[i])
                # print(function)
                break
    return function, function_name

def process_c_file(file_path, output_folder):
    # 在这里编写对单个C文件的处理逻辑
    print("正在处理文件:", file_path)
    function_codes, function_names = extract_all_function_code(file_path)
    print(function_names)
    
    target_keyword = ['main']
    # 定位关键词的函数
    found_function, found_function_name = search_function(function_names, function_codes, target_keyword)
    # print(found_function)

    # print(found_function_name)

    for i in found_function_name:
        if i is None:
            print("函数为空")

        else:
            output_filename = os.path.basename(file_path)
            output_file = os.path.join(output_folder, output_filename)
            print(output_filename)
            output_content = ''
            for function in found_function:
                # 去除函数中的注释
                pattern = r"/\*(.*?)\*/|//.*?$"
                modified_function = re.sub(pattern, '', function.decode(), flags=re.MULTILINE | re.DOTALL)
                output_content += modified_function
                output_content += '\n'
            if output_content.count('\n') + 1 > 7:
                with open(output_file, 'w') as file:
                    file.write(output_content + '\n')


def process_folder(input_folder, output_folder):
    # 遍历输入文件夹中的所有C文件
    pattern = os.path.join(input_folder, "*.cpp")
    c_files = glob.glob(pattern)

    # 处理每个C文件
    for file_path in c_files:
        process_c_file(file_path, output_folder)

    # 递归遍历子文件夹
    for subfolder in os.listdir(input_folder):
        subfolder_path = os.path.join(input_folder, subfolder)
        if os.path.isdir(subfolder_path):
            output_subfolder = os.path.join(output_folder, subfolder)
            os.makedirs(output_subfolder, exist_ok=True)
            process_folder(subfolder_path, output_subfolder)

# 输入文件夹路径
# top_folder_path = "c"
# output_folders = "NoVul_cplus"
#
# 开始处理顶级文件夹及其子文件夹中的C文件
# process_folder(top_folder_path, output_folders)

file_path = "c/main.cpp"
function_codes, function_names = extract_all_function_code(file_path)
print(function_codes, function_names)