#include <kinc/graphics5/graphics.h>
#include <kinc/graphics5/indexbuffer.h>

#import <Metal/Metal.h>

id getMetalDevice(void);

kinc_g5_index_buffer_t *currentIndexBuffer = NULL;

static void index_buffer_unset(kinc_g5_index_buffer_t *buffer) {
	if (currentIndexBuffer == buffer) currentIndexBuffer = NULL;
}

void kinc_g5_index_buffer_init(kinc_g5_index_buffer_t *buffer, int indexCount, bool gpuMemory) {
	buffer->impl.mtlBuffer = nil;
	buffer->impl.myCount = indexCount;
	buffer->impl.gpuMemory = gpuMemory;
	id<MTLDevice> device = getMetalDevice();
	MTLResourceOptions options = MTLCPUCacheModeWriteCombined;
#ifdef KINC_APPLE_SOC
	options |= MTLResourceStorageModeShared;
#else
	if (gpuMemory) {
		options |= MTLResourceStorageModeManaged;
	}
	else {
		options |= MTLResourceStorageModeShared;
	}
#endif
	buffer->impl.mtlBuffer = [device newBufferWithLength:sizeof(int) * indexCount options:options];
}

void kinc_g5_index_buffer_destroy(kinc_g5_index_buffer_t *buffer) {
	buffer->impl.mtlBuffer = 0;
	index_buffer_unset(buffer);
}

int *kinc_g5_index_buffer_lock(kinc_g5_index_buffer_t *buf) {
	id<MTLBuffer> buffer = buf->impl.mtlBuffer;
	return (int*)[buffer contents];
}

void kinc_g5_index_buffer_unlock(kinc_g5_index_buffer_t *buf) {
#ifndef KINC_APPLE_SOC
	if (buf->impl.gpuMemory) {
		id<MTLBuffer> buffer = buf->impl.mtlBuffer;
		NSRange range;
		range.location = 0;
		range.length = kinc_g5_index_buffer_count(buf) * 4;
		[buffer didModifyRange:range];
	}
#endif
}

void kinc_g5_internal_index_buffer_set(kinc_g5_index_buffer_t *buffer) {
	currentIndexBuffer = buffer;
}

int kinc_g5_index_buffer_count(kinc_g5_index_buffer_t *buffer) {
	return buffer->impl.myCount;
}
