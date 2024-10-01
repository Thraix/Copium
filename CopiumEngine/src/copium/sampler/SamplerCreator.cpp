#include "copium/sampler/SamplerCreator.h"

#include "copium/util/Common.h"

namespace Copium
{

  SamplerCreator::SamplerCreator() = default;

  SamplerCreator::SamplerCreator(const MetaFileClass& metaClass)
  {
    if (metaClass.HasValue("min-filter"))
      minFilter = GetFilterFromString(metaClass.GetValue("min-filter"));
    if (metaClass.HasValue("mag-filter"))
      magFilter = GetFilterFromString(metaClass.GetValue("mag-filter"));
    if (metaClass.HasValue("address-mode"))
      addressMode = GetAddressModeFromString(metaClass.GetValue("address-mode"));
  }

  void SamplerCreator::SetMinFilter(VkFilter minFilter)
  {
    SamplerCreator::minFilter = minFilter;
  }

  void SamplerCreator::SetMagFilter(VkFilter magFilter)
  {
    SamplerCreator::magFilter = magFilter;
  }

  VkFilter SamplerCreator::GetFilterFromString(const std::string& str) const
  {
    if (str == "nearest")
      return VK_FILTER_NEAREST;
    else if (str == "linear")
      return VK_FILTER_LINEAR;
    else
      CP_ABORT("Invalid texture filtering: %s", str.c_str());
  }

  VkSamplerAddressMode SamplerCreator::GetAddressModeFromString(const std::string& str) const
  {
    if (str == "clamp-to-edge")
      return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    else if (str == "repeat")
      return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    else
      CP_ABORT("Invalid texture address mode: %s", str.c_str());
  }
}
