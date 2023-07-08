from lxml import html
import requests
import re
import time
import yaml
import concurrent
from bs4 import BeautifulSoup
from concurrent.futures import ThreadPoolExecutor, as_completed



MAX_WORKERS = 20

LOCAL_PROXIES={
    "http" : "http://127.0.0.1:7890",
    "https" : "http://127.0.0.1:7890"
}

def get_cwe2commits():
    with open('Output/cve2commit.yml', 'r') as f:
        cwe2cve_info = yaml.safe_load(f)
    return cwe2cve_info

def save_output(result):
    with open('Output/cve2commit_sanitize.yml', 'w') as f:
        yaml.dump(result, f, indent=4, sort_keys=False)

def timer(func):
    def wrapper(*args, **kwargs):
        start_time = time.time()  # 记录开始时间
        func(*args, **kwargs)
        end_time = time.time()  # 记录结束时间
        elapsed_time = end_time - start_time  # 计算总运行时间
        print("[+] Finish.\n[+] Elapsed time: {:.2f} seconds".format(elapsed_time))  # 输出总运行时间
    return wrapper

# 读取 url 最后40 字符串
# Example: 
# - /tensorflow/tensorflow/commit/4f27d19a9c567ce2033fc5e993499080c7f38614
# - https://github.com/tensorflow/tensorflow/commit/8dc723fcdd1a6127d6c970bd2ecb18b019a1a58d
def get_commit_hashCode_from_url(url):
    return url[-40:]

def get_files_name_and_Lang_from_commit(commit_url):
    get_files_name_and_Lang  = {}
    
    get_files_name_and_Lang["commit_url"] = commit_url
    
    if commit_url != "":
        try:
            headers = {
                "User-Agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36 Edg/114.0.1823.67"
            }
            
            response = requests.get(commit_url, headers=headers, proxies=LOCAL_PROXIES)
            response.raise_for_status()
            html_text = response.text
            
            soup = BeautifulSoup(html_text, 'html.parser')
            div_tags = soup.find_all('div')
            
            a_tag = soup.select_one('a.sha[data-hotkey="p"]')
            parent_href_attr = a_tag.get('href')
            
            parent_commit_hashCode = get_commit_hashCode_from_url(parent_href_attr)
            current_commit_hashCode = get_commit_hashCode_from_url(commit_url)
            
            get_files_name_and_Lang["commit_hashCode"] = {"parent_commit_hashCode": parent_commit_hashCode, "current_commit_hashCode": current_commit_hashCode}
            
            for div_tag in div_tags:
                data_tagsearch_path = div_tag.get('data-tagsearch-path')
                data_tagsearch_lang = div_tag.get('data-tagsearch-lang')

                if data_tagsearch_lang == "":
                    data_tagsearch_lang = "unknown"
                
                if data_tagsearch_path and data_tagsearch_lang:
                    get_files_name_and_Lang[data_tagsearch_path] = {"path": data_tagsearch_path, "lang": data_tagsearch_lang}
            print("[+] Success: " + commit_url)
        except Exception as e:
            print(f"[!] Error: {commit_url} 查询失败：{e}, 重新尝试")
            # get_files_name_and_Lang_from_commit(commit_url)
    return get_files_name_and_Lang


def get_type_from_whole_result(sanitize_result):
    global MAX_WORKERS

    result_from_cwe = {}

    with ThreadPoolExecutor(max_workers=MAX_WORKERS) as executor:
        # 处理每个cwe
        for cwe in sanitize_result:
            tmp_cves = sanitize_result[cwe]
            tmp_result_from_cve = {}

            # 处理每个cve
            for cve in tmp_cves:
                tmp_commits = tmp_cves[cve]
                tmp_result_from_commit = {}

                # 提交任务并获取future对象
                futures = [executor.submit(get_files_name_and_Lang_from_commit, commit) for commit in tmp_commits]

                # 处理完成的任务
                for future in as_completed(futures):
                    commit_result = future.result()
                    
                    if "commit_hashCode" in commit_result:
                        commit_url = commit_result["commit_url"]
                        tmp_result_from_commit[commit_url] = commit_result
                    
                tmp_result_from_cve[cve] = tmp_result_from_commit

            result_from_cwe[cwe] = tmp_result_from_cve
    return result_from_cwe
    
@timer
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