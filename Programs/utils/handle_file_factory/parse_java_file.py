# take from Tian Xiao
import javalang.tree
import javalang.ast
import os
import glob

def extract_functions(java_file):
    with open(java_file, 'r') as file:
        source_code = file.read()

    tree = javalang.parse.parse(source_code)  # type: javalang.ast.Node
    lines = source_code.split('\n')
    
    function_bodies = {}
    functions_names = []

    for path, node in tree.filter(javalang.tree.ClassDeclaration):
        # pos = node.position
        start_line, start_col = node.position
        class_declaration = lines[start_line - 1]

        for child in node.body:

            # if isinstance(child, javalang.tree.FieldDeclaration):
            #     fields.append(lines[child.position[0] - 1])
            if isinstance(child, javalang.tree.MethodDeclaration):
                start_line, start_column = child.position
                function_body = extract_function_body(source_code, start_line, start_column, child.body)
                function_name = child.name
                
                function_body = class_declaration + "\n" + function_body + "\n}"
                
                function_bodies[function_name] = {"function_name": function_name, "function_body": function_body.encode()}
                functions_names.append(function_name)
    return function_bodies, functions_names


def extract_function_body(source_code, start_line, start_column, body):
    lines = source_code.split('\n')
    # function_lines = lines[start_line - 1:]

    # print(len(body))
    # for t in body:
    # print(t.position[0], t.position[1])
    # 如果函数声明之后没有元素，则说明函数为空，提取函数声明即可
    if body is None or len(body) == 0:
        end_line = start_line
    else:
        end_line, _ = body[-1].position
        right_cnt = 0
        while True:
            for c in lines[end_line]:
                if c == '{':
                    right_cnt -= 1
                elif c == '}':
                    right_cnt += 1
                if right_cnt == 1:
                    break
            if right_cnt == 1:
                break
            end_line += 1

    function_lines = lines[start_line - 1: end_line + 2]

    # function_lines[0] = function_lines[0][start_column - 1:]
    # print(function_lines[0])
    offset = 0
    while function_lines[0][offset] == ' ':
        offset += 1
    for i in range(len(function_lines)):
        function_lines[i] = function_lines[i][offset:]
    function_body = '\n'.join(function_lines)
    return function_body


# def test():
#     file_name = "DataSet/CommitsCollection/Java/snakeyaml_snakeyaml/4b8d1af4bd422a015fbb07d235f282412c863220/src_test_java_org_yaml_snakeyaml_issues_issue525_FuzzyStackOverflowTest.java"
#     file_name = "DataSet/CommitsCollection/Java/eclipse-californium_californium/9112803a20152d1386415a4418e639919f8ac12b/scandium-core_src_main_java_org_eclipse_californium_scandium_DTLSConnector.java"
#     functions, functions_name  = extract_functions(file_name)
#     print(functions['stop'])
#     # print(functions_name)

# test()

# import javalang

# with open(file_name, 'r') as file:
#     java_source = file.read()

# tree = javalang.parse.parse(java_source)

# for path, node in tree.filter(javalang.tree.ClassDeclaration):
#     print(f"Class: {node.name}")
#     for field in node.fields:
#         print(f"    Field: {field.type} {field.declarators[0].name}")
#     for method in node.methods:
#         print(f"    Method: {method.name}({', '.join(str(arg.type) for arg in method.parameters)})")
