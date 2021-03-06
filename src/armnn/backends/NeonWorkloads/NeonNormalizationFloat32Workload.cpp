//
// Copyright © 2017 Arm Ltd. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "NeonNormalizationFloat32Workload.hpp"
#include "backends/NeonLayerSupport.hpp"
#include "backends/ArmComputeUtils.hpp"
#include "backends/ArmComputeTensorUtils.hpp"

namespace armnn
{

arm_compute::Status NeonNormalizationWorkloadValidate(const TensorInfo& input,
                                                      const TensorInfo& output,
                                                      const NormalizationDescriptor& descriptor)
{
    const arm_compute::TensorInfo aclInput = armcomputetensorutils::BuildArmComputeTensorInfo(input);
    const arm_compute::TensorInfo aclOutput = armcomputetensorutils::BuildArmComputeTensorInfo(output);

    arm_compute::NormalizationLayerInfo normalizationInfo =
            armcomputetensorutils::BuildArmComputeNormalizationLayerInfo(descriptor);

    return arm_compute::NENormalizationLayer::validate(&aclInput, &aclOutput, normalizationInfo);
}

NeonNormalizationFloat32Workload::NeonNormalizationFloat32Workload(const NormalizationQueueDescriptor& descriptor,
                                                   const WorkloadInfo& info,
                                                   std::shared_ptr<arm_compute::MemoryManagerOnDemand>& memoryManager)
    : FloatWorkload<NormalizationQueueDescriptor>(descriptor, info)
    , m_NormalizationLayer(memoryManager)
{
    m_Data.ValidateInputsOutputs("NeonNormalizationFloat32Workload", 1, 1);
    std::string reasonIfUnsupported;
    if (!IsNeonNormalizationDescParamsSupported(&reasonIfUnsupported, m_Data.m_Parameters))
    {
        throw UnimplementedException(reasonIfUnsupported);
    }

    // Input and output tensors have to have the same dimensionality.
    if (info.m_InputTensorInfos[0].GetShape()[1] != info.m_OutputTensorInfos[0].GetShape()[1]
        || info.m_InputTensorInfos[0].GetShape()[0] != info.m_OutputTensorInfos[0].GetShape()[0]
        || info.m_InputTensorInfos[0].GetShape()[3] != info.m_OutputTensorInfos[0].GetShape()[3]
        || info.m_InputTensorInfos[0].GetShape()[2] != info.m_OutputTensorInfos[0].GetShape()[2])
    {
        throw InvalidArgumentException("Normalization requires input and output tensors to have equal dimensionality.");
    }

    arm_compute::ITensor& input = boost::polymorphic_downcast<INeonTensorHandle*>(m_Data.m_Inputs[0])->GetTensor();
    arm_compute::ITensor& output = boost::polymorphic_downcast<INeonTensorHandle*>(m_Data.m_Outputs[0])->GetTensor();

    const arm_compute::NormType normType =
        ConvertNormalizationAlgorithmChannelToAclNormType(m_Data.m_Parameters.m_NormChannelType);
    arm_compute::NormalizationLayerInfo normalizationInfo(normType,
                                                          m_Data.m_Parameters.m_NormSize,
                                                          m_Data.m_Parameters.m_Alpha,
                                                          m_Data.m_Parameters.m_Beta,
                                                          m_Data.m_Parameters.m_K,
                                                          false);

    m_NormalizationLayer.configure(&input, &output, normalizationInfo);
}

void NeonNormalizationFloat32Workload::Execute() const
{
    ARMNN_SCOPED_PROFILING_EVENT_NEON("NeonNormalizationFloat32Workload_Execute");
    m_NormalizationLayer.run();
}

} //namespace armnn
