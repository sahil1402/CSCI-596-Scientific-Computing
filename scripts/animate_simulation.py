#!/usr/bin/env python3
"""Render elevation or current-speed snapshots as an animated GIF."""
import argparse
from pathlib import Path
import imageio.v2 as imageio
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
def main()->None:
    p=argparse.ArgumentParser();p.add_argument("simulation_directory",type=Path);p.add_argument("--output",type=Path,required=True);p.add_argument("--field",choices=["eta","speed"],default="eta");p.add_argument("--fps",type=int,default=8);a=p.parse_args();a.output.parent.mkdir(parents=True,exist_ok=True)
    geometry=pd.read_csv(a.simulation_directory/"geometry.csv");times=pd.read_csv(a.simulation_directory/"snapshot_times.csv");nx=int(geometry.i.max()+1);ny=int(geometry.j.max()+1);solid=geometry.solid.to_numpy().reshape(ny,nx).astype(bool);protected=geometry.protected.to_numpy().reshape(ny,nx).astype(bool);data=[];maximum=0.0
    for row in times.itertuples():
        frame=pd.read_csv(a.simulation_directory/row.file);values=frame[a.field].to_numpy().reshape(ny,nx);maximum=max(maximum,float(np.nanmax(np.abs(values))));data.append((row.time,values))
    maximum=max(maximum,1e-12);images=[]
    for time,values in data:
        fig,ax=plt.subplots(figsize=(8,4.7),dpi=120);masked=np.ma.masked_where(solid,values);cmap="RdBu_r" if a.field=="eta" else "viridis";vmin=-maximum if a.field=="eta" else 0;im=ax.imshow(masked,origin="lower",cmap=cmap,vmin=vmin,vmax=maximum,aspect="auto");ax.contour(solid.astype(float),levels=[.5],colors="black",linewidths=1.1,origin="lower");ax.contour(protected.astype(float),levels=[.5],colors="#f5c542",linewidths=.8,linestyles="--",origin="lower");ax.set(title=f"{'Water-surface elevation' if a.field=='eta' else 'Current speed'} — t = {time:.1f} s",xlabel="x grid index",ylabel="y grid index");fig.colorbar(im,ax=ax,label="Elevation (m)" if a.field=="eta" else "Speed (m/s)");fig.tight_layout();fig.canvas.draw();images.append(np.asarray(fig.canvas.buffer_rgba())[...,:3].copy());plt.close(fig)
    imageio.mimsave(a.output,images,duration=1000/a.fps,loop=0);print(f"Wrote {len(images)} frames to {a.output.resolve()}")
if __name__=="__main__":main()
