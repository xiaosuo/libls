#include <ls.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	DECLARE_BITMAP(bm1, 250);
	DECLARE_BITMAP(bm2, 250);
	DECLARE_BITMAP(bm3, 250);
	int bit;
	char buf[4096];

	bitmap_zero(bm1, 250);
	set_bit(2, bm1);
	set_bit(3, bm1);
	set_bit(65, bm1);
	set_bit(101, bm1);
	set_bit(123, bm1);
	for_each_set_bit(bit, bm1, 250)
		printf("%d ", bit);
	printf("\n");
	printf("clear 101\n");
	clear_bit(101, bm1);
	for_each_set_bit(bit, bm1, 250)
		printf("%d ", bit);
	printf("\n");
	printf("change bit 3 100\n");
	change_bit(3, bm1);
	change_bit(100, bm1);
	for_each_set_bit(bit, bm1, 250)
		printf("%d ", bit);
	printf("\n");
	printf("printf\n");
	bitmap_scnprintf(buf, sizeof(buf), bm1, 250);
	printf("%s\n", buf);

	bitmap_zero(bm2, 250);
	set_bit(9, bm2);
	set_bit(99, bm2);
	set_bit(100, bm2);
	printf("bm2\n");
	for_each_set_bit(bit, bm2, 250)
		printf("%d ", bit);
	printf("\n");
	printf("printf\n");
	bitmap_scnprintf(buf, sizeof(buf), bm2, 250);
	printf("%s\n", buf);

	printf("or\n");
	bitmap_or(bm3, bm1, bm2, 250);
	bitmap_scnprintf(buf, sizeof(buf), bm3, 250);
	printf("%s\n", buf);

	printf("xor\n");
	bitmap_xor(bm3, bm1, bm2, 250);
	bitmap_scnprintf(buf, sizeof(buf), bm3, 250);
	printf("%s\n", buf);

	printf("and\n");
	bitmap_and(bm3, bm1, bm2, 250);
	bitmap_scnprintf(buf, sizeof(buf), bm3, 250);
	printf("%s\n", buf);

	return EXIT_SUCCESS;
}
