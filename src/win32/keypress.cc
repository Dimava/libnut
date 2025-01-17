#include "../keypress.h"
#include "../deadbeef_rand.h"
#include "../microsleep.h"

#include <cctype> /* For isupper() */

void win32KeyEvent(int key, MMKeyFlags flags)
{
	UINT scan = MapVirtualKey(key & 0xff, MAPVK_VK_TO_VSC);

	/* Set the scan code for extended keys */
	switch (key)
	{
	case VK_RCONTROL:
	case VK_SNAPSHOT: /* Print Screen */
	case VK_RMENU:	  /* Right Alt / Alt Gr */
	case VK_PAUSE:	  /* Pause / Break */
	case VK_HOME:
	case VK_UP:
	case VK_PRIOR: /* Page up */
	case VK_LEFT:
	case VK_RIGHT:
	case VK_END:
	case VK_DOWN:
	case VK_NEXT: /* 'Page Down' */
	case VK_INSERT:
	case VK_DELETE:
	case VK_LWIN:
	case VK_RWIN:
	case VK_APPS: /* Application */
	case VK_VOLUME_MUTE:
	case VK_VOLUME_DOWN:
	case VK_VOLUME_UP:
	case VK_MEDIA_NEXT_TRACK:
	case VK_MEDIA_PREV_TRACK:
	case VK_MEDIA_STOP:
	case VK_MEDIA_PLAY_PAUSE:
	case VK_BROWSER_BACK:
	case VK_BROWSER_FORWARD:
	case VK_BROWSER_REFRESH:
	case VK_BROWSER_STOP:
	case VK_BROWSER_SEARCH:
	case VK_BROWSER_FAVORITES:
	case VK_BROWSER_HOME:
	case VK_LAUNCH_MAIL:
	{
		flags |= KEYEVENTF_EXTENDEDKEY;
		break;
	}
	}

	INPUT keyboardInput;
	keyboardInput.type = INPUT_KEYBOARD;
	keyboardInput.ki.wScan = (WORD)scan;
	keyboardInput.ki.wVk = (WORD)key;
	keyboardInput.ki.dwFlags = KEYEVENTF_SCANCODE | flags;
	keyboardInput.ki.time = 0;
	SendInput(1, &keyboardInput, sizeof(keyboardInput));
}

void toggleKeyCode(MMKeyCode code, const bool down, MMKeyFlags flags)
{
	const DWORD dwFlags = down ? 0 : KEYEVENTF_KEYUP;

	if (down)
	{
		/* Parse modifier keys. */
		if (flags & MOD_META)
			win32KeyEvent(K_META, dwFlags);
		if (flags & MOD_ALT)
			win32KeyEvent(K_ALT, dwFlags);
		if (flags & MOD_CONTROL)
			win32KeyEvent(K_CONTROL, dwFlags);
		if (flags & MOD_SHIFT)
			win32KeyEvent(K_SHIFT, dwFlags);

		win32KeyEvent(code, dwFlags);
	}
	else
	{
		win32KeyEvent(code, dwFlags);

		/* Parse modifier keys. */
		if (flags & MOD_META)
			win32KeyEvent(K_META, dwFlags);
		if (flags & MOD_ALT)
			win32KeyEvent(K_ALT, dwFlags);
		if (flags & MOD_CONTROL)
			win32KeyEvent(K_CONTROL, dwFlags);
		if (flags & MOD_SHIFT)
			win32KeyEvent(K_SHIFT, dwFlags);

	}
}

void tapKeyCode(MMKeyCode code, MMKeyFlags flags)
{
	toggleKeyCode(code, true, flags);
	toggleKeyCode(code, false, flags);
}

void toggleKey(char c, const bool down, MMKeyFlags flags)
{
	MMKeyCode keyCode = keyCodeForChar(c);

	int modifiers = keyCode >> 8; // Pull out modifers.
	if ((modifiers & 1) != 0)
		flags |= MOD_SHIFT; // Update flags from keycode modifiers.
	if ((modifiers & 2) != 0)
		flags |= MOD_CONTROL;
	if ((modifiers & 4) != 0)
		flags |= MOD_ALT;
	keyCode = keyCode & 0xff; // Mask out modifiers.
	toggleKeyCode(keyCode, down, flags);
}

void tapKey(char c, MMKeyFlags flags)
{
	toggleKey(c, true, flags);
	toggleKey(c, false, flags);
}

void typeString(const std::u16string &str)
{
	for (auto it = str.begin(); it != str.end(); ++it) {
		INPUT keyboardInput;
		keyboardInput.type = INPUT_KEYBOARD;
		keyboardInput.ki.wScan = static_cast<WORD>(*it);
		keyboardInput.ki.wVk = 0;
		keyboardInput.ki.time = 0;
		keyboardInput.ki.dwFlags = KEYEVENTF_UNICODE;
		SendInput(1, &keyboardInput, sizeof(keyboardInput));
		keyboardInput.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
		SendInput(1, &keyboardInput, sizeof(keyboardInput));
	}
}