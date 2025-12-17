#ifndef inputlib_hid_c_h_
#define inputlib_hid_c_h_

#include <sys/hidut.h>
#include <sys/hiddi.h>
#include <sys/dispatch.h>

#define MAX_SLIDER                16
#define MAX_HAT                   16

#define HID_MOUSE_HAS_WHEEL       (0x01)
#define HID_MOUSE_WHEEL_ON        (0x02)

/*******************************************************************************
*
* Keyboard Data Structures
*
*******************************************************************************/

typedef struct _hid_keyboard_data {
	_uint16 nLeds;                  /* Number of leds                       */
	_uint16 nKeys;                  /* Size of keyboard array               */
	_uint16 nRate;                  /* Time interval to repeat (in msecs)   */
	_uint16 nDelay;                 /* Delay time interval (in msecs)       */
} hid_keyboard_data_t, *pHid_keyboard_data_t;



/*******************************************************************************
*
* Mouse Data Structures
*
*******************************************************************************/

typedef struct _mouse_data {
	_uint16 nButtons;               /* Number of buttons                    */
	_uint8  flags;                  /* Flags                                */
} mouse_data_t, *pMouse_data_t;

typedef struct _mouse_raw_data {
	_uint8  btnStates;              /* Buttons states (each bit == 1 corresponds to pressed button */

	_int16 x;                       /* pointer x-movement                   */
	_int16 y;                       /* pointer y-movement                   */
	_int16 z;                       /* wheel movement (if wheel exists)     */
} mouse_raw_data_t, *pMouse_raw_data_t;


/*******************************************************************************
*
* Joystick Data Structures
*
*******************************************************************************/
#define JOYSTICK_BUTTON_MAX 64
#define JOYSTICK_AXIS_MAX 6

typedef struct _joystick_attrib {
	_uint16 nButtons;               /* Number of buttons                    */
	_uint16 nButtons2;               /* Number of buttons                    */
	_uint8  flags;                  /* Flags                                */

	_Uint32t devno;
	_Uint32t vendor_id;
	_Uint32t product_id;
	_Uint32t version;

	_uint16 naxis;

	uint8_t abs_map[JOYSTICK_AXIS_MAX];
	uint8_t has_abs[JOYSTICK_AXIS_MAX];

	struct axis_correct
	{
		int minimum;
		int maximum;
	} abs_correct[JOYSTICK_AXIS_MAX];

	_uint8 has_hat;

	struct hat_axis_correct
	{
		int minimum;
		int maximum;
	} hat_correct;

} joystick_attrib_t, *pJoystick_attrib_t;

typedef struct _joystick_data {
//	_uint16 nButtons_1;             /* Number of buttons                                           */
	_uint8  btnStates_1;            /* Buttons states (each bit == 1 corresponds to pressed button */
	_uint64 button_state;
//	_uint16 nButtons_2;             /* Number of buttons                                           */
	_uint8  btnStates_2;            /* Buttons states (each bit == 1 corresponds to pressed button */
	/* Pointer    */
	_uint32 x;
	_uint32 y;
	_uint32 z;
	/* Hat switch */
	_uint32 hat_switch;
	/* Slider   */
	_uint32 slider;
	/* Rotational */
	_uint32 Rx;
	_uint32 Ry;
	_uint32 Rz;
} joystick_raw_data_t, * pJoystick_raw_data_t;


/*******************************************************************************
*
* Touchscreen Data Structures
*
*******************************************************************************/

typedef struct _touch_attrib {
		_uint16 nButtons;               /* Number of buttons                    */
		_uint8  flags;                  /* Flags                                */
} touch_attrib_t, *pTouch_attrib_t;

typedef struct _touch_raw_data {
	_uint8 touched;                 /* 1 if screen yes, otherwise 0         */

	_int16 x;                       /* Absolute x-position                  */
	_int16 y;                       /* Absolute y-position                  */
	_int16 z;                       /* Absolute y-position                  */
} touch_raw_data_t, *pTouch_raw_data_t;


/*******************************************************************************
*
* Consumer Control Data Structures
*
*******************************************************************************/

typedef struct _control_attrib {
		_uint16 nButtons;               /* Number of buttons                    */
		_uint8  flags;                  /* Flags                                */
} control_attrib_t, *pControl_attrib_t;

