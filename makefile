# *****************************************************
# Variables to control Makefile operation
NVCC = nvcc

# ****************************************************
# Targets needed to bring the executable up to date
all: solver

solver:     
	$(NVCC)   solver.cu -o solver  -lboost_system -lboost_filesystem 




