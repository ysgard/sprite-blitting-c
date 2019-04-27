SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
PROG = sprite-blitting
CFLAGS = -g
LDLIBS = -lSDL2 -lSDL2_image

all: $(PROG)

$(PROG): $(OBJS)
