SRC := $(wildcard *.c)

OBJ := ${SRC:.c=.o}

TARGETDIR := ../bin
TARGET := $(TARGETDIR)/espresso0505_checkdistance.exe

CFLAGS += -pg
LDFLAGS += -pg

all: prepare $(TARGET)

prepare:
	mkdir -p $(TARGETDIR)

$(TARGET): $(OBJ)
	$(LINK.c) $(OBJ) -o $(TARGET) $(LDFLAGS)

clean: 
	@- $(RM) $(TARGET)
	@- $(RM) $(OBJ)
	@- $(RM) gmon.out

