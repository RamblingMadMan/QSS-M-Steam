/* Steamworks api integration support */

#if !defined(_STEAMWORKS_H_)
#define _STEAMWORKS_H_

#ifdef USE_STEAMWORKS

int Steam_IsInitialized();

int Steam_Init(int appid, int restart_if_necessary);

int Steam_GetWorkshopDirectories(char **dirs);

#endif

#endif
