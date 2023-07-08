# 从 https://nvd.nist.gov/vuln/detail/[cve-id]
# 中爬取 github commits 
from lxml import html
import requests
import re
import time
import yaml
from concurrent.futures import ThreadPoolExecutor
import concurrent


CWE_CVE_COMMIT = {}

MAX_WORKERS = 10

TOTAL_COMMITS_NUMBER = 0

LOCAL_PROXIES={
    "http" : "http://127.0.0.1:7890",
    "https" : "http://127.0.0.1:7890"
}

def check_github_commit_url(url):
    # 检查字符串是否匹配"https://github.com/XXX/XXX/commit/XXX"
    pattern = r"https://github.com/.*/.*/commit/.*"
    
    if re.match(pattern, url):
        return True
    else:
        return False
    
def sanitize_url(url):
    return url.replace(",", "")

def get_commits_from_cve(cve):
    github_commit_links = []
    if cve != "":
        request_url = "https://nvd.nist.gov/vuln/detail/" + cve
        try:
            html_text = requests.get(request_url,  proxies=LOCAL_PROXIES).text
            # html_text = requests.get(request_url).text
            # 处理 html 文档
            if html_text != "":
                tree = html.fromstring(html_text)
                links = tree.xpath("//td[starts-with(@data-testid,'vuln-hyperlinks-link-')]/a")
                # 检查 是否为 github_commit
                for link in links:
                    tmp_link = sanitize_url(link.get("href"))
                    if(check_github_commit_url(tmp_link)):
                        github_commit_links.append(tmp_link)
            print("[+] Success: " + request_url)
        except Exception as e:
            print(f"[!] Error: {request_url} 查询失败：{e}, 重新尝试")
            get_commits_from_cve(cve)
    return github_commit_links
                    
def get_cwe2cve_info():
    with open('Output/cwe2cve_result.yml', 'r') as f:
        cwe2cve_info = yaml.safe_load(f)
    return cwe2cve_info

def get_info_from_cve_list(cve_list):
    global MAX_WORKERS
    res_dict = {}
    with ThreadPoolExecutor(MAX_WORKERS) as executor:  # 设置线程池大小为10
        # 提交任务给线程池
        future_to_cve = {executor.submit(get_commits_from_cve, cve_id): cve_id for cve_id in cve_list}
        # 获取已完成的任务结果
        for future in concurrent.futures.as_completed(future_to_cve):
            cve_id = future_to_cve[future]
            try:
                github_commit_links = future.result()
                res_dict[cve_id] = github_commit_links
            except Exception as e:
                print(f"[!] Error: {cve_id} 查询失败：{e}, 重新尝试")
                github_commit_links = get_commits_from_cve(cve_id)
                res_dict[cve_id] = github_commit_links
    return res_dict

def get_all_commit_links():
    global CWE_CVE_COMMIT
    cwe2cve_info = get_cwe2cve_info()
    for cwe in cwe2cve_info:
        cve_list = []
        current_cwe_info = cwe2cve_info[cwe]
        for item in current_cwe_info:
            if (item != "cve_number"):
                cve_list.append(item)
        res_dict = get_info_from_cve_list(cve_list)
        CWE_CVE_COMMIT[cwe] = res_dict
    
    # 保存 CWE_CVE_COMMIT
    save_output()    
    
def save_output():
    global CWE_CVE_COMMIT
    with open('output/cve2commit.yml', 'w') as f:
        yaml.dump(CWE_CVE_COMMIT, f, indent=4, sort_keys=False)
    
if __name__ == "__main__":
    start_time = time.time()  # 记录开始时间
    get_all_commit_links()
    end_time = time.time()  # 记录结束时间
    elapsed_time = end_time - start_time  # 计算总运行时间
    print("[+] Finish.\n[+] Elapsed time: {:.2f} seconds".format(elapsed_time))  # 输出总运行时间