typedef struct _control_raw_data {
	_uint32 button_state;

	_int16 Rx;
	_int16 Ry;
	_int16 Rz;
} control_raw_data_t, *pControl_raw_data_t;

/*
 * devi.h
 */

#define MODULE_TABLE_SIZE 	32
#define RESMGR_Q_SIZE 		100

#define MAX_SLIDER          	16
#define MAX_HAT             	16

typedef struct _input_module	input_module_t;

struct _input_module {

	input_module_t			*up;
	input_module_t			*down;
	struct Line			*line;
	int				flags;
#define MODULE_FLAG_INUSE	(1 << 0)

	int				type;

#define DEVI_CLASS_MASK					0x000000ff
#define DEVI_CLASS_NONE					0x00000001
#define DEVI_CLASS_KBD					0x00000002
#define DEVI_CLASS_REL					0x00000004
#define DEVI_CLASS_ABS					0x00000008
#define DEVI_CLASS_JOYSTICK				0x00000010
#define DEVI_CLASS_GAMEPAD				0x00000020
#define DEVI_CLASS_CONTROL				0x00000040

#define DEVI_MODULE_TYPE_MASK				0x0000ff00
#define DEVI_MODULE_TYPE_FILTER				0x00000100
#define DEVI_MODULE_TYPE_PROTO				0x00000200
#define DEVI_MODULE_TYPE_DEVICE				0x00000400

	char				name[12], date[12];
	const char			*args;
	void				*data;
	int				(*init)(input_module_t *);
	int				(*reset)(input_module_t *);
	int				(*input)(input_module_t *, int, void *);
	int				(*output)(input_module_t *, void *, int);
	int				(*pulse)(message_context_t *, int, unsigned, void *);
	int				(*parm)(input_module_t *, int, char *);
	int				(*devctrl)(input_module_t *, int, void *);
	int				(*shutdown)(input_module_t *, int);

	int				(*insertion)(input_module_t *, int, void *);
	int				(*removal)(input_module_t *, int, void *);
};

struct devctl_getkbd {
	short	  		rate;	/* 0 to 31 where 0 is 30Hz */
	short			delay;	/* 0 to 3 where 0 is 250ms, and 3 is 1s */
};

struct devctl_setkbd {
	short			rate;	/* 0 to 31 where 0 is 30Hz */
	short			delay;	/* 0 to 3 where 0 is 250ms, and 3 is 1s */
};

struct devctl_mouse_types
{
    unsigned char type;		/* max level     */
    unsigned char curtype;	/* current level */
};

#define LIST_FIRST_ITEM(head) ((head) -> lh_first)
#define LIST_NEXT_ITEM(elm, field) ((elm == NULL) ? NULL : (elm) -> field.le_next)
#define LIST_PREV_ITEM(elm, field) ((elm == NULL) ? NULL : (elm) -> field.le_prev)

#define ARRAY_SIZE(a)    (sizeof(a) / sizeof(a[0]))

/*
 * const.h
 */

#define MSEC(_t) ((unsigned long)(((_t).tv_sec * 1000) + ((_t).tv_nsec / 1000000)))

/* Mouse types */
#define NO_WHEEL_MOUSE		0x00
#define WHEEL_3B_MOUSE		0x03
#define WHEEL_5B_MOUSE		0x04

