name	wow32
type	win32

  1 stdcall WOWGetDescriptor(long long) WOWGetDescriptor
  2 stdcall WOWCallback16(long long) WOWCallback16
  3 stdcall WOWCallback16Ex(ptr long long ptr ptr) WOWCallback16Ex
  4 stub    WOWDirectedYield16
  5 stdcall WOWGetVDMPointer(long long long) WOWGetVDMPointer
  6 stdcall WOWGetVDMPointerFix(long long long) WOWGetVDMPointerFix
  7 stdcall WOWGetVDMPointerUnfix(long) WOWGetVDMPointerUnfix
  8 stub    WOWGlobalAlloc16
  9 stdcall WOWGlobalAllocLock16(long long ptr) WOWGlobalAllocLock16
 10 stub    WOWGlobalFree16
 11 stub    WOWGlobalLock16
 12 stub    WOWGlobalLockSize16
 13 stub    WOWGlobalUnlock16
 14 stdcall WOWGlobalUnlockFree16(long) WOWGlobalUnlockFree16
 15 stub    WOWHandle16
 16 stdcall WOWHandle32(long long) WOWHandle32
 17 stub    WOWYield16
