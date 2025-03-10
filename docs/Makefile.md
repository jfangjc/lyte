```Makefile
ARCH = x86_64  
  
CC = clang  
  
CFLAGS = -Wall  
INC = -I.  
  
OBJDIR = ../bin/  
SUBDIRS := $(wildcard src/*/)  
  
TARGET = ../bin/Lyte  
SRCFILES = $(wildcard *.c) $(foreach dir, $(SUBDIRS), $(wildcard $(dir)*.c))  
CCOBJS = $(addprefix $(OBJDIR), $(patsubst %.c, %.o, $(SRCFILES)))  
  
define Compile  
	$(CC) $(CFLAGS) $(INC) -c $< -o $@  
endef  
  
.PHONY: all clean run  
  
all: $(CCOBJS)   
	$(CC) -o $(TARGET) $^   
	@echo "Compiled"  
	@../bin/Lyte ../Test.lt  
  
$(OBJDIR)%.o: $(patsubst %.o, %.c, $(subst $(OBJDIR), ./, $(OBJDIR)%.o))  
	@mkdir -p $(@D)  
	$(Compile)  
  
clean:  
	rm -rf $(OBJDIR)*  
  
run:   
	$(TARGET) ../Test.lt
```
