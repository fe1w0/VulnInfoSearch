import time

def timer(func):
    def wrapper(*args, **kwargs):
        start_time = time.time()  # 记录开始时间
        func(*args, **kwargs)
        end_time = time.time()  # 记录结束时间
        elapsed_time = end_time - start_time  # 计算总运行时间
        print("[+] Finish.\n[+] Elapsed time: {:.2f} seconds".format(elapsed_time))  # 输出总运行时间
    return wrapper