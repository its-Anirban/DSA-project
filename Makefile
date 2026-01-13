# Makefile for Student Admission System

CC = gcc
CFLAGS = -Wall -Wextra -I./headers
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Source files
SOURCES = $(SRCDIR)/main.c \
          $(SRCDIR)/admin_menu.c \
          $(SRCDIR)/applicant_ops.c \
          $(SRCDIR)/auth.c \
          $(SRCDIR)/csv_handler.c \
          $(SRCDIR)/department.c \
          $(SRCDIR)/meritlist.c \
          $(SRCDIR)/sorting.c \
          $(SRCDIR)/stud_menu.c \
          $(SRCDIR)/utils.c

# Object files
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

# Executable name
EXECUTABLE = $(BINDIR)/admission_system

# Default target
all: $(EXECUTABLE)

# Create directories if they don't exist
$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(BINDIR):
	@mkdir -p $(BINDIR)

# Link the executable
$(EXECUTABLE): $(OBJDIR) $(BINDIR) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)
	@echo "Build complete! Executable: $@"

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: $(EXECUTABLE)
	@./$(EXECUTABLE)

# Generate applicant data
data:
	@echo "Compiling data generator..."
	@$(CC) $(CFLAGS) tools/generate_applicants.c $(SRCDIR)/data_generator.c -o $(BINDIR)/generate_data
	@echo "Running data generator..."
	@./$(BINDIR)/generate_data
	@echo "Data generation complete!"

# Clean build artifacts
clean:
	@rm -rf $(OBJDIR) $(BINDIR)
	@echo "Cleaned build artifacts"

# Clean and rebuild
rebuild: clean all

# API Server sources
API_SOURCES = $(SRCDIR)/api_server.c \
              $(SRCDIR)/csv_handler.c \
              $(SRCDIR)/sorting.c \
              $(SRCDIR)/utils.c \
              mongoose/mongoose.c

# Build API server executable
$(BINDIR)/api_server: $(BINDIR) $(API_SOURCES)
	$(CC) $(CFLAGS) -I./mongoose -o $@ $(API_SOURCES)
	@echo "API Server build complete!"

# Build and run API server (connects frontend to CSV files)
api: $(BINDIR)/api_server
	@./$(BINDIR)/api_server

# Stop server running on port 8080
stop:
	@echo "Stopping server on port 8080..."
	@-lsof -ti:8080 | xargs -r kill -9 2>/dev/null || true
	@echo "Server stopped."

# Phony targets
.PHONY: all run clean rebuild data api stop
