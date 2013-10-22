#include <ls.h>

int main(int argc, char *argv[])
{
	pr("pr\n");
	pr_emerg("pr_emerg\n");
	pr_alert("pr_alert\n");
	pr_crit("pr_crit\n");
	pr_err("pr_err\n");
	pr_warning("pr_warning\n");
	pr_warn("pr_warng\n");
	pr_notice("pr_notice\n");
	pr_info("pr_info\n");
	pr_debug("pr_debug\n");

	return EXIT_SUCCESS;
}
