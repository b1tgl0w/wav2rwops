CC := gcc
ifeq ($(MAKELEVEL),0)
export CC_FLAGS = --std=c89 -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-overflow=5 -Wswitch-default -Wundef -Wunused -Wmaybe-uninitialized -Wuninitialized -DREENTRANT

endif

.PHONY: all clean
all: post-build

debug-mode: CC_FLAGS += -DDEBUG_MODE
debug-mode: all

pre-build:
	@echo PREBUILD
	mkdir -p obj

post-build: main-build
	@echo POSTBUILD

main-build: pre-build
	@$(MAKE) --no-print-directory main-target

main-target: obj/wavToRwops

obj/wavToRwops: obj/main.o
	$(CC) $(CC_FLAGS) obj/main.o -o obj/wavToRwops -lpthread -ldl -lm

obj/main.o: main.c
	$(CC) $(CC_FLAGS) -c main.c -o obj/main.o

clean:
	rm obj/main.o obj/wavToRwops

