# take from Tian Xiao
import javalang.tree
import javalang.ast


def extract_functions(java_file):
    with open(java_file, "r", errors='ignore')as file:
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
                start_line, start_column = child.position
                # print(child.position)
                fields.append(lines[child.position[0] - 1])
            if isinstance(child, javalang.tree.MethodDeclaration):
                
                start_line, start_column = child.position
                function_name = child.name
                if function_name in function_names:
                    function_name += "@"
                function_names.append(function_name)
                # print(child.modifiers, function_name)
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

def handle_function_body_java(class_declaration, fields, function_body):
    """处理java的函数体

    _extended_summary_

    Arguments:
        class_declaration {_type_} -- _description_
        fields {list} -- _description_
        function_body {_type_} -- _description_

    Returns:
        _type_ -- _description_
    """
    return_class_function_body = class_declaration
    if '{' not in return_class_function_body:
        return_class_function_body += '\n{'
    return_class_function_body += '\n'
    
    for field in fields:
        return_class_function_body += field + '\n'

    return_class_function_body += function_body + '\n'
    
    return_class_function_body += '}'

    return return_class_function_body

def change_java_method_modifier_for_joern(function_body_code):
    source_code_lines = function_body_code.split('\n')
    tmp_code_line = (source_code_lines[0].replace("protected", "public").replace("private", "public"))
    source_code_lines[0] = tmp_code_line
    changed_function_body_code = "\n".join(source_code_lines)
    return changed_function_body_code
    
    

def test():
    file_name = "DataSet/CommitsCollection/Java/snakeyaml_snakeyaml/4b8d1af4bd422a015fbb07d235f282412c863220/src_test_java_org_yaml_snakeyaml_issues_issue525_FuzzyStackOverflowTest.java"
    file_name = "DataSet/CommitsCollection/Java/eclipse-californium_californium/9112803a20152d1386415a4418e639919f8ac12b/scandium-core_src_main_java_org_eclipse_californium_scandium_DTLSConnector.java"
    functions, functions_name  = extract_functions(file_name)
    tmp_function_body = functions['createConnectionStore']
    
    
    tmp_function_body_code = tmp_function_body["function_body"]
    
    handle_function_body = handle_function_body_java(tmp_function_body["class_declaration"], tmp_function_body["fields"], tmp_function_body["function_body"])
    
    result_function_body = change_java_method_modifier_for_joern(tmp_function_body_code)
    
    print(handle_function_body)
    
    print(result_function_body)

# test()