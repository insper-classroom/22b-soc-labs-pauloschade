#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"

#include "led/terasic_os_includes.h"
#include "led/LCD_Lib.h"
#include "led/lcd_graphic.h"
#include "led/font.h"

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

#define USER_IO_DIR     (0x01000000)
#define BIT_LED         (0x01000000)
#define BUTTON_MASK     (0x02000000)

#define SOCKET_NAME "/root/webapp/soc/soc.socket"

void *virtual_base;

int status() {
    uint32_t scan_input;
    scan_input = alt_read_word( ( virtual_base + ( ( uint32_t )(  ALT_GPIO1_EXT_PORTA_ADDR ) & ( uint32_t )( HW_REGS_MASK ) ) ) );
    printf("scan_input %d \n", ~scan_input& BUTTON_MASK);
    return ~scan_input & BUTTON_MASK;
}

void led_on() {
    alt_setbits_word( ( virtual_base + ( ( uint32_t )( ALT_GPIO1_SWPORTA_DR_ADDR ) & ( uint32_t )( HW_REGS_MASK ) ) ), BIT_LED );
}

void led_off() {
    alt_clrbits_word( ( virtual_base + ( ( uint32_t )( ALT_GPIO1_SWPORTA_DR_ADDR ) & ( uint32_t )( HW_REGS_MASK ) ) ), BIT_LED );
}

int main(int argc, char *argv[]) {
    printf("connecting \n");
    LCD_CANVAS LcdCanvas;
    int sock = 0, connfd = 0;
    int fd, st;
    char sendBuff[1025];
	char recvBuff[1025];


    if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}

	virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, HW_REGS_BASE );

    printf("Graphic LCD Demo\r\n");
		
		LcdCanvas.Width = LCD_WIDTH;
		LcdCanvas.Height = LCD_HEIGHT;
		LcdCanvas.BitPerPixel = 1;
		LcdCanvas.FrameSize = LcdCanvas.Width * LcdCanvas.Height / 8;
		LcdCanvas.pFrame = (void *)malloc(LcdCanvas.FrameSize);
		
	if (LcdCanvas.pFrame == NULL){
			printf("failed to allocate lcd frame buffer\r\n");
	}else{			
		
	
		LCDHW_Init(virtual_base);
		LCDHW_BackLight(true); // turn on LCD backlight
		
        LCD_Init();
    
        // clear screen
        DRAW_Clear(&LcdCanvas, LCD_WHITE);

        DRAW_PrintString(&LcdCanvas, 40, 5, "Hello", LCD_BLACK, &font_16x16);
        DRAW_PrintString(&LcdCanvas, 40, 5+16, "SoCFPGA", LCD_BLACK, &font_16x16);
        DRAW_PrintString(&LcdCanvas, 40, 5+32, "Terasic ", LCD_BLACK, &font_16x16);
        DRAW_Refresh(&LcdCanvas);
        free(LcdCanvas.pFrame);
    }

	memset(sendBuff, '0', sizeof(sendBuff));

    /*
     * In case the program exited inadvertently on the last run,
     * remove the socket.
     */
    unlink(SOCKET_NAME);

	sock = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un name;
    memset(&name, 0, sizeof(struct sockaddr_un));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);
   
    bind(sock, (const struct sockaddr *) &name, sizeof(struct sockaddr_un));

    /* Prepare for accepting connections. */
    listen(sock, 20);
    connfd = accept(sock, (struct sockaddr*)NULL, NULL);

    printf("ready to receive \n");

    while(1) {

        // read and print
        int n = 0;
        n = read(connfd, recvBuff, sizeof(recvBuff)-1);
        if (n > 0) {
            recvBuff[n-1] = '\0';

            if(!strcmp(recvBuff, "on")) {
                printf("turning led on\n");
                led_on();
            }else if(!strcmp(recvBuff, "off")) {
                printf("turning led off\n");
                led_off();
            }else if (!strcmp(recvBuff, "status")) {
                char str[10];
                st = status();
                sprintf(str, "%d", st);
                write(connfd, str, strlen(str));
            } else if (!strcmp(recvBuff, "write")) {
                char str[10];
                st = status();
                sprintf(str, "%d", st);
                write(connfd, str, strlen(str));
            }
        }

        sleep(1);
    }

    close(connfd);
    sleep(1);
}