from handle_file_factory.utils_tree_sitter import Parser, CPP_LANGUAGE
# from utils_tree_sitter import Parser, CPP_LANGUAGE 

VALID_DECLARATOR = ["function_declarator"]
VALID_FUNCTION_NAME = ["function_declarator", "pointer_declarator", "qualified_identifier", "field_identifier", "identifier", "reference_declarator", "operator_cast"]

def get_function_name_from_function_definition(function_node):
    """从 node(function_definition)中，获得 function_name

    Arguments:
        function_node {_type_} -- _description_

    Returns:
        _type_ -- _description_
    """
    try:
        for node in function_node.children:
            if node.type in VALID_FUNCTION_NAME :
                function_name = node.text.decode()
                function_name = function_name.split("(")[0].split("\n")[-1]
            if node.type == 'parenthesized_declarator':
                function_name = get_function_name_from_function_definition(node)
    except Exception as e:
        print(f"[!] Error: {e}. No function_name")
    return function_name

def get_function_name_and_body_from_function_node(function_node, source_code, function_bodies, function_names):
    function_name = get_function_name_from_function_definition(function_node)         
    function_start_line = function_node.start_point[0]
    function_end_line = function_node.end_point[0]
    # 不在同一行
    if function_start_line != function_end_line:
        function_body = source_code[function_start_line : function_end_line + 1]
        function_body = "\n".join(function_body)
    else:
        function_body = source_code[function_start_line]

    # function_name@ 表示 有重复
    if function_name in function_names:
        function_name += "@"
    function_names.append(function_name)
    
    function_bodies[function_name] = {"function_name": function_name, "function_body": function_body.encode()}
    
    return function_bodies, function_names
    
def iteration_get_function(node, source_code, function_bodies, function_names):
    
    if node.type == "function_definition":
        function_bodies, function_names = get_function_name_and_body_from_function_node(node, source_code, function_bodies, function_names)
        return function_bodies, function_names

    else:
        # 迭代查询
        for children_node in node.children:
            function_bodies, function_names = iteration_get_function(children_node, source_code, function_bodies, function_names)
        return function_bodies, function_names
    
def extract_functions(file_path):
    # Setup Parse-c++
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
        
    #     if child_node.type == "function_definition":
    #         function_node = child_node
    #         function_bodies, function_names = get_function_name_and_body_from_function_node(function_node, source_code, function_bodies, function_names)
   
        function_bodies, function_names = iteration_get_function(child_node, source_code, function_bodies, function_names)
            
    if function_bodies == {} and function_names == []:
         print(f"[!] Error: No function_bodies and function_names in {file_path}")
         
    return function_bodies, function_names


def test():
    # test_file = "DataSet/CommitsCollection/C++/axiomatic-systems_Bento4/2a940e88c982594a927a7085c828f012d6736c13/Source_C++_Core_Ap4AvccAtom.cpp"
    # test_file = "DataSet/CommitsCollection/C++/simsong_tcpflow/a4e1cd14eb5ccc51ed271b65b3420f7d692c40eb/src_wifipcap_wifipcap.cpp"
    # test_file = "DataSet/CommitsCollection/C++/ImpulseAdventure_JPEGsnoop/fa2bb27deae3d2ac29f7e345763fec52ad7b46d3/JfifDecode.cpp"
    test_file = "DataSet/CommitsCollection/C++/tensorflow_tensorflow/12b1ff82b3f26ff8de17e58703231d5a02ef1b8b/tensorflow_core_kernels_pooling_ops_3d.cc"
    test_file = "DataSet/CommitsCollection/C++/facebook_hhvm/08193b7f0cd3910256e00d599f0f3eb2519c44ca/hphp_runtime_base_type-string.h"
    test_file = "DataSet/CommitsCollection/C++/dropbox_lepton/7789d99ac156adfd7bbf66e7824bd3e948a74cf7/src_vp8_model_model.hh"
    
    # TEST
    test_file = "DataSet/CommitsCollection/C++/yhirose_cpp-peglib/14305f9f53cde207568f21675a1b9294a3ab28b4/test_test1.cc"
    
    function_bodies, function_names = extract_functions(test_file)
    
    print(function_bodies, function_names)
    
# test()