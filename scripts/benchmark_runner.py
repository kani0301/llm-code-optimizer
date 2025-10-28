import os, subprocess, json, time

def run_benchmark():
    os.makedirs("results", exist_ok=True)
    start = time.time()
    subprocess.run(["./scripts/perf_adapter.sh"], check=True)
    end = time.time()
    result = {
        "runtime_sec": round(end - start, 3)
    }
    with open("results/benchmark.json", "w") as f:
        json.dump(result, f, indent=4)
    print("Benchmark done:", result)

if __name__ == "__main__":
    run_benchmark()
