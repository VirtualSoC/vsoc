#include "colorspace.h"

#include "cuda/dynlink_loader.h"

int cs_map_cuda(CsConverter *conv, CUdeviceptr *data, int *linesize);
void cs_deinit_cuda(CsConverter *conv);
