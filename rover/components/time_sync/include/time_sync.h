#ifndef TIME_SYNC_H
#define TIME_SYNC_H




time_t obtain_time(void);
void initialize_sntp(void);
void sntp_sync_time(struct timeval *tv);
void time_sync_notification_cb(struct timeval *tv);
time_t sync_time(void);


#endif