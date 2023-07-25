#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void update_checker_check_update(const char *latest_release_url,
				 const char *plugin_name,
				 const char *plugin_version);

void update_checker_close(void);

#ifdef __cplusplus
}
#endif
