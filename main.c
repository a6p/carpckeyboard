/* Name: main.c
 * Project: Nassato Rotary keyboard
 * Author: Vasya
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "usbdrv.h"
#include "rotaryEncoder.h"

#define ADC1 4
#define ADC2 5

typedef struct
{
	uint8_t  reportId;                                // Report ID = 0x01 (1)
	uint8_t ConsumerControl;                          
	uint8_t nil;
} inputConsumer_t;
	
typedef struct
{
	uint8_t  reportId;                                 // Report ID = 0x02 (2)
	uint8_t  KB_KeyboardKeyboardLeftControl  : 1;      // Usage 0x000700E0: Keyboard Left Control, Value = 0 to 1
	uint8_t  KB_KeyboardKeyboardLeftShift	 : 1;      // Usage 0x000700E1: Keyboard Left Shift, Value = 0 to 1
	uint8_t  KB_KeyboardKeyboardLeftAlt		 : 1;      // Usage 0x000700E2: Keyboard Left Alt, Value = 0 to 1
	uint8_t  KB_KeyboardKeyboardLeftGui		 : 1;      // Usage 0x000700E3: Keyboard Left GUI, Value = 0 to 1
	uint8_t  KB_KeyboardKeyboardRightControl : 1;      // Usage 0x000700E4: Keyboard Right Control, Value = 0 to 1
	uint8_t  KB_KeyboardKeyboardRightShift	 : 1;      // Usage 0x000700E5: Keyboard Right Shift, Value = 0 to 1
	uint8_t  KB_KeyboardKeyboardRightAlt	 : 1;      // Usage 0x000700E6: Keyboard Right Alt, Value = 0 to 1
	uint8_t  KB_KeyboardKeyboardRightGui	 : 1;      // Usage 0x000700E7: Keyboard Right GUI, Value = 0 to 1
	uint8_t  Keyboard;                                 // Value = 0 to 101
} inputKeyboard_t;

static inputConsumer_t consumer_Report;
static inputKeyboard_t keyboard_report; 
static uint8_t  idleRate; 

PROGMEM const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = { /* USB report descriptor */
	0x05, 0x0c,                    // USAGE_PAGE (Consumer Devices)
	0x09, 0x01,                    // USAGE (Consumer Control)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x85, 0x01,                    //   REPORT_ID (1)
	0x19, 0x00,                    //   USAGE_MINIMUM (Unassigned)
	0x2a, 0x3c, 0x02,              //   USAGE_MAXIMUM (AC Format)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x26, 0x3c, 0x02,              //   LOGICAL_MAXIMUM (572)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0x75, 0x10,                    //   REPORT_SIZE (16)
	0x81, 0x00,                    //   INPUT (Data,Var,Abs)
	0xc0,                          // END_COLLECTION
	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	0x09, 0x06,                    // USAGE (Keyboard)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x85, 0x02,                    //   REPORT_ID (2)
	0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
	0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
	0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
	0x75, 0x01,                    //   REPORT_SIZE (1)
	0x95, 0x08,                    //   REPORT_COUNT (8)
	0x81, 0x02,                    //   INPUT (Data,Var,Abs)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0x75, 0x08,                    //   REPORT_SIZE (8)
	0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
	0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
	0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
	0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
	0xc0                           // END_COLLECTION

};

static uint16_t getAdc(uint8_t adc) {
    //return 0;
    ADMUX = adc;
    ADMUX |= (1<<REFS0);
    ADCSRA  |= (1<<ADSC);
    while (ADCSRA &  (1<<ADSC));  
    return ADCW;	
}

static void hardwareInit(void)
{
    uint8_t	i, j;
    PORTB = 0xff;
    DDRB = 0;       
    PORTC = 0xcf;   
    DDRC = 0;         
    rotaryEncoder_init();
    
	j = 0;
	while(--j){     /* USB Reset by device only required on Watchdog Reset */
		i = 0;
		while(--i); /* delay >10ms for USB reset */
	}
    DDRD = 0x02;    /* 0000 0010 bin: remove USB reset condition */
    TCCR0 = 5;      /* timer 0 prescaler: 1024 */
    
    ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); //adc division factor 128
    
}

