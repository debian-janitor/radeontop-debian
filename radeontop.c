/*
    Copyright (C) 2012 Lauri Kasanen

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "radeontop.h"
#include <getopt.h>

#ifdef ENABLE_NLS
#include <locale.h>
#endif

const void *area;

void die(const char * const why) {
	puts(why);
	exit(1);
}

static void help(const char * const me, const unsigned int ticks) {
	printf(_("\n\tRadeonTop for R600 and above.\n\n"
		"\tUsage: %s [-ch] [-b bus] [-t ticks]\n\n"
		"-b --bus 3		Pick card from this PCI bus\n"
		"-c --color		Enable colors\n"
		"-h --help		Show this help\n"
		"-t --ticks 50		Samples per second (default %u)\n"),
		me, ticks);
	die("");
}

unsigned int readgrbm() {

	const void *ptr = area + 0x10;

	const unsigned int *inta = ptr;

	return *inta;
}

int main(int argc, char **argv) {

	unsigned int ticks = 120;
	unsigned char color = 0;
	unsigned char bus = 0;

	// Translations
#ifdef ENABLE_NLS
	setlocale(LC_ALL, "");
	bindtextdomain("radeontop", "/usr/share/locale");
	textdomain("radeontop");
#endif

	// opts
	const struct option opts[] = {
		{"bus", 1, 0, 'b'},
		{"color", 0, 0, 'c'},
		{"help", 0, 0, 'h'},
		{"ticks", 1, 0, 't'},
		{0, 0, 0, 0}
	};

	while (1) {
		int c = getopt_long(argc, argv, "b:cht:", opts, NULL);
		if (c == -1) break;

		switch(c) {
			case 'h':
			case '?':
				help(argv[0], ticks);
			break;
			case 't':
				ticks = atoi(optarg);
			break;
			case 'c':
				color = 1;
			break;
			case 'b':
				bus = atoi(optarg);
			break;
		}
	}

	// init
	const unsigned int pciaddr = init_pci(bus);

	const int family = getfamily(pciaddr);
	if (!family)
		puts(_("Unknown Radeon card. <= R500 won't work, new cards might."));

	const char * const cardname = family_str[family];

	initbits(family);

	// runtime
	collect(&ticks);

	printf(_("Collecting data, please wait....\n"));

	present(ticks, cardname, color);

	munmap((void *) area, MMAP_SIZE);
	return 0;
}
