/*
  Simple DirectMedia Layer
  Copyright (C) 2017 BlackBerry Limited

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include "../../SDL_internal.h"
#include "../../events/SDL_mouse_c.h"
#include "../../events/SDL_keyboard_c.h"
#include "SDL_events.h"
#include "SDL_scancode.h"

#include <sys/keycodes.h>

#include "s3hid_hid_c.h"

extern int s3hid_handleJoystickEvent(input_module_t *module, int data_size, void * data);
extern int s3hid_handleJoystickInsert(input_module_t *module, int data_size, void * data);
extern int s3hid_handleJoystickRemove(input_module_t *module, int data_size, void * data);

static void *g_joystick_client_h;
static void *g_keyboard_client_h;
static void *g_mouse_client_h;

#define _MOUSE_ID       1

int s3hid_handleMouseEvent(input_module_t *module, int data_size, void *data)
{
        static uint8_t prevBtnStates; //FIXME: this will not work for 2 or more mouses
        pMouse_raw_data_t m_data;
        uint8_t changedBtnStates;
        int i;

        if (data_size < sizeof(mouse_raw_data_t))
                return -1;

        m_data = (pMouse_raw_data_t)data;

        SDL_LogDebug(SDL_LOG_CATEGORY_INPUT, "m_data: X:%04d Y:%04d Z:%04d btn:%02x\n",
                     m_data->x, m_data->y, m_data->z, m_data->btnStates);

        /* Send motion event if the motion was really occur */
        if ((m_data->x != 0) || (m_data->y != 0))
                SDL_SendMouseMotion(NULL, _MOUSE_ID, 1, m_data->x, m_data->y);

        /* Send mouse button press/release events */
        changedBtnStates = prevBtnStates ^ m_data->btnStates;
        if (changedBtnStates) {
                /* Cycle all buttons status */
                for (i = 0; i < 8; i++) {
                        Uint8 state;

                        if (!((1 << i) & changedBtnStates))
                                continue;

                        state = ((1 << i) & m_data->btnStates) ? SDL_PRESSED : SDL_RELEASED;
                        SDL_SendMouseButton(NULL, _MOUSE_ID, state, i);
                }

                prevBtnStates = m_data->btnStates;
        }

        /* Send mouse wheel events */
        /* Send vertical wheel event only */
        if (m_data->z != 0)
                SDL_SendMouseWheel(NULL, _MOUSE_ID, 0, m_data->z, SDL_MOUSEWHEEL_NORMAL);

        return 0;
}

static int s3hid_handleKeyboardEvent(input_module_t *module,
				     int data_size, void *data)
{
	uint16_t *k_data = (uint16_t *)data;
	int i, num_keys = data_size / sizeof(k_data[0]);
	uint8_t state;

	SDL_LogDebug(SDL_LOG_CATEGORY_INPUT, "%s with data_size: %d\n",
		     __func__, data_size);

	if (num_keys < 1)
		return 0;

	state = k_data[0];

	for (i = 1; i < num_keys; i++) {
		SDL_Scancode scancode = k_data[i];

		SDL_LogDebug(SDL_LOG_CATEGORY_INPUT, "key: %d (0x%04x) %s - %s\n",
			     scancode, scancode, SDL_GetScancodeName(scancode),
			     state ? "pressed" : "released");

		SDL_SendKeyboardKey(state, scancode);
	}

	return 0;
}

void s3hid_init(void)
{
	static input_module_t joystick_input;
	static input_module_t keyboard_input;
	static input_module_t mouse_input;

	joystick_input.type = DEVI_CLASS_JOYSTICK;
	joystick_input.input = s3hid_handleJoystickEvent;
	joystick_input.insertion = s3hid_handleJoystickInsert;
	joystick_input.removal = s3hid_handleJoystickRemove;

	keyboard_input.type = DEVI_CLASS_KBD;
	keyboard_input.input = s3hid_handleKeyboardEvent;

	mouse_input.type = DEVI_CLASS_REL;
	mouse_input.input = s3hid_handleMouseEvent;

	devi_hid_init();
	devi_hid_server_connect("/dev/io-hid/my-hid");

	g_joystick_client_h = devi_hid_register_client(&joystick_input,
						       HIDD_CONNECT_WILDCARD);
	if (g_joystick_client_h == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_INPUT, "s3hid joystick register error\n");
	}

	g_mouse_client_h = devi_hid_register_client(&mouse_input,
						    HIDD_CONNECT_WILDCARD);
	if (g_mouse_client_h == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_INPUT, "s3hid mouse register error\n");
	}

	g_keyboard_client_h = devi_hid_register_client(&keyboard_input,
						       HIDD_CONNECT_WILDCARD);
	if (g_keyboard_client_h == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_INPUT, "s3hid keyboad register error\n");
	}

	SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "QNX S3 HID driver initialized\n");
}

void s3hid_deinit(void)
{
	devi_unregister_hid_client(g_joystick_client_h);
	devi_unregister_hid_client(g_keyboard_client_h);
	devi_unregister_hid_client(g_mouse_client_h);
	devi_hid_server_disconnect();

	SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "QNX S3 HID driver de-initialized\n");
}
