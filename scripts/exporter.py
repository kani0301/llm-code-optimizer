import csv, json

def merge_reports():
    data = {}
    with open("results/summary.csv") as f:
        reader = csv.reader(f)
        for row in reader:
            data[row[1]] = row[0]
    with open("results/benchmark.json") as f:
        runtime = json.load(f)
    final = {
        "perf": data,
        "runtime": runtime
    }
    with open("results/final_report.json", "w") as f:
        json.dump(final, f, indent=4)
    print("Final report saved as results/final_report.json")

if __name__ == "__main__":
    merge_reports()
