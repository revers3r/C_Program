#include <stdio.h>

#pragma warning(disable:4996)
int main(void)
{
	unsigned int ip = 0;
	unsigned char mbf = 0;
	unsigned char ipa = 0;
	unsigned char ipb = 0;
	unsigned char ipc = 0;
	unsigned char ipd = 0;
	unsigned int msk = 0;
	unsigned int msk2 = 0;
	int subb = 0;

	printf("ip 3磊府究 : ");
	scanf("%d", &ipa);
	ip = ipa;

	printf("ip 3磊府究 : %d. ", ipa);
	ip = ip << 8;
	scanf("%d", &ipb);
	ip = ip | ipb;

	printf("ip 3磊府究 : %d.%d. ", ipa, ipb);
	ip = ip << 8;
	scanf("%d", &ipc);

	ip = ip | ipc;
	getchar();
	return 0;
}