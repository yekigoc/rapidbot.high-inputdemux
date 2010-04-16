#include "FreeRTOS.h"
#include "locator.h"
#include "locator_task.h"
#include "common.h"
#include "task.h"
#include "pio/pio.h"

#define locatorSTACK_SIZE		(200)
#define SPI_PCS(npcs)       ((~(1 << npcs) & 0xF) << 16)

void vLocatorTask( void *pvParameters );

/*-----------------------------------------------------------*/

void vStartLocatorTask( unsigned portBASE_TYPE uxPriority )
{
unsigned portBASE_TYPE uxLEDTask;

/* Spawn the task. */
 xTaskCreate( vLocatorTask,  ( signed portCHAR * ) "Loc", locatorSTACK_SIZE, ( void * ) NULL, uxPriority, ( xTaskHandle * ) NULL );
}
/*-----------------------------------------------------------*/

void loc_writecommand(unsigned char command, unsigned int snpcs, unsigned char leavenpcslow)
{
  Pin npcs = NPCS_LOC;
  Pin cdout = CDOUT_LOC;
  Pin spck = SPCK_LOC;
  PIO_Clear (&spck);
  int i = 0;
  int j = 0;
  for (i=0; i<50; i++)
    asm("nop");
  PIO_Clear (&npcs);
  for (i=0; i<50; i++)
    asm("nop");
  for (j=0; j<8; j++)
    {
      PIO_Set (&spck);
      if ((command & 1<<(7-j)))
	PIO_Set (&cdout);
      else 
	PIO_Clear (&cdout);
      for (i=0; i<50; i++)
	asm("nop");
      PIO_Clear (&spck);
      for (i=0; i<50; i++)
	asm("nop");
    }
  if (leavenpcslow==0)
    PIO_Set (&npcs);
  PIO_Set (&spck);
  for (i=0; i<10; i++)
    asm("nop");
 
}

void vLocatorTask( void *pvParameters )
{
  int i = 0;
  Pin npcs = NPCS_LOC;
  //  extern void ( vSPI_ISR_Wrapper )( void );
  //  iostat.portchange = 0;
  //  AT91C_BASE_PIOA->PIO_IER = 1<<12;
  //  AIC_ConfigureIT(AT91C_ID_PIOA, 0, vSPI_ISR_Wrapper);
  //  AIC_EnableIT(AT91C_ID_PIOA);

  unsigned char command = 0x1;

  //  trspistat.compassstat = 1;
  loc_writecommand(command, 3, 0);
  vTaskDelay( 50 / portTICK_RATE_MS );
  command = 0x0;
  loc_writecommand(command, 3, 0);
  vTaskDelay( 50 / portTICK_RATE_MS );

  for(;;)
    {
      vTaskDelay( 500 / portTICK_RATE_MS );
      command = 0x0;
      loc_writecommand(command, 3, 0);
      vTaskDelay( 500 / portTICK_RATE_MS );
      command = 0x1;
      loc_writecommand(command, 3, 0);
      vTaskDelay( 500 / portTICK_RATE_MS );
      command = 0xF;
      loc_writecommand(command, 3, 0);
      //      vTaskDelay( 50 / portTICK_RATE_MS );
    }
}
