/* pls imagine a license */

#include "util/util_volume.h"

/* Insert some
 * comments here. */

CCL_NAMESPACE_BEGIN

size_t compute_index(size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, size_t min)
{
	if(x < min || y < min || z < min || x >= width || y >= height || z >= depth)
		return min-1;
	return x + width * (y + z * height);
}

int compute_tile_resolution(int res)
{
    if(res % CUBE_SIZE == 0)
        return res / CUBE_SIZE;
    return res / CUBE_SIZE + 1;
}

void create_sparse_grid(float4 *voxels,
                        //float4 isovalue, /* threshold value */
                        const size_t width,
                        const size_t height,
                        const size_t depth,
                        vector<VolumeTile> grid,
                        vector<int> offsets)
{

    /* Resize vectors to tiled resolution. */
    int active_tile_count = 0;
    int total_tile_count = compute_tile_resolution(width) +
                           compute_tile_resolution(height) +
                           compute_tile_resolution(depth);
    grid.resize(total_tile_count); /* Overalloc because we don't know the number of active tiles yet. */
    offsets.resize(total_tile_count);
    total_tile_count = 0;

    for(int z=0 ; z < depth ; z += CUBE_SIZE) {
        for(int y=0 ; y < height ; y += CUBE_SIZE) {
            for(int x=0 ; x < width ; x += CUBE_SIZE) {

                bool is_empty = true;
                int indexes[CUBE_SIZE*CUBE_SIZE*CUBE_SIZE];
                int c = 0;

                /* Check if tile is empty. */
                for(int k=z ; k < z+CUBE_SIZE ; ++k) {
                    for(int j=y ; j < y+CUBE_SIZE ; ++j) {
                        for(int i=x ; i < x+CUBE_SIZE ; ++i) {
                            int index = compute_index(i, j, k, width, height, depth);
                            /* Store index for later. */
                            indexes[c] = index;
                            ++c;
                            if(is_empty) {
                                if(index>=0) {
                                    /* All values are greater than the isovalue's. */
                                    if((voxels[index] >= isovalue) == make_int4(1, 1, 1, 1)){
                                        is_empty = false;
                                    }
                                }
                            }
                        }
                    }
                }

                /* Create VolumeTile and copy voxel values in that range. */
                if(is_empty) {
                    offsets[total_tile_count] = -1;
                }
                else {
                    VolumeTile tile;
                    for(c=0 ; c<CUBE_SIZE*CUBE_SIZE*CUBE_SIZE ; ++c) {
                        if(indexes[c] < 0)
                            /* Out of bounds of original image, store empty voxel. */
                            tile.voxels[c] = make_float4(0.0f, 0.0f, 0.0f, 0.0f);
                        else
                            tile.voxels[c] = voxels[indexes[c]];
                    }
                    grid[active_tile_count] = tile;
                    offsets[total_tile_count] = active_tile_count;
                    ++active_tile_count;
                }

                ++total_tile_count;

            }
        }
    }

    grid.resize(active_tile_count);

}

float4 get_voxel(VolumeTile *grid, int *offsets,
                 size_t x, size_t y, size_t z,
                 size_t width, size_t height, size_t depth)
{
    /* Get the 1D array tile index of the tile the voxel (x, y, z) is in. */
    int tile_index = compute_index(x/CUBE_SIZE, y/CUBE_SIZE, z/CUBE_SIZE, width, height, depth);
    if(tile_index < 0)
        return make_float4(0.0f, 0.0f, 0.0f, 0.0f);
    /* Get the index of the tile in the sparse grid. */
    int grid_index = offsets[tile_index];
    if (grid_index < 0 || grid_index > width+height+depth)
        return make_float4(0.0f, 0.0f, 0.0f, 0.0f);
    /* Get tile and look up voxel in tile. */
    int voxel_index = compute_index(x%CUBE_SIZE, y%CUBE_SIZE, z%CUBE_SIZE);
    return grid[grid_index].voxels[voxel_index];
}

CCL_NAMESPACE_END
