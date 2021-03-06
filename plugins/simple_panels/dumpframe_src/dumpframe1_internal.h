#ifndef _PROCESSIMAGESEND2PANELS_INTERNAL_H_
#define _PROCESSIMAGESEND2PANELS_INTERNAL_H_
#include "dumpframe1.h"

    /* Macros: */
        /* Frame info */
        #define FRAME_PANEL_ROW_NUM             4
        #define FRAME_PANEL_COL_NUM             11

        /* Panel info */
        #define PANEL_PIXEL_NUM                 8
        #define PANEL_MESSAGE_BUFFER_SIZE       24
        #define PANEL_GRAY_SCALE_BITS           3   /*1,2,3*/

        /* USB device vendor and product id */
        #define USB_DEV_VID                     0x1781
        #define USB_DEV_PID                     0x0BB2

        /* USB device endpoint number and size */
        #define USB_EP_IN_NUM                   0x82
        #define USB_EP_OUT_NUM                  0x01

        #define USB_EP_IN_SIZE                  2
        #define USB_EP_OUT_SIZE                 64

	/* Type defines: */
            typedef struct
		    {
			    struct
			    {
				    unsigned char  PanelAddress;
				    unsigned char  BytesInPanelMessage;
			    } Header;

                struct
                {
                    unsigned char Row;
                    unsigned char Col;
                } FrameImageIndex;

                unsigned char MessageIndex;
			    unsigned char Message[PANEL_MESSAGE_BUFFER_SIZE];
		    } PanelWrapper_t;

        typedef struct
		{
            unsigned char  PanelsInThisArray;
			PanelWrapper_t Panel[FRAME_PANEL_ROW_NUM*FRAME_PANEL_COL_NUM];
		} PanelArrayWrapper_t;

            typedef struct
		    {
			    struct
			    {
                    unsigned char  ControlByte;
                    unsigned char  USBPacketIDNumber;
                    unsigned char  PanelsInThisUSBPacket;
			    } Header;
                /*Use sizeof(USBPacket.Header) to determine how many non-header data bytes can be placed in USBPacket.Data*/
                /*Actual header values should be stored in USBPacket.Data to ensure continuous locations in memory*/

                unsigned char BytesInPacketData;
                unsigned char Data[USB_EP_OUT_SIZE];
		    } USBPacketWrapper_t;

        typedef struct
		{
            unsigned char  PacketsInThisArray;
			USBPacketWrapper_t Packet[FRAME_PANEL_ROW_NUM*FRAME_PANEL_COL_NUM]; /*Allocate buffer size assuming worst case, only one panel per packet*/
		} USBPacketArrayWrapper_t;

        typedef struct
		{
            unsigned short  RowNum;
            unsigned short  ColNum;
			unsigned char ImageMatrix[FRAME_PANEL_ROW_NUM*PANEL_PIXEL_NUM][FRAME_PANEL_COL_NUM*PANEL_PIXEL_NUM];
		} FrameImageWrapper_t;

	/* Global Variables: */
		extern USBPacketArrayWrapper_t  USBPacketArray;
        extern PanelArrayWrapper_t      PanelArray;
		extern FrameImageWrapper_t      FrameImage;

        static const unsigned char PanelAddressMatrix[FRAME_PANEL_ROW_NUM][FRAME_PANEL_COL_NUM] = { { 4, 8,12,16,20,24,28,32,36,40,44},
                                                                                                    { 3, 7,11,15,19,23,27,31,35,39,43},
                                                                                                    { 2, 6,10,14,18,22,26,30,34,38,42},
                                                                                                    { 1, 5, 9,13,17,21,25,29,33,37,41}};

static void InputImage2FrameImage( void * data, intptr_t stride0,
                                   intptr_t shape0, intptr_t shape1,
                                   intptr_t offset0, intptr_t offset1 );
static void MapFrameImage2PanelArray(void);
static void ConvertFrameImage2PanelMessages(void);
static void PanelArray2USBPacketArray(void);
static usb_dev_handle *OpenUSBDev(void);
static dumpframe_error_t USBBulkWriteUSBPacketArray(dumpframe_device_t* device);


#endif /* _PROCESSIMAGESEND2PANELS_INTERNAL_H_ */
