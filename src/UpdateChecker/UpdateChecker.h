#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void update_checker_check_update(const char *latest_release_url);

void update_checker_close(void);

#ifdef __cplusplus
}
#endif
