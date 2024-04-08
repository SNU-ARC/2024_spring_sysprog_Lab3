#---------------------------------------------------------------------------------------------------
# System Programming                      Memory Lab                                    Spring 2024
#
# Makefile
#
# GNU make driver
# Author: Bernhard Egger <bernhard@csap.snu.ac.kr>
# Change Log:
#   2020/09/27 Bernhard Egger created
#   2021/10/03 Bernhard Egger move deps and object files into separate directories
#
# License
# Copyright (c) 2020-2023, Computer Systems and Platforms Laboratory, SNU
# All rights reserved
#
# Redistribution and use in source and binary forms, with or without modification, are permitted
# provided that the following conditions are met:
#
# - Redistributions of source code must retain the above copyright notice, this list of condi-
#   tions and the following disclaimer.
# - Redistributions in binary form must reproduce the above copyright notice, this list of condi-
#   tions and the following disclaimer in the documentation and/or other materials provided with
#   the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED  TO,  THE IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT,  INDIRECT, INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR CONSE-
# QUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE,  DATA, OR PROFITS; OR BUSINESS INTERRUPTION)  HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
# DAMAGE.
#---------------------------------------------------------------------------------------------------


#---------------------------------------------------------------------------------------------------
# Put your source and header files into the SRC_DIR (=src/) directory and make sure that SOURCES
# includes ALL C source files required to compile your project.
#
SOURCES=memmgr.c dataseg.c blocklist.c nulldriver.c
#---------------------------------------------------------------------------------------------------


#--- variable declarations

# directories
SRC_DIR=src
OBJ_DIR=obj
DEP_DIR=.deps
DRV_DIR=driver

# C compiler and compilation flags
CC=gcc
CFLAGS=-Wall -Wno-stringop-truncation -O2 -g
LINKFLAGS=-lpthread -ldl -rdynamic
DEPFLAGS=-MMD -MP -MT $@ -MF $(DEP_DIR)/$*.d

# derived variables & constants
DRV_OBJ=$(DRV_DIR)/mm_driver.o $(DRV_DIR)/mm_util.o
TARGET_MAIN=mm_test.c
TARGET_OBJ=$(TARGET_MAIN:%.c=$(OBJ_DIR)/%.o)
OBJECTS=$(SOURCES:%.c=$(OBJ_DIR)/%.o)
DEPS=$(SOURCES:%.c=$(DEP_DIR)/%.d)

TARGET=mm_test
DRIVER=mm_driver


#--- rules
.PHONY: doc clean mrproper

all: $(TARGET)

$(TARGET): $(TARGET_OBJ) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(DRIVER): $(OBJECTS) $(DRV_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LINKFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(DEP_DIR) $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ -c $<

$(DEP_DIR):
	@mkdir -p $(DEP_DIR)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

-include $(DEPS)

doc: $(SOURCES:%.c=$(SRC_DIR)/%.c) $(wildcard $(SOURCES:%.c=$(SRC_DIR)/%.h))
	doxygen doc/Doxyfile

clean:
	rm -rf $(OBJ_DIR) $(DEP_DIR)

mrproper: clean
	rm -rf $(TARGET) $(DRIVER) doc/html
