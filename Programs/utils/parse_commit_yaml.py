from handle_yaml_file import read_yaml_file
from download_commit_files import convert_file_name, get_project_name
import os
from task_timer import timer
import traceback

from handle_file_factory.parse_c_and_cpp_file import extract_functions as extract_functions_c
from handle_file_factory.parse_java_file import extract_functions as extract_functions_java
from handle_file_factory.parse_python_file import extract_functions as extract_functions_python

TOTAL_TIMER = 0
ERROR_TIMER = 0

OPERATION_LANG = ["C", "C++", "Java", "Python"]

def check_file_exist(file_path):
    return os.path.exists(file_path)

def mkdir_folder(folder_path):
    os.makedirs(folder_path, exist_ok=True)

def parse_commit_yaml():
    # 从 Output/cve2commit_sanitize.yml 中提取 代码 commit 信息
    commit_data = read_yaml_file("Output/cve2commit_sanitize.yml")
    
    for cwe_id in commit_data:
        tmp_cves = commit_data[cwe_id]
        for cve_id in tmp_cves:
            cve_info = tmp_cves[cve_id]
            for commit in cve_info:
                tmp_commit = cve_info[commit]
                project_name  = get_project_name(tmp_commit["commit_url"])
                
                for commit_item in cve_info[commit]:
                    
                    if commit_item != "commit_url" and commit_item != "commit_hashCode":
                        file_name = commit_item
                        program_language = tmp_commit[file_name]["lang"]
                        defective_commit_file_path = "DataSet/CommitsCollection" + "/" + program_language + "/" + convert_file_name(project_name) + "/" +  tmp_commit["commit_hashCode"]["parent_commit_hashCode"] + "/" +  convert_file_name(file_name)
                        patch_commit_file_path = "DataSet/CommitsCollection" + "/" + program_language + "/" + convert_file_name(project_name) + "/" +  tmp_commit["commit_hashCode"]["current_commit_hashCode"] + "/" +  convert_file_name(file_name)
                        handle_different_code_from_commits(defective_commit_file_path, patch_commit_file_path, program_language, cve_id, cwe_id)

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
        return_class_function_body += field.replace("protected", "public").replace("private", "public") + '\n'

    return_class_function_body += function_body + '\n'
    
    return_class_function_body += '}'

    return return_class_function_body
    

def diff_function_bodies(defective_commit_function_bodies, patch_commit_function_bodies, total_function_names):
    
    vuln_info = []
    no_vuln_info = []
    
    # print(total_function_names)
    for function_name in total_function_names:
        # 同类函数，存在修改现象
        if function_name in defective_commit_function_bodies and function_name in patch_commit_function_bodies:
            if defective_commit_function_bodies[function_name] != patch_commit_function_bodies[function_name]:
                vuln_info.append({"function_name": function_name, "function_body": defective_commit_function_bodies[function_name]})
                no_vuln_info.append(({"function_name": function_name, "function_body": patch_commit_function_bodies[function_name]}))
        # 添加的 补丁函数    
        elif function_name not in defective_commit_function_bodies and function_name in patch_commit_function_bodies:
            no_vuln_info.append(({"function_name": function_name, "function_body": patch_commit_function_bodies[function_name]}))
        # 被删除的恶意函数
        elif function_name in defective_commit_function_bodies and function_name not in patch_commit_function_bodies:
            vuln_info.append({"function_name": function_name, "function_body": defective_commit_function_bodies[function_name]})

    return vuln_info, no_vuln_info


def get_lang_extension(program_lang):
    if program_lang == "C":
        return ".c"
    elif program_lang == "C++":
        return ".cpp"
    elif program_lang == "Java":
        return ".java"
    elif program_lang == "Python":
        return ".py"

def change_java_method_modifier_for_joern(function_body_code):
    source_code_lines = function_body_code.split('\n')
    tmp_code_line = (source_code_lines[0].replace("protected", "public").replace("private", "public"))
    source_code_lines[0] = tmp_code_line
    changed_function_body_code = "\n".join(source_code_lines)
    return changed_function_body_code

