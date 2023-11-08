#pragma once

#include "tl_common.h"
#include "zcl_include.h"

typedef struct {
    ev_timer_event_t *timerReportMinEvt;
    ev_timer_event_t *timerReportMaxEvt;
    reportCfgInfo_t  *pEntry;
    u32               time_posted;
} app_reporting_t;


void app_reportAttrTimerStart();

extern app_reporting_t app_reporting[ZCL_REPORTING_TABLE_NUM];