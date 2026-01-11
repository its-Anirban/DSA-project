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

# Executable name (Added .exe for Windows)
EXECUTABLE = $(BINDIR)\admission_system.exe

# Default target
all: $(EXECUTABLE)

# Create directories using Windows syntax
$(OBJDIR):
	@if not exist $(OBJDIR) mkdir $(OBJDIR)

$(BINDIR):
	@if not exist $(BINDIR) mkdir $(BINDIR)

# Link the executable
$(EXECUTABLE): $(OBJDIR) $(BINDIR) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)
	@echo Build complete! Executable: $(EXECUTABLE)

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: $(EXECUTABLE)
	@$(EXECUTABLE)

# Generate applicant data
data:
	@echo Compiling data generator...
	@$(CC) $(CFLAGS) tools/generate_applicants.c $(SRCDIR)/data_generator.c -o $(BINDIR)\generate_data.exe
	@echo Running data generator...
	@$(BINDIR)\generate_data.exe
	@echo Data generation complete!

# Clean build artifacts using Windows 'rmdir' and 'del'
clean:
	@if exist $(OBJDIR) rmdir /s /q $(OBJDIR)
	@if exist $(BINDIR) rmdir /s /q $(BINDIR)
	@echo Cleaned build artifacts.

# Clean and rebuild
rebuild: clean all

# Phony targets
.PHONY: all run clean rebuild data