/* devctls */
#define DEVCTL_SETBAUD		0x0010	/* change the baud rate */
#define DEVCTL_RESET		0x0020	/* reset */
#define DEVCTL_LED		0x0030	/* set keyboard leds */
#define DEVCTL_GETKBD		0x0040	/* get keyboard rate and delay */
#define DEVCTL_SETKBD		0x0050	/* set keyboard rate and delay */
#define DEVCTL_RESET_KBD	0x0060	/* reset keyboard device */
#define DEVCTL_RESET_PS2MOUSE	0x0070	/* reset ps2 controller */
#define DEVCTL_ISKBD		0x0080	/* device emitting real scancodes? */
#define DEVCTL_RESCHG		0x0090	/* establish new display resolution */
#define DEVCTL_GETDEVFLAGS	0x00a0	/* get the device flags */
#define DEVCTL_GETKEYF	 	0x00b0	/* get keymap file name */
#define DEVCTL_SETKEYF 		0x00c0	/* set keymap file name */
#define DEVCTL_GETPTRBTNS 	0x00d0	/* get available buttons mask */
#define	DEVCTL_GETPTRCOORD	0x00e0	/* get number of device coordinates */
#define	DEVCTL_GETPTRPRESS	0x00f0	/* get number of device pressures */
#define DEVCTL_GETCOORDRNG	0x0100	/* get the range of a coordinate */
#define DEVCTL_GETPRESSRNG	0x0110	/* get the range of a pressure */
#define DEVCTL_GETDEVTYPE	0x0120	/* get the range of a pressure */
#define DEVCTL_COORDCHG		0x0130	/* change the device coordinates */
#define DEVCTL_BELL		0x0140	/* sound the bell */
#define DEVCTL_GETBELLPARM	0x0150	/* get bell parameters */
#define DEVCTL_SETBELLPARM	0x0160	/* set bell parameters */
#define DEVCTL_GETLED		0x0170	/* get LED status */
#define DEVCTL_SETLED		0x0180	/* set LED status */
#define DEVCTL_GETLEDMASK	0x0190	/* get the LED's present on device */
#define DEVCTL_SETTRIG		0x01c0  /* set trigger level */
#define DEVCTL_GETLOADKBDS  	0x01d0  /* get list of loaded keyboards    */
#define DEVCTL_SEL_KBD      	0x01d1  /* select keyboard                 */
#define DEVCTL_GET_SEL_KBD  	0x01d2  /* get selected keyboard           */
#define DEVCTL_ADD_KBD      	0x01d3  /* add new keyboard                */
#define DEVCTL_REMOVE_KBD   	0x01d4  /* remove kbd                      */
#define DEVCTL_RELOAD_KBDS  	0x01d5  /* reload keyboards from scratch   */
#define DEVCTL_SET_CALIB_MATRIX 0x01d6  /* Set the 3 Point Calibration Matrix */
#define DEVCTL_SET_STATE        0x01d7  /* Set the Device State */
#define DEVCTL_SET_KBD_STATE    0x01d8  /* Set the KBD device State */
#define DEVCTL_SET_REL_STATE    0x01d9  /* Set the REL device State */
#define DEVCTL_SET_ABS_STATE    0x01da  /* Set the ABS device State */

/* signalled on lower layers once reset complete */
#define DEVCTL_CHECKPOINT	0x01d0

#define DEVCTL_SETACCEL		0x01f00	/* change relative filter parms */
#define DEVCTL_GETACCEL		0x02000

#define DEVCTL_GETSWAP		0x02100
#define DEVCTL_SETSWAP		0x02200

#define DEVCTL_ABS_MODE         0x02400

#define DEVCTL_GETWHEEL		0x02600
#define DEVCTL_SETWHEEL		0x02700

#define DEVCTL_GETWHEEL_ACCEL   0x02800
#define DEVCTL_SETWHEEL_ACCEL   0x02900

#define DEVCTL_GETZMODE         0x02a00  /* I mouse has a wheel, how to deal with it ? */
#define DEVCTL_SETZMODE         0x02b00  /* See constants below                        */

#define DEVCTL_GETSAMPLE_RATE   0x02c00  /* Number of samples per second               */
#define DEVCTL_SETSAMPLE_RATE   0x02d00

#define DEVCTL_GET_RESOLUTION   0x02e00
#define DEVCTL_SET_RESOLUTION   0x02f00  /* Value sets power of two counts per mm      */


#define DEVCTL_GETMOUSETYPE     0x04000  /* get mouse type: returns in buffer struct devctl_mouse_types; types - see before  */
#define DEVCTL_SETMOUSETYPE     0x04100  /* send in buffer int value (new mouse type); types - see before */


/*
 * proto.h
 */

/* Initializes HID driver              */
void  devi_hid_init();

/* Connects to USB HID server          */
int devi_hid_server_connect(char * serv_path_name);

/* Disconnects client from USB HID server */
void devi_hid_server_disconnect();

/* This function attach client to HID USB sub-system.  Register your client from
 * reset function of each device module. If nDev >= 0, it specifies device number.
 * Function returns local module handler
*/
void *  devi_hid_register_client(input_module_t *module, int nDev);

/* Detach input module from HID driver  */
void devi_unregister_hid_client(void * h);

/* HID devctrl processor                */
int devi_hid_devctrl(void * pHandle, int event, void *ptr,
    int nDev /*HIDD_CONNECT_WILDCARD if all devices connected to this module*/);

#endif /* inputlib_hid_c_h_ */
