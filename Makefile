# Defining the object files for this application
SRCS = main.c chunk.c debug.c memory.c value.c vm.c scanner.c compiler.c
OBJS = $(SRCS:.c=.o) # Automatically creates a list of object files (.o) from source files (.c)

# Define the executable
EXE = clox

# Final build step (links the object files)
$(EXE): $(OBJS)
	gcc -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer -o $(EXE) $(OBJS)

# Generic rule to compile a .c file into a .o file
%.o: %.c
	gcc -g -c $< -o $@

# Cleaning the build
clean:
	rm -f $(EXE) $(OBJS)

# Main build target (runs the default rule)
.PHONY: all
all: $(EXE)

# Dedicated build step (compiles only)
.PHONY: build
build: all

# Runs the compiled application
.PHONY: run
run: all
	./$(EXE)

# Runs app with arg
.PHONY: run_file
run_file: all
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