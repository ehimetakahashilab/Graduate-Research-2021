/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include <sys/alt_stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "altera_avalon_pio_regs.h"
#include "system.h"

#define read_io(x)	IORD_ALTERA_AVALON_PIO_DATA(x)
#define write_io(x, y)	IOWR_ALTERA_AVALON_PIO_DATA(x, y)

int main()
{
	int i;
	int n;
	int st;
	unsigned int A1[8];		/* An */
	unsigned int A2[8];		/* An+1 */
	unsigned int M[8];		/* Mn */
	unsigned int xor[8];
	unsigned int a[8];	/* α */
	unsigned int b[8];	/* β */
	unsigned int check[8];	/* サーバ側で生成するβ */
	unsigned int carry = 0x00000000;
	int flag;

	/* リセット信号送信 */
	write_io(RST_BASE, 1);
	write_io(CALL_BASE, 0);
	write_io(SUC_BASE, 0);
	st = 0;

	/* An, Mnを設定(今は256bit全部0) */
	for( i = 0; i < 8; i++ )
	{
		A1[i] = 0;
		M[i] = 0;
	}
//	A1
//	A1[0] = 16 (7718cb4d) 10 (1998113613)
//	A1[1] = 16 (570b9631) 10 (1460377137)
//	A1[2] = 16 (4e78f2e2) 10 (1316549346)
//	A1[3] = 16 (7e8b88da) 10 (2123073754)
//	A1[4] = 16 (2e20f421) 10 (773911585)
//	A1[5] = 16 (702be447) 10 (1881924679)
//	A1[6] = 16 (519eec17) 10 (1369369623)
//	A1[7] = 16 (158d2756) 10 (361572182)
//	M
//	M[0] = 16 (1d899967) 10 (495556967)
//	M[1] = 16 (5163dcee) 10 (1365499118)
//	M[2] = 16 (1f28573c) 10 (522737468)
//	M[3] = 16 (6a30b2b2) 10 (1781576370)
//	M[4] = 16 (48a4e6e8) 10 (1218766568)
//	M[5] = 16 (321febf) 10 (52559551)
//	M[6] = 16 (7c17c9d9) 10 (2081933785)
//	M[7] = 16 (5a8ade4c) 10 (1519050316)

	A1[0] = 1998113613;
	A1[1] = 1460377137;
	A1[2] = 1316549346;
	A1[3] = 2123073754;
	A1[4] = 773911585;
	A1[5] = 1881924679;
	A1[6] = 1369369623;
	A1[7] = 361572182;

	M[0] = 495556967;
	M[1] = 1365499118;
	M[2] = 522737468;
	M[3] = 1781576370;
	M[4] = 1218766568;
	M[5] = 52559551;
	M[6] = 2081933785;
	M[7] = 1519050316;

	/* An+1生成 */
	srand( (unsigned int) time(NULL) );
	for( i = 0; i < 8; i++ )
	{
		A2[i] = rand();
//		A2[i] = 63;
	}

	/* α生成 */
	for( i = 0; i < 8; i++ )
	{
		xor[i] = A2[i] ^ A1[i];
	}
	for( i = 0; i < 8; i++ )
	{
		a[i] = xor[i] ^ M[i];
	}

	alt_printf("alpha\n");
	for( i = 0; i < 8; i++ )
	{
		alt_printf("a[%x]: %x\n",i, a[i]);
	}
	alt_printf("\n");

	/* 認証準備 */
	for( i = 0; i < 8; i++ )
	{
		check[i] = A2[i] + A1[i] + carry;
		if( A2[i] + A1[i] + carry > 0xffffffff )
		{
			carry = 0x00000001;
		}
		else
		{
			carry = 0x00000000;
		}
	}

	write_io(RST_BASE, 0);
//	st = read_io(ST_BASE);
//	alt_printf("%x\n", st);
//	m = 0;
	n = 0;
	write_io(DI0_BASE, a[0]);
	write_io(DI1_BASE, a[1]);
	write_io(DI2_BASE, a[2]);
	write_io(DI3_BASE, a[3]);
	write_io(DI4_BASE, a[4]);
	write_io(DI5_BASE, a[5]);
	write_io(DI6_BASE, a[6]);
	write_io(DI7_BASE, a[7]);

	write_io(CALL_BASE, 1);
//	while (st==0x00) st = read_io(ST_BASE);
//	write_io(CALL_BASE, 0);

		while( 1 )
		{
			st = read_io(ST_BASE);
//			alt_printf("st %x\n", st);


			if( st == 0x05)
			{
				b[0] = read_io(DO0_BASE);
				b[1] = read_io(DO1_BASE);
				b[2] = read_io(DO2_BASE);
				b[3] = read_io(DO3_BASE);
				b[4] = read_io(DO4_BASE);
				b[5] = read_io(DO5_BASE);
				b[6] = read_io(DO6_BASE);
				b[7] = read_io(DO7_BASE);
				n = 1;
			}
			if( n == 1 )
			{
				break;
			}
		}

	alt_printf("check\n");
	for( i = 0; i < 8; i++ )
	{
		alt_printf("check[%x]: %x\n",i, check[i]);
	}
	alt_printf("\n");

	alt_printf("beta\n");
	for( i = 0; i < 8; i++ )
	{
		alt_printf("b[%x]: %x\n",i, b[i]);
	}
	alt_printf("\n");

	/* 認証 */
	for( i = 0; i < 8; i++ )
	{
		if( check[i] != b[i] )
		{
			flag = 1;
			break;
		}
		else
		{
			flag = 0;
		}
	}

	if( flag == 0 )
	{
		alt_printf("certification succeeded\n");
		write_io(SUC_BASE, 1);
		for(i =0; i < 8; i++ )
		{
			M[i] = M[i] + A1[i];
			A1[i] = A2[i];
		}
	}
	else
	{
		alt_printf("certification failed\n");
		write_io(SUC_BASE, 0);
	}
	return 0;
}


