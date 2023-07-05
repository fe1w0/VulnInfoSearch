from lxml import html
import requests
import re
import time
import yaml
from concurrent.futures import ThreadPoolExecutor
import concurrent
from bs4 import BeautifulSoup


MAX_WORKERS = 10

LOCAL_PROXIES={
    "http" : "http://127.0.0.1:7890",
    "https" : "http://127.0.0.1:7890"
}

LANGUAGE_DICT = {
    "C/CPP": [".cpp", ".c", ".h", ".hpp", ".cxx", ".cc"],
    "Python": [".py"],
    "PHP": [".php"],
    "Java": [".java"],
    "JavaScript": [".js", ".ts"],
    "Go": [".go"],
    "Rust": [".rs"],
    "Swift": [".swift"],
    "Ruby": [".rb"],
    "HTML": [".html", ".htm"],
    "CSS": [".css"],
    "TypeScript": [".ts"],
    "Kotlin": [".kt"],
    "Perl": [".pl", ".pm"],
    "Shell": [".sh"],
    "Objective-C": [".m"],
    "R": [".r"],
    "Scala": [".scala"],
    "Lua": [".lua"],
    "Matlab": [".m"],
    "Haskell": [".hs"],
    "Groovy": [".groovy"],
    "VB.NET": [".vb"],
    "C#": [".cs"],
    "CoffeeScript": [".coffee"],
    "Dart": [".dart"],
    "Julia": [".jl"],
    "Erlang": [".erl"],
    "Clojure": [".clj"],
    "Fortran": [".f90"],
    "Assembly": [".asm"],
    "F#": [".fs"],
    "OCaml": [".ml"],
    "PowerShell": [".ps1"]
}

def get_cwe2commits():
    with open('Output/cve2commit.yml', 'r') as f:
        cwe2cve_info = yaml.safe_load(f)
    return cwe2cve_info


def judge_program_language(file_name):
    for language, extensions in LANGUAGE_DICT.items():
        for extension in extensions:
            if file_name.endswith(extension):
                return language
    return "Unknown"

def save_output(result):
    with open('Output/cve2commit_sanitize.yml', 'w') as f:
        yaml.dump(result, f, indent=4, sort_keys=False)

def get_files_name_from_commit(commit_url):
    get_files_name  = []
    if commit_url != "":
        try:
            headers = {
                "User-Agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36 Edg/114.0.1823.67"
            }
            html_text = requests.get(commit_url, headers=headers, proxies=LOCAL_PROXIES).text
            soup = BeautifulSoup(html_text, 'html.parser')
            files = soup.select("span.Truncate > a")
            for file in files:
                file_name = file.get("title")
                get_files_name.append(file_name)
            print("[+] Success: " + commit_url)
        except Exception as e:
            print(f"[!] Error: {commit_url} 查询失败：{e}, 重新尝试")
            get_files_name_from_commit(commit_url)
    return get_files_name
    
def handle_commit_files(get_files_name):
    # Example:
    # {'Unknown': ['README.md', 'requirements.txt'], 
    # 'Python': ['gerapy/__version__.py', 'gerapy/server/core/views.py']}
    result_type_and_files = {}
    for file_name in get_files_name:
        type = judge_program_language(file_name)
        if type not in result_type_and_files:
            result_type_and_files[type] = []
            result_type_and_files[type].append(file_name)
        else:
            result_type_and_files[type].append(file_name)
    return result_type_and_files

def get_type_from_whole_result(sanitize_result):
    # 将 sanitize_result 按照 cwe 分开，对每个 cve 的 commits 进行多线程处理
    global MAX_WORKERS
    result_from_cwe = {}
    with concurrent.futures.ThreadPoolExecutor(MAX_WORKERS) as executor:
        for cwe in sanitize_result:
            tmp_cves = sanitize_result[cwe]
            tmp_result_from_cve = {}
            for cve in tmp_cves:
                tmp_commits = tmp_cves[cve]
                tmp_result_from_commit = {}
                futures = []
                for commit in tmp_commits:
                    future = executor.submit(get_files_name_from_commit, commit)
                    futures.append((commit, future))
                for commit, future in futures:
                    get_files_name = future.result()
                    result_type_and_files = handle_commit_files(get_files_name)
                    tmp_result_from_commit[commit] = result_type_and_files
                tmp_result_from_cve[cve] = tmp_result_from_commit
            result_from_cwe[cwe] = tmp_result_from_cve  
    return result_from_cwe

def main():
    sanitize_result = {}
    total_number = 0
    cwe2cve_info = get_cwe2commits()
    for cwe in cwe2cve_info:
        tmp_cwe = cwe2cve_info[cwe]
        tmp_result = {}
        for cve in tmp_cwe:
            commits = tmp_cwe[cve]
            if commits != []:
                tmp_result[cve] = commits
        tmp_length = len(tmp_result)
        total_number = total_number + tmp_length
        # 统计数据
        print(cwe + ": " + str(tmp_length))
        sanitize_result[cwe] = tmp_result
    final_result = get_type_from_whole_result(sanitize_result)
    # 保存文件
    save_output(final_result)
    # 统计数据
    print("[+] Length " + str(total_number))
    
    
if __name__ == "__main__":
    main()
    # commit_url = "https://github.com/ImageMagick/ImageMagick/commit/9c9a84cec4ab28ee0b57c2b9266d6fbe68183512"
    # get_files_name = get_files_name_from_commit(commit_url)
    # print(get_files_name)
    # result_type_and_files = handle_commit_files(get_files_name)
    # print(result_type_and_files)
   