
#ifndef ZONAL_STAT_MONITOR_H
#define ZONAL_STAT_MONITOR_H

#include "monitorconfig.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "zonalstat_input.h"

void runZonalStatMonitor( const MonitorConfig& config ) ;


#endif