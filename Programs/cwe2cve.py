import yaml
from urllib.parse import urlencode
import requests
from lxml import html
import time
import threading
from concurrent.futures import ThreadPoolExecutor, as_completed

TIMEOUT = 20

TIMESLEEP = 1

SEARCH_NVD_URL = "https://nvd.nist.gov/vuln/search/results?isCpeNameSearch=false&results_type=overview&form_type=Advanced&search_type=all&"

# 避免重复
TOTAL_CVE_LENGTH = 0

MAX_WORKERS = 15

lock = threading.Lock()  # 锁，防止结果同时写入

# LOCAL_PROXIES={
#     "http" : "http://127.0.0.1:7890",
#     "https" : "http://127.0.0.1:7890"
#     }

def read_cwe_info():
    with open('DataSet/cwe_id.yml', 'r') as f:
        software_data = yaml.safe_load(f)
    return software_data

def get_cve_from_html(html_text):
    cve_dict = {}
    tree = html.fromstring(html_text)
    cve_id_list = tree.xpath("//a[starts-with(@data-testid,'vuln-detail-link-')]/text()")
    cve_summary_list = tree.xpath("//p[starts-with(@data-testid,'vuln-summary-')]/text()")
    for cve_id, summary in zip(cve_id_list, cve_summary_list):
        cve_id = str(cve_id)
        summary = str(summary)
        cve_dict[cve_id] = {"cve_id": cve_id, "summary": summary}
    return cve_dict
    
    
def get_all_cve_requests(cwe_id, index, result_dict, semaphore):
    semaphore.acquire()  # 获取信号量
    search_url = SEARCH_NVD_URL + urlencode({"cwe_id" : cwe_id, "startIndex" : index})
    try:
        # res = requests.get(search_url, proxies=LOCAL_PROXIES, timeout=TIMEOUT)
        res = requests.get(search_url, timeout=TIMEOUT)
        tmp_dict = get_cve_from_html(res.text)
        with lock:
            result_dict.update(tmp_dict)
        time.sleep(TIMESLEEP)
        print("[+] Success: " + search_url)
    except Exception as e:
        print(f"[!] Error: {search_url}查询失败：{e}, 重新尝试")
        get_all_cve_requests(cwe_id, index, result_dict, semaphore)
    finally:
        semaphore.release()  # 释放信号量
    
def get_all_cve(cwe_id, max_index, result_dict):
    semaphore = threading.Semaphore(MAX_WORKERS)  
    wait_request_indexs = range(0, max_index + 1, 20)
    futures = []
    with ThreadPoolExecutor(max_workers=MAX_WORKERS) as executor:
        for request_index in wait_request_indexs:
            futures.append(executor.submit(get_all_cve_requests, cwe_id, request_index, result_dict, semaphore))

    for future in as_completed(futures):
        pass
          
    return result_dict
    
def get_cve_info(cwe_id, html_text):
    cve_dict = {}
    tree = html.fromstring(html_text)
    cve_number = tree.xpath("//strong[@data-testid='vuln-matching-records-count']/text()")
    cve_number = cve_number[0].replace(",", "")
    cve_number = int(cve_number)
    # 记录 总数
    cve_dict['cve_number'] = cve_number
    print("CWE_ID: " + cwe_id +  ", Length: " + str(cve_number))
    if cve_number <= 20:
        cve_id_list = tree.xpath("//a[starts-with(@data-testid,'vuln-detail-link-')]/text()")
        cve_summary_list = tree.xpath("//p[starts-with(@data-testid,'vuln-summary-')]/text()")
        # generate result
        for cve_id, summary in zip(cve_id_list, cve_summary_list):
            cve_id = str(cve_id)
            summary = str(summary)
            cve_dict[cve_id] = {"cve_id": cve_id, "summary": summary}
    else:
        # NVD 数据库 每次查询最多有 20 个记录
        max_index = (int( cve_number / 20 ) + 1 ) * 20
        get_all_cve(cwe_id, max_index, cve_dict)
    return cve_dict
        
    
def search_cve_from_NVD(cwe_id):
    search_url = SEARCH_NVD_URL + urlencode({"cwe_id" : cwe_id})
    # res = requests.get(search_url, proxies=LOCAL_PROXIES, timeout=TIMEOUT)
    res = requests.get(search_url, timeout=TIMEOUT)
    if res.text != "" :
        cve_dict = get_cve_info(cwe_id, res.text)
    else:
        # res = requests.get(search_url, proxies=LOCAL_PROXIES, timeout=TIMEOUT)
        res = requests.get(search_url, timeout=TIMEOUT)
        cve_dict = get_cve_info(cwe_id, res.text)
    return cve_dict
    

def main():
    global TOTAL_CVE_LENGTH
    cwe2cvw_file_info = {}
    cwe_info = read_cwe_info()
    for cwe in cwe_info:
        cwe_id = cwe['cwe_id']
        time.sleep(TIMESLEEP)
        cve_dict = search_cve_from_NVD(cwe['cwe_id'])
        cwe2cvw_file_info[cwe_id] = cve_dict
        TOTAL_CVE_LENGTH = TOTAL_CVE_LENGTH + cve_dict['cve_number']
                
    # print(cwe2cvw_file_info)
    with open('Output/cwe2cve_result.yml', 'w') as f:
        yaml.dump(cwe2cvw_file_info, f, indent=4, sort_keys=False)
        
if __name__ == "__main__":
    start_time = time.time()  # 记录开始时间
    main()
    end_time = time.time()  # 记录结束时间
    elapsed_time = end_time - start_time  # 计算总运行时间
    print("[+] Length " + str(TOTAL_CVE_LENGTH) + "\n[+] Finish.\n[+] Elapsed time: {:.2f} seconds".format(elapsed_time))  # 输出总运行时间
