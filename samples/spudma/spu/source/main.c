#include <spu_intrinsics.h>
#include <spu_mfcio.h>

#define TAG 1

extern void spu_thread_exit(uint32_t);

/* wait for dma transfer to be finished */
static void wait_for_completion(void) {
	mfc_write_tag_mask(1<<TAG);
	spu_mfcstat(MFC_TAG_UPDATE_ALL);
}

int main(uint64_t ea, uint64_t outptr, uint64_t arg3, uint64_t arg4)
{
	/* memory-aligned buffer (vectors always are properly aligned) */
	volatile vec_uchar16 v;

	/* fetch the 16 bytes using dma */
	mfc_get(&v, ea, 16, TAG, 0, 0);
	wait_for_completion();

	/* compare all characters with the small 'a' character code */
	vec_uchar16 cmp = spu_cmpgt(v, spu_splats((unsigned char)('a'-1)));

	/* for all small characters, we remove 0x20 to get the corresponding capital*/
	vec_uchar16 sub = spu_splats((unsigned char)0x20) & cmp;

	/* convert all small characters to capitals */
	v = v - sub;

	/* send the updated vector to ppe */
	mfc_put(&v, ea, 16, TAG, 0, 0);
	wait_for_completion();

	/* send a message to inform the ppe program that the work is done */
	uint32_t ok __attribute__((aligned(16))) = 1;
	mfc_put(&ok, outptr, 4, TAG, 0, 0);
	wait_for_completion();

	/* properly exit the thread */
	spu_thread_exit(0);
	return 0;
}
