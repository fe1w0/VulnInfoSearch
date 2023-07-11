from handle_file_factory.utils_tree_sitter import Parser, C_LANGUAGE
# from utils_tree_sitter import Parser, C_LANGUAGE

def extract_functions(file_path):
    # Setup Parse-c
    parser = Parser()
    parser.set_language(C_LANGUAGE)
    
    with open(file_path, "r", errors='ignore')as file:
        source_code = file.read()
        
    tree = parser.parse(bytes(source_code, "utf-8"))
    root_node = tree.root_node
    
    source_code = source_code.split("\n")
    
    
    function_bodies = {}
    function_names = [] 
    
    for child_node in root_node.children:
        if child_node.type == "function_definition":
            function_node = child_node
                    
            flag_contain_code = False
            
            for index in range(function_node.child_count):
                node = function_node.children[index]
                
                if node.type == "function_declarator" or node.type == "pointer_declarator":
                    flag_contain_code = True
                    function_declarator_part = node.text.decode()
                    function_name = function_declarator_part.split("(")[0].split("\n")[-1]
                    
            
            function_start_line = child_node.start_point[0]
            function_end_line = child_node.end_point[0]
            
            # 不在同一行
            if function_start_line != function_end_line:
                function_body = source_code[function_start_line : function_end_line + 1]
                function_body = "\n".join(function_body)
            else:
                function_body = source_code[function_start_line]

            if flag_contain_code:
                # function_name@ 表示 有重复
                if function_name in function_names:
                    function_name += "@"
                function_names.append(function_name)
                function_bodies[function_name] = {"function_name": function_name, "function_body": function_body.encode()}
            
            
    
    return function_bodies, function_names


def test():
    test_file = "DataSet/CommitsCollection/C/krb5_krb5/fa62bd33a0c0889c083999c0289ffa81a5d51e7b/src_lib_krb5_krb_pac.c"
    test_file = "DataSet/CommitsCollection/C/bitlbee_bitlbee/701ab8129ba9ea64f569daedca9a8603abad740f/protocols_bee_ft.c"
    test_file = "DataSet/CommitsCollection/C/michaelrsweet_htmldoc/6898d0a29da923702a1895fbbff7bba2910c54fb/htmldoc_file.c"
    test_file = "DataSet/CommitsCollection/C/torvalds_linux/d974baa398f34393db76be45f7d4d04fbdbb4a0a/arch_x86_kvm_vmx.c"
    function_bodies, function_names = extract_functions(test_file)
    
    
    print(function_bodies, function_names)
    
# test()