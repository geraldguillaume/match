/*  =========================================================================
    match_zrex - match

    Copyright (C) 2014 - 2017 Eaton

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    =========================================================================
*/

/*
@header
    match_zrex - match
@discuss
@end
*/

#include "match_classes.h"

bool match_zrex(char* pattern, char* value, int loop){
    zrex_t *rex = zrex_new (pattern);
    bool result = true;
    for(int n=0;n<loop;n++){
        result &= zrex_matches (rex, value);
    }
    return result;
}

int main (int argc, char *argv [])
{
    bool verbose = false;
    int loop = 10;
    int argn;
    if (argc==2 && (streq (argv [1], "--help")
        ||  streq (argv [1], "-h"))) {
        puts ("match_zrex [options] pattern value");
        puts ("  --verbose / -v         verbose test output");
        puts ("  --help / -h            this information");
        puts ("  -l <loop>              loop count [10]");
        return 0;
    }
    if(argc<3){
        puts("need pattern value");
        return 1;
    }
    char *pattern=argv [argc-2];
    char *value=argv [argc-1];
    for (argn = 1; argn < argc-2; argn++) {
        if (streq (argv [argn], "--verbose")
        ||  streq (argv [argn], "-v"))
            verbose = true;
        else
        if (streq (argv [argn], "--loop")
        ||  streq (argv [argn], "-l"))
            loop = atoi(argv [++argn]);
        
        else {
            printf ("Unknown option: %s\n", argv [argn]);
            return 1;
        }
    }
    if (verbose){
        zsys_debug("debug enabled");
    }
    //  Insert main code here
    zsys_info ("match_zrex - match %s %s %d",pattern, value, loop);
    bool rv =  match_zrex(pattern, value, loop);
    if(rv)
        zsys_info ("match_zrex return true");
    else
        zsys_info ("match_zrex return false");
    return 0;
}
