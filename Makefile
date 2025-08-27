# defining the object files for this application
OBJS = main.c chunk.c debug.c memory.c value.c vm.c

# define the executables
EXE = clox

# the final build step
$(EXE): $(OBJS)
	gcc -o $(EXE) $(OBJS)

# cleaning the build
clean:
	rm -f $(EXE)

# runs the compiled application
run: clean $(EXE)
	./$(EXE)

# runs app with arg
run_file: clean $(EXE)
	./$(EXE) ./test

########## For HPC project ############

# defining the object files for this application
OBJS_HPC = hpc.cpp

# define the executables
EXE_HPC = hpc

# the final build step
$(EXE_HPC): $(OBJS_HPC)
	g++ -o $(EXE_HPC) $(OBJS_HPC)

# cleaning the build
clean_hpc:
	rm -f $(EXE_HPC)

# runs the compiled application
run_hpc: clean $(EXE_HPC)
	./$(EXE_HPC)