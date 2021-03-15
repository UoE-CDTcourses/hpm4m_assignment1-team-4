n="/home/donald/Desktop/HPC/simplestMPI/CPP-version/copy_to_cirrus/run_job_t<blank>.slurm"
for i in [1,4,12,36]:
    with open(n) as f:
        s=f.read()
    with open(n.replace("<blank>",str(i)),mode="w") as f:
        f.write(s.replace("<blank>",str(i)))
