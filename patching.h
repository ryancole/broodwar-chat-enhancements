#pragma once

int PatchAddress(int type, unsigned long AddressToPatch, unsigned long AddressToUse, unsigned long PadSize);
unsigned long PatcherThread(void);