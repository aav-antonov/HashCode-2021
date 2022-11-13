# *****************************************************
# Variables to control Makefile operation
NVCC = nvcc  -x cu -arch=sm_60 

# ****************************************************
# Targets needed to bring the executable up to date
all: solver

solver:     
	$(NVCC)   main.cpp -o solver  -lboost_system -lboost_filesystem 




