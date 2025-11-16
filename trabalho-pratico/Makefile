CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g

# Programa principal
MAIN_SRCS = $(filter-out src/main_testes.c src/comparador.c src/metricas.c src/executor_testes.c, $(wildcard src/*.c))
MAIN_OBJDIR = src/obj
MAIN_OBJS = $(patsubst src/%.c,$(MAIN_OBJDIR)/%.o,$(MAIN_SRCS))
MAIN_TARGET = programa-principal

# Programa de testes
# Build all sources except the production `main.c` so tests can link program logic
TEST_SRCS = $(filter-out src/main.c, $(wildcard src/*.c))
TEST_OBJDIR = src/obj_testes
TEST_OBJS = $(patsubst src/%.c,$(TEST_OBJDIR)/%.o,$(TEST_SRCS))
TEST_TARGET = programa-testes

.PHONY: all tester clean

all: $(MAIN_TARGET)

# Programa principal
$(MAIN_TARGET): $(MAIN_OBJS)
	$(CC) $(MAIN_OBJS) -o $(MAIN_TARGET)

$(MAIN_OBJDIR)/%.o: src/%.c | $(MAIN_OBJDIR)
	@printf "CC $< -> $@\n"
	$(CC) $(CFLAGS) -c $< -o $@

$(MAIN_OBJDIR):
	mkdir -p $(MAIN_OBJDIR)

# Programa de testes (build with 'make tester')
tester: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(TEST_OBJS) -o $(TEST_TARGET)

$(TEST_OBJDIR)/%.o: src/%.c | $(TEST_OBJDIR)
	@printf "CC $< -> $@\n"
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_OBJDIR):
	mkdir -p $(TEST_OBJDIR)

clean:
	rm -rf $(MAIN_OBJDIR) $(TEST_OBJDIR)
	rm -f $(MAIN_TARGET) $(TEST_TARGET)
	rm -rf resultados/*

.PHONY: all tester clean
