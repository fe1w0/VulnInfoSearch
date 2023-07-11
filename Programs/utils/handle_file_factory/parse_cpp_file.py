from handle_file_factory.utils_tree_sitter import Parser, CPP_LANGUAGE
# from utils_tree_sitter import Parser, CPP_LANGUAGE

VALID_DECLARATOR = ["function_declarator", "pointer_declarator", "reference_declarator"]

def get_function_name_from_function_definition(function_node):
    for index in range(function_node.child_count):
        node = function_node.children[index]
        
        if node.type in VALID_DECLARATOR :
            if node.type == "reference_declarator":
                function_name = get_function_name_from_function_definition(node)
            else:
                function_name = node.text.decode()
                function_name = function_name.split("(")[0].split("\n")[-1]
    return function_name

def extract_functions(file_path):
    # Setup Parse-c
    parser = Parser()
    parser.set_language(CPP_LANGUAGE)
    
    with open(file_path, "r", errors='ignore') as file:
        source_code = file.read()
        
    tree = parser.parse(bytes(source_code, "utf-8"))
    root_node = tree.root_node
    
    source_code = source_code.split("\n")
    
    function_bodies = {}
    function_names = [] 
    
    for child_node in root_node.children:
        
        function_name = ''
        
        if child_node.type == "function_definition":
            function_node = child_node
            
            function_name = get_function_name_from_function_definition(function_node)         
            function_start_line = child_node.start_point[0]
            function_end_line = child_node.end_point[0]
            # 不在同一行
            if function_start_line != function_end_line:
                function_body = source_code[function_start_line : function_end_line + 1]
                function_body = "\n".join(function_body)
            else:
                function_body = source_code[function_start_line]

            if function_name == '':
                print(1)

            # function_name@ 表示 有重复
            if function_name in function_names:
                function_name += "@"
            function_names.append(function_name)
            
            
            function_bodies[function_name] = {"function_name": function_name, "function_body": function_body.encode()}
    
    return function_bodies, function_names


def test():
    test_file = "DataSet/CommitsCollection/C++/axiomatic-systems_Bento4/2a940e88c982594a927a7085c828f012d6736c13/Source_C++_Core_Ap4AvccAtom.cpp"
    test_file = "DataSet/CommitsCollection/C++/simsong_tcpflow/a4e1cd14eb5ccc51ed271b65b3420f7d692c40eb/src_wifipcap_wifipcap.cpp"
    test_file = "DataSet/CommitsCollection/C++/ImpulseAdventure_JPEGsnoop/fa2bb27deae3d2ac29f7e345763fec52ad7b46d3/JfifDecode.cpp"
    
    function_bodies, function_names = extract_functions(test_file)
    
    print(function_bodies, function_names)
    
# test()