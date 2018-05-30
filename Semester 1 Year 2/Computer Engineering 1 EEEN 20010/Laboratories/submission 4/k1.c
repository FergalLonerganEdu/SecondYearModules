#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct {
	double value, weight, cost;
	const char * name;
} items[] = { /* value , weight, cost, letter attributed*/
	{7, 1,  4,	"a"},
	{6, 5, 10,	"b"},
	{6,11, 15,	"c"},
	{5, 4,  8,  "d"},
	{4, 2, 20,  "e"},
	{3, 6,  8,  "f"},
    {2, 4,  9,  "g"},
    {2,10,  3,  "h"},
    {1, 6,  7,  "i"},
    {0, 0,  0, 0}
    };

int best_pack[16] = {0}, pack[16] = {0};
double best_v = 0;

void opt_pack(int i, double max_weight, double max_cost, double v)
{
	double value;
	int t = max_cost / items[i].weight;
	pack[i] = max_weight / items[i].cost;

	if (pack[i] > t)
        {
        pack[i] = t;
        }

	while (pack[i] >= 0) {
		value = v + pack[i] * items[i].value;
		if (!items[i + 1].name) {
			if (value > best_v) {
				best_v = value;
				memcpy(best_pack, pack, sizeof(int) * (1 + i));
			}
			return;
		}
		opt_pack(i + 1, max_weight - pack[i] * items[i].cost, max_cost - pack[i] * items[i].weight, value);
		pack[i]--;
	}
}

int main(void)
{
	int i;
	const int max_c = 45,max_w = 13;


	opt_pack(0, max_w, max_c, 0);
	printf("Max value attainable: %g \n", best_v);
	for (i = 0; items[i].name; i++)
		printf("%d %s\n", best_pack[i], items[i].name);

	return 0;
}
