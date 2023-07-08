import requests
import os
import yaml


LOCAL_PROXIES={
    "http" : "http://127.0.0.1:7890",
    "https" : "http://127.0.0.1:7890"
}

def read_commit_yaml(commit_yaml_file):
    with open(commit_yaml_file, 'r') as file:
        commit_yaml = yaml.safe_load(file)
    return commit_yaml

def convert_file_name(file_name):
    """_summary_

    _extended_summary_

    Arguments:
        file_name {_type_} -- _description_
    """
    return file_name.replace("/", "_")

def download_commit_file(project_name, commit_hashCode, file_name, program_language):
    """根据 commit 和 file_name 下载文件

    _extended_summary_

    Arguments:
        project_name {String} -- i.e. krb5/krb5
        commit_hashCode {String} -- i.e. ea92d2f0fcceb54a70910fa32e9a0d7a5afc3583
        file_name {String} -- src/lib/krb5/krb/pac.c
        program_language {String} -- 
    """
    
    download_commit_file_url = "https://raw.githubusercontent.com/" + project_name + "/" + commit_hashCode + "/" + file_name
    # print(download_commit_file_url)
        
    try:
        folder_path = "DataSet/CommitsCollection" + "/" + program_language + "/" + convert_file_name(project_name) + "/" +  commit_hashCode
        filename = folder_path + "/" +  convert_file_name(file_name)
        
        response = requests.get(download_commit_file_url, proxies=LOCAL_PROXIES)
        
        response.raise_for_status()
        
        os.makedirs(folder_path, exist_ok=True)
        
        with open(filename, 'wb') as file:
            file.write(response.content)
        
        print(f'[+] 文件 "{filename}" 下载完成。')
    except:
        print(f'[!] 文件 "{filename}" 下载失败。')

   
def download_commit_files():
    commit_yaml_file = "Output/cve2commit_sanitize_new.yml"
    
    commits_yaml = read_commit_yaml(commit_yaml_file)
    
    for cwe in commits_yaml:
        cves = commits_yaml[cwe]
        for cve in cves:
            commits = cves[cve]
            for commit in commits:
                if "commit_hashCode" not in commits[commit]:
                    print(commits[commit])
            
def test():
    project_name = "krb5/krb5"
    commit_hashCode = "ea92d2f0fcceb54a70910fa32e9a0d7a5afc3583"
    file_name = "src/lib/krb5/krb/pac.c"
    program_language = "C"

    download_commit_file(project_name, commit_hashCode, file_name, program_language)

# test()

download_commit_files()