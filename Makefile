CC := aarch64-linux-gnu-gcc
SRC := 	$(shell find src -name "*.c")
INC := ./inc \
		./3rd/usr/local/include \
		./3rd/usr/include \
		./3rd/usr/include/python3.10 \
		./3rd/usr/include/aarch64-linux-gnu/python3.10 \
		./3rd/usr/include/aarch64-linux-gnu \

OBJ := $(subst src/, obj/, $(SRC:.c=.o))

TARGET = ./obj/smartHome

CFLAGS := $(foreach item, $(INC), -I$(item))

LIBS_PATH := ./3rd/usr/local/lib \
			./3rd/lib/aarch64-linux-gnu \
			./3rd/usr/lib/aarch64-linux-gnu \
			./3rd/usr/lib/python3.10 \

LDFLAGS := $(foreach item, $(LIBS_PATH), -L$(item))

LIBS := -lwiringPi -lpython3.10 -lpthread -lexpat -lz -lcrypt

obj/%.o : src/%.c
	$(CC) -o $@ -c $^ $(CFLAGS)

$(TARGET) : $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LIBS)

compile : $(TARGET)

clean : rm $(TARGET) obj/$(OBJ) -rf

.PHONY : compile clean