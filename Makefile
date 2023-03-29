CC = gcc
CFLAGS = -Wall -Wextra -pedantic
LIBS = 
SRCS = $(wildcard src/*.c)
DEPS = $(SRCS:src/%.c=.deps/%.d)
OBJS = $(SRCS:src/%.c=obj/%.o)
IFLAGS = -Iinclude $(shell pkg-config --cflags $(LIBS))
LDFLAGS = $(shell pkg-config --libs $(LIBS))

all : main

include $(DEPS)

$(DEPS) : .deps/%.d : obj/%.o

$(OBJS) : obj/%.o : src/%.c
	@mkdir -p obj
	@mkdir -p .deps
	$(CC) $(CFLAGS) $(IFLAGS) -MMD -MF $(<:src/%.c=.deps/%.d) -c $< -o $@

main : $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean :
	rm -rf .deps obj main

.PHONY : all clean
