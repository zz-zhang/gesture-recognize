#include "test_keyboardresp.h"

void TestKeyboard(){
	wchar_t *windowname = L"无标题 - 记事本";
	HWND hq = FindWindow(NULL, windowname);
	if (hq != NULL)
		cout << "y";
	else { 
		cout << "n";
		return; 
	}
	SendMessage(hq, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	for (int i = 0; i < 100; ++i){
		keybd_event(65, 0, 0, 0);
		keybd_event(65, 0, KEYEVENTF_KEYUP, 0);
	}
}