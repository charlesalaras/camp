#ifndef _VK_BUFFER_H_
#define _VK_BUFFER_H_
#include <stdint.h>
#include <vulkan/vulkan.h>

// Insert defines for buffer types / configs

struct Buffer {
  uint32_t mDesc;
  VkBuffer mAPIResource;
};

#endif
