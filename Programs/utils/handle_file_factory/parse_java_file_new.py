from utils_tree_sitter import Parser, JAVA_LANGUAGE

def extract_functions(file_path):
    # Setup Parse-c
    parser = Parser()
    parser.set_language(JAVA_LANGUAGE)
    
    with open(file_path, "r") as file:
        source_code = file.read()
        
    tree = parser.parse(bytes(source_code, "utf-8"))
    root_node = tree.root_node
    
    source_code = source_code.split("\n")
    
    
    function_bodies = {}
    function_names = []
    
    for child_node in root_node.children:
        
        
        
        if child_node.type == "interface_declaration":
            class_node = child_node
            
            # class_declaration = 
            
            # Class 
            for index in range(class_node.child_count):
                node = class_node.children[index]
                
            


    
    # return function_bodies, function_names


def test():
    test_file = "DataSet/CommitsCollection/Java/hyperledger_besu/a9bdd29128eca70d1922b282aa2b479ab492b563/ethereum_api_src_main_java_org_hyperledger_besu_ethereum_api_jsonrpc_internal_methods_JsonRpcMethod.java"
    function_bodies, function_names = extract_functions(test_file)
    
    print(function_bodies, function_names)
    
test()