import subprocess
import concurrent.futures

def run_siege():
    command = "siege --delay=0.5 --concurrent=255 --verbose http://localhost:8080/index.html"
    result = subprocess.run(command, shell=True, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output = result.stdout.decode()
    return output

with concurrent.futures.ThreadPoolExecutor() as executor:
    futures = [executor.submit(run_siege) for _ in range(1000)]
    for future in concurrent.futures.as_completed(futures):
        print(future.result())