static uint8_t    keyPressed(void)
{
    uint8_t   i, mask, x;  
    uint16_t adc1_value = 0;
    uint16_t adc2_value = 0;
        
    x = PINB;
    mask = 1;
    for(i=0;i<6;i++){
        if((x & mask) == 0)
            return i + 1;
        mask <<= 1;
    }
    x = PINC;
    mask = 1;
    for(i=0;i<4;i++){
        if((x & mask) == 0)
            return i + 7;
        mask <<= 1;
    }
    adc1_value = getAdc(ADC1); 
    adc2_value = getAdc(ADC2); 
    
    if ( adc1_value < 10){
        return 15;
    } 
    else if ( adc1_value >200 && adc1_value < 320){
        return 16;
    }else if ( adc1_value >460 && adc1_value < 600){
        return 17;
    }else if ( adc1_value >700 && adc1_value < 890){
        return 18;
    }
               
    if ( adc2_value < 10){
        return 19;
    } 
    else if ( adc2_value >200 && adc2_value < 320){
        return 20;
    }else if ( adc2_value >460 && adc2_value < 600){
        return 21;
    }else if ( adc2_value >700 && adc2_value < 890){
        return 22;
    }
    return 0;
}

void buildKeyboardReport(uint8_t send_key) {
	
	keyboard_report.reportId = 2;
	keyboard_report.KB_KeyboardKeyboardLeftControl	   = 0 ;
	keyboard_report.KB_KeyboardKeyboardLeftShift	   = 0 ;
	keyboard_report.KB_KeyboardKeyboardLeftAlt		   = 0 ;
	keyboard_report.KB_KeyboardKeyboardLeftGui		   = 0 ;
	keyboard_report.KB_KeyboardKeyboardRightControl	   = 0 ;
	keyboard_report.KB_KeyboardKeyboardRightShift	   = 0 ;
	keyboard_report.KB_KeyboardKeyboardRightAlt		   = 0 ;
	keyboard_report.KB_KeyboardKeyboardRightGui		   = 0 ;
	keyboard_report.Keyboard = send_key;
	
}

void buildConsumerReport(uint8_t key)
{
	consumer_Report.reportId = 1;
	consumer_Report.ConsumerControl = key;
    consumer_Report.nil = 0;
}

uchar	usbFunctionSetup(uchar data[8]){
    usbRequest_t    *rq = (void *)data;
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* class request type */
        if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
            /* we only have one report type, so don't look at wValue */
            if (rq->wValue.bytes[0] == 1)
			{
				buildConsumerReport(KEY_NONE);
				usbMsgPtr = (usbMsgPtr_t)&consumer_Report;
				return sizeof(consumer_Report);
			}
			
			if(rq->wValue.bytes[0] == 2)
			{
				buildKeyboardReport(KEY_NONE);					   
				usbMsgPtr = (usbMsgPtr_t)&keyboard_report;
				return sizeof(keyboard_report);
			}
        }else if(rq->bRequest == USBRQ_HID_GET_IDLE){
            usbMsgPtr = (usbMsgPtr_t)&idleRate;
            return 1;
        }else if(rq->bRequest == USBRQ_HID_SET_IDLE){
            idleRate = rq->wValue.bytes[1];
        }
    }else{
        /* no vendor specific requests implemented */
    }
	return 0;
}

