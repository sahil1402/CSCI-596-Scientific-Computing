#!/usr/bin/env python3
"""Plot grid-refinement results and summarize verification checks."""
import argparse
from pathlib import Path
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import pandas as pd
def main()->None:
    p=argparse.ArgumentParser();p.add_argument("verification_directory",type=Path);p.add_argument("output_directory",type=Path);a=p.parse_args();a.output_directory.mkdir(parents=True,exist_ok=True);grid=pd.read_csv(a.verification_directory/"grid_refinement.csv");checks=pd.read_csv(a.verification_directory/"verification.csv");fig,axes=plt.subplots(1,3,figsize=(12,3.8));cells=grid.nx*grid.ny
    for ax,column,label in zip(axes,["peak_abs_elevation","max_entrance_speed","mean_protected_energy"],["Peak elevation (m)","Max entrance speed (m/s)","Mean energy (J/m²)"]):ax.plot(cells,grid[column],"o-",lw=2);ax.set(xlabel="Grid cells",ylabel=label);ax.grid(alpha=.25)
    fig.suptitle("Grid-Refinement Sensitivity");fig.tight_layout();fig.savefig(a.output_directory/"grid_refinement.png",dpi=180,bbox_inches="tight");plt.close(fig);(a.output_directory/"verification_summary.txt").write_text("HarborTune verification\n"+"="*24+"\n"+"\n".join(f"{r.check}: {'PASS' if r.passed else 'FAIL'} (relative error {r.relative_error:.3g}, tolerance {r.tolerance:.3g})" for r in checks.itertuples())+"\n",encoding="utf-8");print(f"Wrote verification outputs to {a.output_directory.resolve()}")
if __name__=="__main__":main()
