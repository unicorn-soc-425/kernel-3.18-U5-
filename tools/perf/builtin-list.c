/*
 * builtin-list.c
 *
 * Builtin list command: list all event types
 *
 * Copyright (C) 2009, Thomas Gleixner <tglx@linutronix.de>
 * Copyright (C) 2008-2009, Red Hat Inc, Ingo Molnar <mingo@redhat.com>
 * Copyright (C) 2011, Red Hat Inc, Arnaldo Carvalho de Melo <acme@redhat.com>
 */
#include "builtin.h"

#include "perf.h"

#include "util/parse-events.h"
#include "util/cache.h"
#include "util/pmu.h"
<<<<<<< HEAD
#include "util/parse-options.h"
=======
#include <subcmd/parse-options.h>

static bool desc_flag = true;
>>>>>>> v4.9.227

int cmd_list(int argc, const char **argv, const char *prefix __maybe_unused)
{
	int i;
<<<<<<< HEAD
	const struct option list_options[] = {
		OPT_END()
	};
	const char * const list_usage[] = {
		"perf list [hw|sw|cache|tracepoint|pmu|event_glob]",
		NULL
	};

=======
	bool raw_dump = false;
	bool long_desc_flag = false;
	struct option list_options[] = {
		OPT_BOOLEAN(0, "raw-dump", &raw_dump, "Dump raw events"),
		OPT_BOOLEAN('d', "desc", &desc_flag,
			    "Print extra event descriptions. --no-desc to not print."),
		OPT_BOOLEAN('v', "long-desc", &long_desc_flag,
			    "Print longer event descriptions."),
		OPT_END()
	};
	const char * const list_usage[] = {
		"perf list [<options>] [hw|sw|cache|tracepoint|pmu|sdt|event_glob]",
		NULL
	};

	set_option_flag(list_options, 0, "raw-dump", PARSE_OPT_HIDDEN);

>>>>>>> v4.9.227
	argc = parse_options(argc, argv, list_options, list_usage,
			     PARSE_OPT_STOP_AT_NON_OPTION);

	setup_pager();

<<<<<<< HEAD
	if (argc == 0) {
		print_events(NULL, false);
=======
	if (!raw_dump && pager_in_use())
		printf("\nList of pre-defined events (to be used in -e):\n\n");

	if (argc == 0) {
		print_events(NULL, raw_dump, !desc_flag, long_desc_flag);
>>>>>>> v4.9.227
		return 0;
	}

	for (i = 0; i < argc; ++i) {
<<<<<<< HEAD
		if (i)
			putchar('\n');
		if (strncmp(argv[i], "tracepoint", 10) == 0)
			print_tracepoint_events(NULL, NULL, false);
		else if (strcmp(argv[i], "hw") == 0 ||
			 strcmp(argv[i], "hardware") == 0)
			print_events_type(PERF_TYPE_HARDWARE);
		else if (strcmp(argv[i], "sw") == 0 ||
			 strcmp(argv[i], "software") == 0)
			print_events_type(PERF_TYPE_SOFTWARE);
		else if (strcmp(argv[i], "cache") == 0 ||
			 strcmp(argv[i], "hwcache") == 0)
			print_hwcache_events(NULL, false);
		else if (strcmp(argv[i], "pmu") == 0)
			print_pmu_events(NULL, false);
		else if (strcmp(argv[i], "--raw-dump") == 0)
			print_events(NULL, true);
		else {
			char *sep = strchr(argv[i], ':'), *s;
			int sep_idx;

			if (sep == NULL) {
				print_events(argv[i], false);
=======
		char *sep, *s;

		if (strcmp(argv[i], "tracepoint") == 0)
			print_tracepoint_events(NULL, NULL, raw_dump);
		else if (strcmp(argv[i], "hw") == 0 ||
			 strcmp(argv[i], "hardware") == 0)
			print_symbol_events(NULL, PERF_TYPE_HARDWARE,
					event_symbols_hw, PERF_COUNT_HW_MAX, raw_dump);
		else if (strcmp(argv[i], "sw") == 0 ||
			 strcmp(argv[i], "software") == 0)
			print_symbol_events(NULL, PERF_TYPE_SOFTWARE,
					event_symbols_sw, PERF_COUNT_SW_MAX, raw_dump);
		else if (strcmp(argv[i], "cache") == 0 ||
			 strcmp(argv[i], "hwcache") == 0)
			print_hwcache_events(NULL, raw_dump);
		else if (strcmp(argv[i], "pmu") == 0)
			print_pmu_events(NULL, raw_dump, !desc_flag,
						long_desc_flag);
		else if (strcmp(argv[i], "sdt") == 0)
			print_sdt_events(NULL, NULL, raw_dump);
		else if ((sep = strchr(argv[i], ':')) != NULL) {
			int sep_idx;

			if (sep == NULL) {
				print_events(argv[i], raw_dump, !desc_flag,
							long_desc_flag);
>>>>>>> v4.9.227
				continue;
			}
			sep_idx = sep - argv[i];
			s = strdup(argv[i]);
			if (s == NULL)
				return -1;

			s[sep_idx] = '\0';
<<<<<<< HEAD
			print_tracepoint_events(s, s + sep_idx + 1, false);
=======
			print_tracepoint_events(s, s + sep_idx + 1, raw_dump);
			print_sdt_events(s, s + sep_idx + 1, raw_dump);
			free(s);
		} else {
			if (asprintf(&s, "*%s*", argv[i]) < 0) {
				printf("Critical: Not enough memory! Trying to continue...\n");
				continue;
			}
			print_symbol_events(s, PERF_TYPE_HARDWARE,
					    event_symbols_hw, PERF_COUNT_HW_MAX, raw_dump);
			print_symbol_events(s, PERF_TYPE_SOFTWARE,
					    event_symbols_sw, PERF_COUNT_SW_MAX, raw_dump);
			print_hwcache_events(s, raw_dump);
			print_pmu_events(s, raw_dump, !desc_flag,
						long_desc_flag);
			print_tracepoint_events(NULL, s, raw_dump);
			print_sdt_events(NULL, s, raw_dump);
>>>>>>> v4.9.227
			free(s);
		}
	}
	return 0;
}
