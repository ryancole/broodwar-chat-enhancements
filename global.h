#pragma once

// typedefs and structs
typedef bool funcptr_1(unsigned long textaddr);
typedef void funcptr_2();

// function definitions and pointers to them
bool HandleOutgoingText(unsigned long textaddr);
extern funcptr_1 *handleoutgoingtextptr;

void EnterChannelEvent();
extern funcptr_2 *enterchanneleventptr;

void QuickReply(char *text);
void SetWhisperBackTarget(char *buffer);
void SendWhisperBack(char *text);
void AddChat(const char *username, const char *message);
void AddMessage(const char *format, ...);
void SendChat(const char *buffer);
void QueueChat(const char *format, ...);
void SpoofLobbyName(char *text);
void SpoofChannelName(char *text);
void PrintUptimes();

// global variables
extern char *lastwhisperfrom;
extern char *lastwhisperto;
extern char *replytext;
extern char *messageboxtext;
extern char *currentchannel;
extern __int64 startTime;