def write_function_body(function_bodies_set, cwe_id, cve_id, program_language, tmp_folder="DataSet/MethodSet/VulnSet/"):
    folder = tmp_folder + program_language + "/" + cwe_id + "/" + cve_id + "/"
    os.makedirs(folder, exist_ok=True)
    
    lang_extension = get_lang_extension(program_language)
    
    if function_bodies_set != []:
    
        for function_item in function_bodies_set:
            function_name = function_item["function_name"]
            
            if program_language in ["Java"]:
                tmp_function_body_code = function_item["function_body"]["function_body"]
                function_item["function_body"]["function_body"] = change_java_method_modifier_for_joern(tmp_function_body_code)
                function_body = handle_function_body_java(function_item["function_body"]["class_declaration"], function_item["function_body"]["fields"], function_item["function_body"]["function_body"]).encode()
            else:
                function_body = function_item["function_body"]["function_body"]

            file_path = folder + function_name + lang_extension
            
            with open(file_path, 'wb') as file:
                file.write(function_body)
            file.close()    
        
            # print(f"[+] Write: {file_path}")


def handle_different_code_from_commits(defective_commit_file_path, patch_commit_file_path, program_language, cve_id, cwe_id):
    """处理 defective_commit_file 和 patch_commit_file

    生成 Target 目录和文件
    Vuln: DataSet/MethodSet/VulnSet/{lang}/{cwe-id}/{cve-id}/{method_name}.{lang_extension}
    Patch: DataSet/MethodSet/NoVulnSet/{lang}/{cwe-id}/{cve-id}/{method_name}.{lang_extension}
    
    Arguments:
        defective_commit_file_path {_type_} -- _description_
        patch_commit_file_path {_type_} -- _description_
        program_language {_type_} -- _description_
        cve_id {_type_} -- _description_
        cwe_id {_type_} -- _description_
    """
    global TOTAL_TIMER, ERROR_TIMER
    try:
        if check_file_exist(defective_commit_file_path) and check_file_exist(patch_commit_file_path) and program_language in OPERATION_LANG:
            if program_language in ["C", "C++"]:
                defective_commit_function_bodies, defective_commit_function_names = extract_functions_c(defective_commit_file_path)
                patch_commit_function_bodies, patch_commit_function_names = extract_functions_c(patch_commit_file_path)
            elif program_language in ["Java"]:
                defective_commit_function_bodies, defective_commit_function_names = extract_functions_java(defective_commit_file_path)
                patch_commit_function_bodies, patch_commit_function_names = extract_functions_java(patch_commit_file_path)
            elif program_language in ["Python"]:
                defective_commit_function_bodies, defective_commit_function_names = extract_functions_python(defective_commit_file_path)
                patch_commit_function_bodies, patch_commit_function_names = extract_functions_python(patch_commit_file_path)
            
            total_function_names = list(set(defective_commit_function_names + patch_commit_function_names))
            
            vuln_info, no_vuln_info = diff_function_bodies(defective_commit_function_bodies, patch_commit_function_bodies, total_function_names)
            
            write_function_body(vuln_info, cwe_id, cve_id, program_language, tmp_folder="DataSet/MethodSet/VulnSet/")
            
            write_function_body(no_vuln_info, cwe_id, cve_id, program_language, tmp_folder="DataSet/MethodSet/NoVulnSet/")
            
            TOTAL_TIMER += 1
            
            # print(vuln_info, no_vuln_info)
            # if defective_commit_function_names == patch_commit_function_names:
                # print(defective_commit_file_path, patch_commit_file_path)
    except Exception as e:
        ERROR_TIMER += 1
        traceback.print_exc()
        print(f"[!] Error:{e}. Defective: {defective_commit_file_path}, Patch: {patch_commit_file_path}")
        
@timer   
def test():
    parse_commit_yaml()
    
test()
print(f"TOTAL_TIMER: {TOTAL_TIMER}")
print(f"ERROR_TIMER: {ERROR_TIMER}")