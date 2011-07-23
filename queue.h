#pragma once
#include <string>
#include <queue>
#define MAX_SEND	223

void CALLBACK QueueThread(void);

struct Message {
	std::string Text;
	int Priority;

	Message();
	Message(const std::string& t, int p)
	: Text(t), Priority(p) {}
};

class QueueHandler {
private:
	class MessageComparer {
	public:
		bool operator()(const Message& first, const Message& second) {
			return (first.Priority > second.Priority);
		}
	};

public:
	HANDLE queueThread, queueEvent;
	DWORD sendQueuedMessage;
	std::priority_queue<Message, std::vector<Message>, MessageComparer> queue;

	QueueHandler() {
		queueThread = NULL;
		sendQueuedMessage = 0;
	}

	inline void AddQueue(const std::string &message, int priority) {
		if(message.length() > MAX_SEND) {
			std::string whisperto, splitmsg, wholemsg;
			int times = 1;
			bool whisp = false;
			wholemsg = message;
			if(wholemsg.substr(0, 3) == "/w ") {
				whisp = true;
				whisperto = wholemsg.substr(3, wholemsg.substr(3, wholemsg.length()-4).find_first_of(" "));
			}
			do {
				if(times == 1)
					splitmsg = wholemsg.substr(0, 200);
				else
					splitmsg = "/w " + whisperto + " <cont'd> " + wholemsg.substr(0, 200);
				if(queue.empty()) {
					queue.push(Message(splitmsg, priority));
					ResumeThread(queueThread);
				} else if(!queue.empty()) {
					queue.push(Message(splitmsg, priority));
				}
				wholemsg = wholemsg.substr(200, wholemsg.length()-200);
				times++;
			} while(wholemsg.length() > 200);
			if(wholemsg.length() > 0) {
				wholemsg = "<cont'd> " + wholemsg;
				if(whisp)
					wholemsg = "/w " + whisperto +  " <cont'd> " + wholemsg;
				if(queue.empty()) {
					queue.push(Message(wholemsg, priority));
					ResumeThread(queueThread);
				} else if(!queue.empty()) {
					queue.push(Message(wholemsg, priority));
				}
			}
		} else {
			if(queue.empty()) {
				queue.push(Message(message, priority));
				ResumeThread(queueThread);
			} else if(!queue.empty()) {
				queue.push(Message(message, priority));
			}
		}
	}

	inline void ClearQueue() {
		sendQueuedMessage = 0;
		CalculateQueueDelayEx(0);
		SetEvent(queueEvent);
	}

	inline DWORD CalculateQueueDelayEx(DWORD bytes) {
		static unsigned long lasttick      = 0;
		static unsigned long delay		   = 0;
		static unsigned long consecsends   = 0;
	 
		unsigned long tick				   = GetTickCount();
	 
		if((lasttick == 0) || (tick - lasttick > delay + 1000))
			delay = consecsends = 0;
		else {
			if(consecsends == 5)
				consecsends = 0;
	 
			if ((bytes >= 0) && (bytes < 64))
				delay = 2000;
			else if ((bytes >= 64) && (bytes < 104))
				delay = 4000;
			else if ((bytes >= 104) && (bytes <= 259))
				delay = 6000;
			else
				delay = 25 * bytes;
	 
			delay += consecsends * 100;
		}
	 
		lasttick = tick;
		consecsends++;
	 
		return delay;
	}
};

extern QueueHandler queueManip;