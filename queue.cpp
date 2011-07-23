#include <windows.h>
#include "queue.h"
#include "global.h"

QueueHandler queueManip;

void CALLBACK QueueThread(void) {
	while(GetModuleHandle("battle.snp")) {
		if(!queueManip.queue.empty()) {
			if(GetTickCount() > queueManip.sendQueuedMessage) {
				SendChat(queueManip.queue.top().Text.c_str());
				queueManip.sendQueuedMessage = GetTickCount() + queueManip.CalculateQueueDelayEx((DWORD)queueManip.queue.top().Text.length());
				queueManip.queue.pop();
				DWORD wait = queueManip.sendQueuedMessage - GetTickCount();
				WaitForSingleObject(queueManip.queueEvent, wait);
			}
		} else if(queueManip.queue.empty()) {
			SuspendThread(queueManip.queueThread);
		}
		Sleep(0);
	}
}