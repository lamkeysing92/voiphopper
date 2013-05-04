/*
    voiphopper - VoIP Hopper
    Copyright (C) 2012 Jason Ostrom <jpo@pobox.com>

    This file is part of VoIP Hopper.

    VoIP Hopper is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    VoIP Hopper is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mac.h"


mac_t * mc_mac_dup (const mac_t *mac)
{
	mac_t *new;

	new = (mac_t *)malloc(sizeof(mac_t));
	memcpy (new, mac, sizeof(mac_t));
	return new;
}


void mc_mac_free (mac_t *mac)
{
	free (mac);
}


void mc_mac_into_string (const mac_t *mac, char *s)
{
	int i;
	for (i=0; i<6; i++) {
		sprintf (&s[i*3], "%02x%s", mac->byte[i], i<5?":":"");
	}
}
unsigned char *mc_macbytes_into_string (unsigned char *macbytes)
{

	unsigned char *string = (unsigned char*) malloc(18);

	int y;
	/*printf("Mac bytes2:  ");
	for (y = 0; y < 6; y++) {
		printf("%02x ",macbytes[y]);
	}
	printf("\n\n");*/

        int i;
        for (i=0; i<6; i++) {
                sprintf (&string[i*3], "%02x%s", macbytes[i], i<5?":":"");
        }

	return string;
}

void mc_mac_random (mac_t *mac, unsigned char last_n_bytes)
{
	/* The LSB of first octet can not be set.  Those are musticast
	 * MAC addresses and not allowed for network device: 
	 * x1:, x3:, x5:, x7:, x9:, xB:, xD: and xF:
	 */

	switch (last_n_bytes) {
	case 6:
		mac->byte[0] = (random()%255) & 0xFE;
	case 5:
		mac->byte[1] = random()%255;
	case 4:
		mac->byte[2] = random()%255;
	case 3:
		mac->byte[3] = random()%255;
	case 2:
		mac->byte[4] = random()%255;
	case 1:
		mac->byte[5] = random()%255;
	}
}

int mc_mac_equal (const mac_t *mac1, const mac_t *mac2)
{
	int i;
	for (i=0; i<6; i++) {
		if (mac1->byte[i] != mac2->byte[i]) {
			return 0;
		}
	}
	return 1;
}


void mc_mac_next (mac_t *mac)
{
	if (++mac->byte[5]) return;
	if (++mac->byte[4]) return;
	if (++mac->byte[3]) return;
	if (++mac->byte[2]) return;
	if (++mac->byte[1]) return;
	if (++mac->byte[0]) return;
	mac->byte[5] = 1;
}


int mc_mac_read_string (mac_t *mac, char *string)
{
	int nbyte = 5;

	/* Check the format */
	if (strlen(string) != 17) {
		fprintf (stderr, "ERROR: Incorrect format: MAC length must be 17. %s(%d)\n", string, strlen(string));
		return -1;
	}
	
	for (nbyte=2; nbyte<16; nbyte+=3) {
		if (string[nbyte] != ':') {
			fprintf (stderr, "Incorrect format: %s\n", string);
			return -1;
		}
	}
	
	/* Read the values */
	for (nbyte=0; nbyte<6; nbyte++) {
		mac->byte[nbyte] = (char) (strtoul(string+nbyte*3, 0, 16) & 0xFF);
	}
	
	return 0;
}