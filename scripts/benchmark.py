import json, os, subprocess, time

TARGET = "./analyzer/sample_target"
OUTFILE_BEFORE = "before_perf.json"
OUTFILE_AFTER = "after_perf.json"

def run_perf(outfile):
    subprocess.run(["./scripts/profiler_linux.sh", TARGET, outfile])

def delta(before, after):
    with open(before) as f1, open(after) as f2:
        b, a = json.load(f1), json.load(f2)
    return {k: round(((a[k]-b[k])/b[k])*100, 2) if b[k]!=0 else 0 for k in b}

print("Running benchmark before optimization...")
run_perf(OUTFILE_BEFORE)
time.sleep(2)
print("Running benchmark after optimization...")
run_perf(OUTFILE_AFTER)

diff = delta(OUTFILE_BEFORE, OUTFILE_AFTER)
print("Performance delta (%):")
print(json.dumps(diff, indent=2))

with open("perf_delta.json", "w") as f:
    json.dump(diff, f, indent=2)
