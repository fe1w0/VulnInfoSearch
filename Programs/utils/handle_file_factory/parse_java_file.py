# take from Tian Xiao
import javalang.tree
import javalang.ast
import os
import glob

def extract_functions(java_file):
    with open(java_file, 'r') as file:
        source_code = file.read()

    tree = javalang.parse.parse(source_code)  # type: javalang.ast.Node
    
    function_bodies = {}
    functions_names = []

    for path, node in tree.filter(javalang.tree.MethodDeclaration):
        function_name = node.name
        node_copy = node  # type: javalang.tree.MethodDeclaration
        # print(type(node))
        # print(type(node.body))
        pos = node.position  # type: javalang.tokenizer.Position
        # print(type(pos))
        start_line, start_column = node.position  # type: javalang.ast.Node.position

        # print("start_line:", start_line, "start_column:", start_column)
        function_body = extract_function_body(source_code, start_line, start_column, node.body).encode()
        
        function_bodies[function_name] = {"function_name": function_name, "function_body": function_body}
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

    function_lines = lines[start_line - 1: end_line + 1]

    # function_lines[0] = function_lines[0][start_column - 1:]
    # print(function_lines[0])
    offset = 0
    while function_lines[0][offset] == ' ':
        offset += 1
    for i in range(len(function_lines)):
        function_lines[i] = function_lines[i][offset:]
    function_body = '\n'.join(function_lines)
    return function_body


def test():
    file_name = "DataSet/CommitsCollection/Java/hyperledger_besu/a9bdd29128eca70d1922b282aa2b479ab492b563/ethereum_api_src_main_java_org_hyperledger_besu_ethereum_api_jsonrpc_internal_methods_JsonRpcMethod.java"
    functions, functions_name  = extract_functions(file_name)
    print(functions)
    print(functions_name)