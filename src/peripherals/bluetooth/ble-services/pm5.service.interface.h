#pragma once

#include "../../../rower/stroke.model.h"
#include "./base-metrics.service.interface.h"

class IPm5Service : public IBaseMetricsBleService
{
public:
    virtual void broadcastBaseMetrics(const RowingDataModels::RowingMetrics &data) = 0;
};
