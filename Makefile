#/*****************************************************************************
#* Copyright © 2021 Mike Sharkey <mike@8bitgeek.net>                          *
#*                                                                            *
#* Permission is hereby granted, free of charge, to any person obtaining a    *
#* copy of this software and associated documentation files (the "Software"), *
#* to deal in the Software without restriction, including without limitation  *
#* the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
#* and/or sell copies of the Software, and to permit persons to whom the      *
#* Software is furnished to do so, subject to the following conditions:       *
#*                                                                            *
#* The above copyright notice and this permission notice shall be included in *
#* all copies or substantial portions of the Software.                        *
#*                                                                            *
#* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
#* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
#* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
#* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
#* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    *
#* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        *
#* DEALINGS IN THE SOFTWARE.                                                  *
#*****************************************************************************/
TARGETS=ezbus_runner_1.elf \
		ezbus_runner_2.elf

LIBEZBUS_DIR=./libezbus
LIBEZBUS_INCLUDE=$(LIBEZBUS_DIR)/src
LIBEZBUS_TARGET=$(LIBEZBUS_DIR)/libezbus.a

LIBEZBUS_UDP_DIR=./libezbus_udp
LIBEZBUS_UDP_INCLUDE=$(LIBEZBUS_UDP_DIR)/src
LIBEZBUS_UDP_TARGET=libezbus_udp/libezbus_udp.a

PREFIX=/usr/bin/

CC=$(PREFIX)gcc
LD=$(PREFIX)gcc
AR=$(PREFIX)ar
AS=$(PREFIX)as
CP=$(PREFIX)objcopy
OD=$(PREFIX)objdump
SE=$(PREFIX)size

ARFLAGS = rcs
CFLAGS += -c
CFLAGS += -std=gnu99 -ggdb -O0 -Wall -Wno-unused-function
LFLAGS += $(LIBEZBUS_TARGET) $(LIBEZBUS_UDP_TARGET)

INCLUDE =  -I ./
INCLUDE += -I ./src
INCLUDE += -I $(LIBEZBUS_INCLUDE)
INCLUDE += -I $(LIBEZBUS_UDP_INCLUDE)

C_SRC += src/ezbus_runner_1.c
C_SRC += src/main.c

# Object files to build.
OBJS  = $(AS_SRC:.S=.o)
OBJS += $(C_SRC:.c=.o)

# Default rule to build the whole project.
.PHONY: all
all: $(LIBEZBUS_TARGET) $(LIBEZBUS_UDP_TARGET) $(TARGETS)

# Rule to build assembly files.
%.o: %.S
	$(CC) -x assembler-with-cpp $(ASFLAGS) $(INCLUDE) $< -o $@

# Rule to compile C files.
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@

# Rules to make libraries
$(LIBEZBUS_TARGET):
	(cd $(LIBEZBUS_DIR) && make)

$(LIBEZBUS_UDP_TARGET):
	(cd $(LIBEZBUS_UDP_DIR) && make)

# Rule to create an ELF file from the compiled object files.
ezbus_runner_1.elf: src/main.o src/ezbus_runner_1.o
	$(CC) $^ $(LFLAGS) -o $@

ezbus_runner_2.elf: src/main.o src/ezbus_runner_2.o 
	$(CC) $^ $(LFLAGS) -o $@

clean:
		rm -f $(OBJS) $(TARGET) ezbus_runner_[0-9] *.map
		(cd $(LIBEZBUS_DIR) && make clean)
		(cd $(LIBEZBUS_UDP_DIR) && make clean)

