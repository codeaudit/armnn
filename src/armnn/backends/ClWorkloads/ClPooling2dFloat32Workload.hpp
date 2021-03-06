//
// Copyright © 2017 Arm Ltd. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#pragma once

#include "backends/ClWorkloadUtils.hpp"
#include "backends/ClWorkloads//ClPooling2dBaseWorkload.hpp"

namespace armnn
{
class ClPooling2dFloat32Workload : public ClPooling2dBaseWorkload<DataType::Float16, DataType::Float32>
{
public:
    ClPooling2dFloat32Workload(const Pooling2dQueueDescriptor& descriptor, const WorkloadInfo& info);
    void Execute() const override;

};

} //namespace armnn
