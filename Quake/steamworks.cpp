#ifdef USE_STEAMWORKS

extern "C" {
#include "steamworks.h"
}

#include "steam/steam_api.h"

#include <cstdlib>

// Too many includes that violate C++ standard :/

extern "C" void* Q_malloc(size_t size);
extern "C" void Q_memset (void *dest, int fill, size_t count);

extern "C" int Q_strlen (const char *str);
extern "C" void Q_strcpy (char *dest, const char *src);

extern "C" void Sys_Error (const char *error, ...);

static int steamworks_initialized = 0;
static int steamworks_appid = -1;

int Steam_IsInitialized(){
	return steamworks_initialized;
}

int Steam_Init(int appid, int restart_if_necessary){
	if(steamworks_initialized){
		return 1;
	}

	if(SteamAPI_RestartAppIfNecessary(appid)){
		return 0;
	}

	steamworks_appid = appid;

	SteamErrMsg init_err_msg;
	if(SteamAPI_InitEx(&init_err_msg) != k_ESteamAPIInitResult_OK){
		Sys_Error("Error in SteamAPI_Init: %s", init_err_msg);
		return 0;
	}

	steamworks_initialized = 1;

	return 1;
}

int Steam_GetWorkshopDirectories(char **dirs){
	if (!steamworks_initialized)
	{
		return 0;
	}

	ISteamUGC* const UGC = SteamUGC();
	if (!UGC)
	{
		return 0;
	}

	const uint32 num_subscribed_items = UGC->GetNumSubscribedItems();
	if (!num_subscribed_items)
	{
		return 0;
	}

	PublishedFileId_t *file_ids = reinterpret_cast<PublishedFileId_t*>(Q_malloc(sizeof(PublishedFileId_t) * num_subscribed_items));
	Q_memset(file_ids, 0, sizeof(PublishedFileId_t) * num_subscribed_items);

	const uint32 num_items_retrieved = UGC->GetSubscribedItems(file_ids, num_subscribed_items);

	char *result_str = nullptr;
	uint64 malloced_size = 0;

	for (uint32 item_idx = 0; item_idx < num_items_retrieved; item_idx++)
	{
		uint64 size_on_disk;
		char dir_temp[1024] = { 0 };
		if (UGC->GetItemInstallInfo(file_ids[item_idx], &size_on_disk, dir_temp, 1023, nullptr))
		{
			uint32 dir_len = Q_strlen(dir_temp);

			uint64 old_malloced_size = malloced_size;
			uint64 new_malloced_size = 0;
			char *new_result_str = nullptr;

			if (old_malloced_size > 0)
			{
				new_malloced_size = old_malloced_size + dir_len + 2; // add space for ';' and NULL
				new_result_str = reinterpret_cast<char*>(Q_malloc(new_malloced_size));

				Q_strcpy(new_result_str, result_str);
				Q_strcpy(new_result_str + old_malloced_size, ";");
				Q_strcpy(new_result_str + old_malloced_size + 1, dir_temp);
				free(result_str);
			}
			else
			{
				new_malloced_size = dir_len + 1; // add space for NULL
				new_result_str = reinterpret_cast<char*>(Q_malloc(new_malloced_size));

				Q_strcpy(new_result_str, dir_temp);
			}

			result_str = new_result_str;
		}
	}

	*dirs = result_str;

	free(file_ids);

	return num_subscribed_items;
}

#endif // USE_STEAMWORKS
