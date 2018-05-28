/* pls imagine a license */

#ifndef __UTIL_VOLUME_H__
#define __UTIL_VOLUME_H__

#include "util/util_types.h"
#include "util/util_vector.h"

/* Insert some
 * comments here. */

CCL_NAMESPACE_BEGIN

static const int CUBE_SIZE = 8;
static const float4 isovalue = make_float4(0.5f, 0.5f, 0.5f, 0.5f);

struct VolumeTile {
    float4 voxels[CUBE_SIZE * CUBE_SIZE * CUBE_SIZE];
};

size_t compute_index(size_t x, size_t y, size_t z, size_t width=CUBE_SIZE, size_t height=CUBE_SIZE, size_t depth=CUBE_SIZE, size_t min=0);

int compute_tile_resolution(int res);

void create_sparse_grid(float4 *voxels,
                        //float4 isovalue, /* threshold value */
                        const size_t width,
                        const size_t height,
                        const size_t depth,
                        vector<VolumeTile> grid,
                        vector<int> offsets);

float4 get_voxel(VolumeTile *grid, int *offsets,
                 size_t x, size_t y, size_t z,
                 size_t width, size_t height, size_t depth);

CCL_NAMESPACE_END

#endif /*__UTIL_VOLUME_H__*/
