# import re


# def get_whole_url(url):
#     try:
#         regex = r"(https:\/\/github\.com\/[^\/]+\/[^\/]+\/commit\/[0-9a-zA-Z]+)"
#         urls = re.findall(regex, url)
#         matched_part = urls[0]
#         return matched_part
#     except Exception as e:
#         print(f"[!] Error: {e} Match Error {url}")


# url = "https://github.com/viabtc/viabtc_exchange_server/commit/4a7c27bfe98f409623d4d857894d017ff0672cc9#diff-9fabc53ea796ec492aef432594298baa"
# # url = "https://github.com/libarchive/libarchive/commit/3014e198"
# test = get_whole_url(url)
# print(test)


# import difflib
# def compare_files(file1_path, file2_path):
#     with open(file1_path, 'r') as file1, open(file2_path, 'r') as file2:
#         lines1 = file1.readlines()
#         lines2 = file2.readlines()

#     diff = difflib.unified_diff(lines1, lines2)

#     for line in diff:
#         print(line)

# # 比较文件
# file1_path = '/Users/fe1w0/Project/Python/VulnInfoSearch/DataSet/CommitsCollection/C/krb5_krb5/fa62bd33a0c0889c083999c0289ffa81a5d51e7b/src_lib_krb5_krb_pac.c'
# file2_path = '/Users/fe1w0/Project/Python/VulnInfoSearch/DataSet/CommitsCollection/C/krb5_krb5/ea92d2f0fcceb54a70910fa32e9a0d7a5afc3583/src_lib_krb5_krb_pac.c'
# compare_files(file1_path, file2_path)