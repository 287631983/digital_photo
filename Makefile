#CROSS_COMPILE = arm-linux-
AS	= $(CROSS_COMPILE)as
LD	= $(CROSS_COMPILE)ld
CC 	= $(CROSS_COMPILE)gcc 
CPP	= $(CC) -E
AR 	= $(CROSS_COMPILE)ar 
NM	= $(CROSS_COMPILE)nm 

STRIP	= $(CROSS_COMPILE)strip 
OBJCOPY	= $(CROSS_COMPILE)objcopy 
OBJDUMP	= $(CROSS_COMPILE)objdump 

#export导出的变量供子目录下的Makefile使用
export AS LD CC CPP AR NM
export STRIP OBJCOPY OBJDUMP

#编译参数设置
#-Wall:显示所有警告 -O2:编译器优化等级 -g:支持gdb调试 -I:指定头文件路径
CFLAGS := -Wall -O2 -g 
CFLAGS += -I $(PWD)/include

#链接参数设置
LDFLAGS := -lm -lfreetype -lpthread -lts

export CFLAGS LDFLAGS

TOPDIR := $(PWD)
export TOPDIR

#定义最终生成的执行文件的名称
TARGET := show_file

#添加顶层目录下的子文件夹
obj-y += main.o 
obj-y += display/
obj-y += draw/
obj-y += encoding/
obj-y += fonts/
obj-y += input/

all:
	make -C ./ -f $(TOPDIR)/Makefile.build
	$(CC) -o  $(TARGET) built-in.o $(LDFLAGS)
	cp $(TARGET) ~/nfs/rootfs/ -raf

clean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)

install:
	cp $(TARGET) ~/nfs/rootfs/ -raf
