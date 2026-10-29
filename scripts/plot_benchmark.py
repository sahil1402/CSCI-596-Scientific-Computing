#!/usr/bin/env python3
"""Plot HarborTune OpenMP scaling results."""
import argparse
from pathlib import Path
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import pandas as pd
def main()->None:
    p=argparse.ArgumentParser();p.add_argument("benchmark_csv",type=Path);p.add_argument("output_directory",type=Path);a=p.parse_args();a.output_directory.mkdir(parents=True,exist_ok=True);d=pd.read_csv(a.benchmark_csv).sort_values("threads")
    plt.rcParams.update({"axes.grid":True,"grid.alpha":.25,"axes.spines.top":False,"axes.spines.right":False});fig,(x,y)=plt.subplots(1,2,figsize=(10,4));x.errorbar(d.threads,d.mean_seconds,yerr=d.stddev_seconds,marker="o",capsize=4);x.set(xlabel="OpenMP threads",ylabel="Mean sweep runtime (s)",title="Runtime Scaling");x.set_xticks(d.threads);y.plot(d.threads,d.speedup,"o-",label="Measured");y.plot(d.threads,d.threads,"--",color="grey",label="Ideal");y.set(xlabel="OpenMP threads",ylabel="Speedup",title="Strong Scaling");y.set_xticks(d.threads);y.legend(frameon=False);fig.suptitle(f"HarborTune 60-Case Sweep ({int(d.nx.iloc[0])}×{int(d.ny.iloc[0])} grid)");fig.tight_layout();fig.savefig(a.output_directory/"openmp_scaling.png",dpi=180,bbox_inches="tight");plt.close(fig);print(f"Wrote benchmark figure to {a.output_directory.resolve()}")
if __name__=="__main__":main()
