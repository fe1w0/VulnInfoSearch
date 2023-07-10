# take from Tian Xiao
import javalang.tree
import javalang.ast


def extract_functions(java_file):
    with open(java_file, 'r') as file:
        source_code = file.read()
    tree = javalang.parse.parse(source_code)  # type: javalang.ast.Node
    lines = source_code.split('\n')
        
    function_names = []
    function_bodies = {}

    for path, node in tree.filter(javalang.tree.ClassDeclaration):
        pos = node.position  # type: javalang.tokenizer.Position
        start_line, start_col = node.position  # type: javalang.ast.Node.position
        class_declaration = lines[start_line - 1]

        fields = []
        
        tmp_function_names = []
        
        tmp_function_bodies = {}

        for child in node.body:
            if isinstance(child, javalang.tree.FieldDeclaration):
                fields.append(lines[child.position[0] - 1])
            if isinstance(child, javalang.tree.MethodDeclaration):
                start_line, start_column = child.position
                function_name = child.name
                function_names.append(function_name)
                tmp_function_names.append(function_name)
                function_body = extract_function_body(lines, start_line, child.body)
                
                tmp_function_bodies[function_name] = function_body
        
        for function_name in tmp_function_names:
            function_body = tmp_function_bodies[function_name]
            function_bodies[function_name] = {"function_name": function_name, "function_body": function_body, "fields": fields, "class_declaration": class_declaration}
            
    return function_bodies, function_names


def extract_function_body(lines, start_line, body):
    if body is None or len(body) == 0:
        end_line = start_line
    else:
        end_line, _ = body[-1].position
        end_line -= 1
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

    function_body = '\n'.join(function_lines)
    return function_body

# def test():
#     file_name = "DataSet/CommitsCollection/Java/snakeyaml_snakeyaml/4b8d1af4bd422a015fbb07d235f282412c863220/src_test_java_org_yaml_snakeyaml_issues_issue525_FuzzyStackOverflowTest.java"
#     file_name = "DataSet/CommitsCollection/Java/eclipse-californium_californium/9112803a20152d1386415a4418e639919f8ac12b/scandium-core_src_main_java_org_eclipse_californium_scandium_DTLSConnector.java"
#     functions, functions_name  = extract_functions(file_name)
#     print(functions['stop']['function_body'].decode())

# test()