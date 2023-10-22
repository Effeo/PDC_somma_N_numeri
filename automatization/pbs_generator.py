generated = 0

def generate_new_pbs_with_cpus(input_pbs_file, name, s, cpus, n):
    try:
        with open(input_pbs_file, 'r') as input_file, open(f"{name}.pbs", 'w') as output_file:
            for line in input_file:
                # Check if the line contains the number of CPUs, and replace it
                if line.strip().startswith("#PBS -l nodes"):
                    updated_line = f"#PBS -l nodes={cpus}\n"
                elif line.strip().startswith("#PBS -N Test"):
                    updated_line = f"#PBS -N test_{name}\n"
                elif line.strip().startswith("#PBS -o Test.out"):
                    updated_line = f"#PBS -o result_{name}.out\n"
                elif line.strip().startswith("#PBS -e Test.err"):
                    updated_line = f"#PBS -e err_{name}.err\n"
                else:
                    updated_line = line
                
                updated_line = updated_line.replace("{{exec_name}}", name);
                updated_line = updated_line.replace("{{CPUS}}", str(cpus));
                updated_line = updated_line.replace("{{strategy}}", str(s))
                updated_line = updated_line.replace("{{noOfNumbers}}", str(n))
                if(n < 20):
                    r = []
                    for _ in range(1, n+1):
                        r.append(1)
                    nso = " ".join(map(str, r))
                    updated_line = updated_line.replace("{{numbers}}", str(nso))
                else:
                    updated_line = updated_line.replace("{{numbers}}", "")
                output_file.write(updated_line)
        print(f"New PBS script with {cpus} CPUs has been generated: {name}.job")

        global generated
        generated += 1
    except FileNotFoundError:
        print("File not found. Make sure the input PBS file exists.")

if __name__ == "__main__":
    strategy = [1, 2, 3]
    cpus = [1, 2, 4, 5, 8]
    numbers = [19, 100000, 10000000, 100000000, 1000000000]

    for s in strategy:
        for cpu in cpus:
            for n in numbers:
                generate_new_pbs_with_cpus("template.pbs", f"job_s{s}_p{cpu}_n{n}", s, cpu, n)
                if(n > 19 and cpu != 1):
                    generate_new_pbs_with_cpus("template.pbs", f"job_s{s}_p{cpu}_n{n*cpu}", s, cpu, n*cpu)
                elif(n > 19):
                    generate_new_pbs_with_cpus("template.pbs", f"job_s{s}_p{cpu}_n{n*2}", s, cpu, n*2)
                    generate_new_pbs_with_cpus("template.pbs", f"job_s{s}_p{cpu}_n{n*4}", s, cpu, n*4)
                    generate_new_pbs_with_cpus("template.pbs", f"job_s{s}_p{cpu}_n{n*8}", s, cpu, n*8)
    
    print(f"Generated {generated} PBS files.")