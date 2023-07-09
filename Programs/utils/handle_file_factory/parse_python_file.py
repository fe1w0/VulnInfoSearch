import ast

def extract_functions(file_path):
    with open(file_path, "r") as file:
        source_code = file.read()
    
    tree = ast.parse(source_code)
    
    function_bodies = {}
    function_names = []
    
    for node in ast.walk(tree):
        if isinstance(node, ast.FunctionDef):
            function_name = node.name
            function_body = ast.unparse(node.body).encode()
            
            function_names.append(function_name)
            function_bodies[function_name] = {"function_name": function_name, "function_body": function_body}

            
    return function_bodies, function_names

def test():
    file_path = "DataSet/CommitsCollection/Python/pyupio_dparse/3290bb5b361971195a7d10175bf85ac75377af3d/dparse_parser.py"
    function_bodies, function_names = extract_functions(file_path)
    print(function_bodies, function_names)
