#include "colorspace.h"

#include "cuda/dynlink_loader.h"

int cs_map_cuda(CsConverter *conv, CUdeviceptr *data, int *linesize);
void cs_deinit_cuda(CsConverter *conv);
void copy_tex_to_cuda(CUdeviceptr dst, GLuint src_tex, int *linesize);