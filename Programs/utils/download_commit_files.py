import requests
import os
import yaml
import hashlib
import json
import multiprocessing
import task_timer

TOTAL_TASKS = 0
COMPLETED_TASKS = 0
DOWNLOAD_TASK = []
LOCAL_PROXIES={
    "http" : "http://127.0.0.1:7890",
    "https" : "http://127.0.0.1:7890"
}

def read_commit_yaml(commit_yaml_file):
    with open(commit_yaml_file, 'r') as file:
        commit_yaml = yaml.safe_load(file)
    return commit_yaml

def convert_file_name(file_name):
    """转换文件名
    i.e. src/lib/krb5/krb/pac.c -> src_lib_krb5_krb_pac.c
    
    Arguments:
        file_name {_type_} -- _description_
    """
    return file_name.replace("/", "_")

def get_project_name(commit_url):
    """转换得到 项目名
    i.e. https://github.com/krb5/krb5/commit/ea92d2f0fcceb54a70910fa32e9a0d7a5afc3583 ->
    krb5/krb5
    
    Arguments:
        commit_url {_type_} -- _description_

    Returns:
        _type_ -- _description_
    """
    tmp_project_name = commit_url[19:-48]
    return tmp_project_name
    

def download_commit_file(project_name, commit_hashCode, file_name, program_language):
    """根据 commit 和 file_name 下载文件

    Arguments:
        project_name {String} -- i.e. krb5/krb5
        commit_hashCode {String} -- i.e. ea92d2f0fcceb54a70910fa32e9a0d7a5afc3583
        file_name {String} -- src/lib/krb5/krb/pac.c
        program_language {String} -- 
    """
    global COMPLETED_TASKS
    COMPLETED_TASKS += 1

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
        
        # print(f'[+] 文件 "{filename}" 下载完成。')
    except:
        print(f'[!] 文件 "{filename}" 下载失败，URL: {download_commit_file_url}')

def append_unique_dict(list, dict):
    """append_unique_dict

    Arguments:
        list {_type_} -- _description_
        dict {_type_} -- _description_
    """
    if dict not in list:
        list.append(dict)        

def compute_hash(data):
    data_str = json.dumps(data, sort_keys=True).encode('utf-8')
    hash_result = hashlib.sha256(data_str).hexdigest()
    return hash_result  

def get_tmp_tasks(commit_item, file_name):
    
    project_name = get_project_name(commit_item["commit_url"])
    
    parent_commit_hashCode = commit_item["commit_hashCode"]["parent_commit_hashCode"]
    current_commit_hashCode = commit_item["commit_hashCode"]["current_commit_hashCode"]
    
    tmp_One_task = {
        "project_name": project_name,
        "commit_hashCode": parent_commit_hashCode,
        "file_name": file_name,
        "program_language": commit_item[file_name]["lang"]
    }
    
    # tmp_One_task["task_hashCode"] = compute_hash(tmp_One_task)
    
    tmp_Two_task = {
        "project_name": project_name,
        "commit_hashCode": current_commit_hashCode,
        "file_name": file_name,
        "program_language": commit_item[file_name]["lang"]
    }
    
    # tmp_Two_task["task_hashCode"] = compute_hash(tmp_Two_task)

    return tmp_One_task, tmp_Two_task
            
        

def get_download_tasks_from_commit_item(commit_item):
    global DOWNLOAD_TASK
    
    for item in commit_item:
        if item != "commit_url" and item != "commit_hashCode":
            # update download_tasks
            file_name = item
            
            tmp_One_task, tmp_Two_task = get_tmp_tasks(commit_item, file_name)
            
            append_unique_dict(DOWNLOAD_TASK, tmp_One_task)
            append_unique_dict(DOWNLOAD_TASK, tmp_Two_task)   
 
   
def get_download_tasks_from_commit_files():
    global DOWNLOAD_TASK
    commit_yaml_file = "Output/cve2commit_sanitize.yml"
    
    commits_yaml = read_commit_yaml(commit_yaml_file)
    
    for cwe in commits_yaml:
        cves = commits_yaml[cwe]
        for cve in cves:
            commits = cves[cve]
            for commit_url in commits:
                get_download_tasks_from_commit_item(commits[commit_url])


def handle_download_tasks(pool_size=4):
    # 多进程 download_commit_file
    global DOWNLOAD_TASK, TOTAL_TASKS, COMPLETED_TASKS
      
    TOTAL_TASKS = len(DOWNLOAD_TASK)
    COMPLETED_TASKS = 0
      
    # 使用多进程进行文件下载
    pool = multiprocessing.Pool(pool_size)
    for task in DOWNLOAD_TASK:
        pool.apply_async(download_commit_file, args=(task["project_name"], task["commit_hashCode"], task["file_name"], task["program_language"]))
    pool.close()
    pool.join()
    
    print("[+] 所有任务已完成")
    
    
                      
def test_single():
    project_name = "krb5/krb5"
    commit_hashCode = "ea92d2f0fcceb54a70910fa32e9a0d7a5afc3583"
    file_name = "src/lib/krb5/krb/pac.c"
    program_language = "C"

    download_commit_file(project_name, commit_hashCode, file_name, program_language)


@task_timer.timer    
def test():
    # 获取要下载的任务列表
    get_download_tasks_from_commit_files()
    handle_download_tasks()
    
if __name__ == '__main__':
    test()