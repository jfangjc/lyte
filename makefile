CC := clang

CFLAGS := -Wall -Wextra -g -I./src/compiler -fsanitize=address
LDFLAGS := -fsanitize=address

ifeq ($(shell arch), aarch64)
	ARCHFLAGS := -I./src/arch/aarch64
else
	ARCHFLAGS :=-I./src/arch/universal
endif

SRCDIR := ./src
OBJDIR := ./obj
BINDIR := ./bin

EXECUTABLE := lyte
TARGET := $(BINDIR)/$(EXECUTABLE)

SRCS := $(shell find $(SRCDIR) -type f -name '*.c')

OBJS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CC) $(LDFLAGS) $(OBJS) -o $@
	@echo "Executable '$(TARGET)' created successfully."

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(ARCHFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR) test.s

test:
	$(TARGET) ./test.lt

run:
	make clean && make all && make test

.PHONY: all clean test
