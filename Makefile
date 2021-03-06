#-------------- Config
# executable name
TARGET			:= http-server
# compiler to use
CC				:= gcc
# compiler flags
CFLAGS   		:= -std=gnu11 -Wall -pedantic -MD
# linker to use
LINKER   		:= gcc
# linking flags
LFLAGS			:= -I./
# directories
SRCDIR   		:= .
INCDIR			:= .
OBJDIR			:= obj

#-------------- Helper scripts
rm				:= rm -rf
mkdir			:= mkdir -p
findc			:= du -a $(SRCDIR) | grep -E '\.(c)$$' | awk '{print $$2}'
findh			:= du -a $(INCDIR) | grep -E '\.(h)$$' | awk '{print $$2}'

#-------------- Populate build sources
SOURCES  		:= $(shell $(findc))
INCLUDES 		:= $(shell $(findh))

#-------------- What are my objects?
OBJECTS  		:= $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
#$(shell $(mkdir) $(OBJDIR))

#-------------- make subfolders
XTRADIR  := $(shell find $(SRCDIR) -type d | grep -v -E ".git|.vscode|^.$$" | awk '{sub(/.\//,"obj/")}1')
$(shell $(mkdir) $(OBJDIR) $(XTRADIR))

#-------------- Rules
all: build

# debug build
debug: CFLAGS += -DDEBUG -g
debug: LFLAGS += -DDEBUG -g
debug: build

# release build
release: CFLAGS += -DRELEASE -O3
release: build

# compile and link
build: $(OBJECTS) $(TARGET)

# rebuild.
rebuild: clean
	@$(MAKE) build

# compile objects.
$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	$(info Compiled $< -> $@)

# link objects.
$(TARGET): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	$(info Linking complete -> $(TARGET))

.PHONY:	clean count

# clean all building materials.
clean:
	@$(rm) $(OBJDIR)
	@echo "Objects removed!"
	@$(rm) $(TARGET)
	@echo "Executable removed!"

# display the current word count.
count:
	@echo "Word count:"
	@wc -l *.c *.h

-include $(OBJECTS:.o=.d)
