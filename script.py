#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import re
import subprocess
import os

# Define the array of thread counts
nThreads = [2, 4, 8, 16, 32, 64, 128, 256]

nFiles = [64000000, 128000000, 256000000, 512000000]

for nFile in nFiles:

    # Define the command template
    command_template = ["srun", "-p", "teen", "-c", "32", "time", "./p", str(nFile), "%s"]  # Corrected nfile to str(nFile)

    if os.path.exists("teen" + str(nFile) + ".txt"):  # Corrected nfile to str(nFile)
        os.remove("teen" + str(nFile) + ".txt")  # Corrected nfile to str(nFile)

    # Execute the command for each thread count
    for nThread in nThreads:
        print(nThread)
        command_template[8] = str(nThread)
        command = command_template.copy()  # Corrected to make a copy of the template
        print(command)
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        output, stderr = process.communicate()
        with open("teen" + str(nFile) + ".txt", "a") as f:  # Corrected nfile to str(nFile)
            f.write("NUMERO DE THREADS: ")
            f.write(str(nThread))
            output_str = output.decode("utf-8")  # Decode the bytes object to a string
            f.write(output_str)
            print(output_str)
            f.write("\n-------------------------------------------\n\n")
        with open("errorTeen" + str(nFile) + ".txt", "a") as e:  # Corrected nfile to str(nFile)
            if stderr:
                e.write("NUMERO DE THREADS: ")
                e.write(str(nThread))
                output_str = stderr.decode("utf-8")  # Decode the bytes object to a string
                e.write(output_str)
                print(output_str)
                e.write("-------------------------------------------\n\n")

exit(0)

