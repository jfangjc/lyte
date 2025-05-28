CC := clang

CFLAGS := -Wall -Wextra -g -I./src #-fsanitize=address
LDFLAGS := #-fsanitize=address

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
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR) test.s

test:
	$(TARGET) ./test.lt

.PHONY: all clean test
