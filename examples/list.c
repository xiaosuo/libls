#include <ls.h>
#include <stdio.h>

struct num_list {
	struct list_head	link;
	int			num;
};

int main(int argc, char *argv[])
{
	struct num_list a1, a2, a3;
	LIST_HEAD(head);
	struct num_list *nl;

	a1.num = 1;
	a2.num = 2;
	a3.num = 3;

	list_add(&a2.link, &head);
	list_add(&a1.link, &head);
	list_add_tail(&a3.link, &head);

	list_for_each_entry(nl, &head, link)
		printf("%d\n", nl->num);

	return EXIT_SUCCESS;
}