int	main(void){
    uint8_t key, lastKey = 0, keyDidChange = 0, firstPress = 0;
    uint8_t idleCounter = 0;
    uint8_t mustCloseConsumer = 0;
    uint8_t mustCloseKeyboard = 0; 
    uint16_t iter = 0;
    ENCODER_SPIN_DIRECTION encoderDirection;     
    
	wdt_enable(WDTO_2S);
    hardwareInit();
	usbInit();
	sei();
    
	for(;;){	/* main event loop */
		wdt_reset();
		usbPoll();        
        keyDidChange = 0;
        key = keyPressed();
        if(lastKey != key ){
            lastKey = key;
            keyDidChange = 1;
            iter = 0;
            firstPress = 0;
        } else {
            if ( key == 17 || key == 18 ){
                if ( !firstPress ){
                    if ( iter > 3000 ){
                        keyDidChange = 1;
                        iter = 0;
                        firstPress = 1;
                    } else iter++;
                }else{
                    if ( iter > 700 ){
                        keyDidChange = 1;
                        iter = 0;
                    } else iter++;
                }
            }
        }    
        if(TIFR & (1<<TOV0)){  
            TIFR = 1<<TOV0;
            if(idleRate != 0 && key < 15){
                if(idleCounter > 4){
                    idleCounter -= 5;  
                }else{
                    idleCounter = idleRate;
                    keyDidChange = 1;
                }
            }
        }
        
        rotaryEncoder_process();
        
        if( usbInterruptIsReady()){              
            
            encoderDirection = rotaryEncoder_get_direction();			
            
			switch(encoderDirection)
			{
				case ENCODER_SPIN_DIRECTION_LEFT: buildConsumerReport(MMKEY_VOL_UP);
					mustCloseConsumer = 1;
					usbSetInterrupt((void *)&consumer_Report, sizeof(consumer_Report));
                    break;
				case ENCODER_SPIN_DIRECTION_RIGHT: buildConsumerReport(MMKEY_VOL_DOWN);
					mustCloseConsumer = 1;
					usbSetInterrupt((void *)&consumer_Report, sizeof(consumer_Report));	
                    break;
                case ENCODER2_SPIN_DIRECTION_LEFT: buildKeyboardReport(KEY_F1);
                    mustCloseKeyboard =1;
					usbSetInterrupt((void *)&keyboard_report, sizeof(keyboard_report));
                    break;
				case ENCODER2_SPIN_DIRECTION_RIGHT: buildKeyboardReport(KEY_F2);
                    mustCloseKeyboard = 1;
					usbSetInterrupt((void *)&keyboard_report, sizeof(keyboard_report));	
				    break;
				case ENCODER_SPIN_DIRECTION_NONE:
					if (mustCloseConsumer)
					{
						buildConsumerReport(KEY_NONE);
						mustCloseConsumer = 0;
						usbSetInterrupt((void *)&consumer_Report, sizeof(consumer_Report));
						continue;
					} 
					else if (mustCloseKeyboard){
                        buildKeyboardReport(KEY_NONE);
                        mustCloseKeyboard=0;
                        usbSetInterrupt((void *)&keyboard_report, sizeof(keyboard_report));
                        continue;
                    }
				default:
				break;
			}				
			
			if(encoderDirection == ENCODER_SPIN_DIRECTION_NONE && !mustCloseConsumer && keyDidChange)
			{
                keyDidChange = 0;
				switch(lastKey){
                    case 0:
                            buildKeyboardReport(KEY_NONE);	
                            usbSetInterrupt((void *)&keyboard_report, sizeof(keyboard_report));
                            break;
                    /*default:
				            buildKeyboardReport(lastKey);	
                            usbSetInterrupt((void *)&keyboard_report, sizeof(keyboard_report));
                            break;*/  
                    case 3: buildConsumerReport(MMKEY_MUTE);
                            usbSetInterrupt((void *)&consumer_Report, sizeof(consumer_Report));
                            mustCloseConsumer=1;
                            break;
                    case 4: buildConsumerReport(MMKEY_PLAYPAUSE);
                            usbSetInterrupt((void *)&consumer_Report, sizeof(consumer_Report)); 
                            mustCloseConsumer=1;
                            break;                   
                    case 9: buildKeyboardReport(KEY_F3); 
                            usbSetInterrupt((void *)&keyboard_report, sizeof(keyboard_report));
                            break;
                    case 10: buildKeyboardReport(KEY_F4); 
                            usbSetInterrupt((void *)&keyboard_report, sizeof(keyboard_report));
                            break;
                    case 11: buildKeyboardReport(KEY_F5); 
                            usbSetInterrupt((void *)&keyboard_report, sizeof(keyboard_report));
                            break;
                    case 12: buildKeyboardReport(KEY_F6); 
                            usbSetInterrupt((void *)&keyboard_report, sizeof(keyboard_report));
                            break;
                    case 13: buildKeyboardReport(KEY_F7); 
                            usbSetInterrupt((void *)&keyboard_report, sizeof(keyboard_report));
                            break;    
                    case 15: buildConsumerReport(MMKEY_SCAN_NEXT_TRACK);
                            usbSetInterrupt((void *)&consumer_Report, sizeof(consumer_Report));
                            mustCloseConsumer=1;
                            break;
                    case 16: buildConsumerReport(MMKEY_SCAN_PREV_TRACK);
                            usbSetInterrupt((void *)&consumer_Report, sizeof(consumer_Report));
                            mustCloseConsumer=1;
                            break;   
                    case 17: buildConsumerReport(MMKEY_VOL_UP);
                            mustCloseConsumer = 1;
                            usbSetInterrupt((void *)&consumer_Report, sizeof(consumer_Report));
                            break;
                    case 18: buildConsumerReport(MMKEY_VOL_DOWN);
                            mustCloseConsumer = 1;
                            usbSetInterrupt((void *)&consumer_Report, sizeof(consumer_Report));	
                            break;
                    case 19: buildConsumerReport(MMKEY_PLAYPAUSE);
                            usbSetInterrupt((void *)&consumer_Report, sizeof(consumer_Report));
                            mustCloseConsumer=1;
                            break;
                    case 20: buildKeyboardReport(KEY_F4);
                            usbSetInterrupt((void *)&keyboard_report, sizeof(consumer_Report));
                            mustCloseKeyboard = 1;
                            break;   
                    case 21: buildKeyboardReport(KEY_F5);
                            usbSetInterrupt((void *)&keyboard_report, sizeof(consumer_Report));
                            mustCloseKeyboard = 1;
                            break;
                    case 22: buildKeyboardReport(KEY_F6);
                            usbSetInterrupt((void *)&keyboard_report, sizeof(consumer_Report));	
                            mustCloseKeyboard = 1;
                            break;
                }
			}
			
		}

	}
	return 0;
}
