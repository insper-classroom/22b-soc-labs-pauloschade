TARGET = webapp/main
TARGET_DIR = webapp
IP = 169.254.0.13
DIR_NAME = /root
ALT_DEVICE_FAMILY ?= soc_cv_av
SOCEDS_ROOT ?= $(SOCEDS_DEST_ROOT)
HWLIBS_ROOT = $(SOCEDS_ROOT)/ip/altera/hps/altera_hps/hwlib
CROSS_COMPILE = arm-linux-gnueabihf-
CFLAGS = -g -Wall   -D$(ALT_DEVICE_FAMILY) -I$(HWLIBS_ROOT)/include/$(ALT_DEVICE_FAMILY)   -I$(HWLIBS_ROOT)/include/
LDFLAGS =  -g -Wall 
CC = $(CROSS_COMPILE)gcc
ARCH= arm

build: $(TARGET)

$(TARGET): main.o led/terasic_lib.o  \
	led/LCD_Lib.o led/LCD_Driver.o led/LCD_Hw.o led/lcd_graphic.o led/font.o
	$(CC) $(LDFLAGS)   $^ -o $@   -lrt -lm

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: deploy
deploy: $(TARGET)
	scp -r $(TARGET_DIR) root@$(IP):$(DIR_NAME)

.PHONY: clean
clean:
	rm -f $(TARGET) *.a *.o *~