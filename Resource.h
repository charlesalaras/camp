#ifdef VULKAN_IMPL
#include <vk/Buffer.h>
#include <vk/Context.h>
#include <vk/Device.h>
#elif  DIRECTX_IMPL
#include <dx12/Buffer.h>
#include <dx12/Context.h>
#include <dx12/Device.h>
#elif  METAL_IMPL
#include <mtl/Buffer.h>
#include <mtl/Context.h>
#include <mtl/Device.h>
#elif  OPENGL_IMPL
#include <gl/Buffer.h>
#include <gl/Context.h>
#include <gl/Device.h>
#elif  GLES_IMPL
#include <gles/Buffer.h>
#include <gles/Context.h>
#include <gles/Device.h>
#else
/*
#include <template/Buffer.h>
#include <template/Context.h>
#include <template/Device.h>
*/
#endif

#include <stdint.h>

struct Handle {  uint32_t value;  };
