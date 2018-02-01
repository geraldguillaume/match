/*  =========================================================================
    match_simple - match

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
    match_simple - match
@discuss
@end
*/

#include "match_classes.h"

bool isAll=false;
char *first;
int first_len;
char *second;
int second_len;
bool isRegex=false;

/**
 * Supported patterns are :
 * '.*' or '*' are managed by isAll flag status
 * pattern1, pattern1(.)*, ^pattern1(.(*))  pattern1 is managed by first
 * *(.)pattern2,  pattern1(.)*pattern2(.*), ^pattern1(.)*pattern2(.*)  pattern2 is managed by second
 * \, (, | is managed as a pure regex
 * default : managed as a pure regex   
 * @param pattern
 */
void compile(const char *pattern){
    char * ptr_pattern=strdup(pattern);
    bool bfirst_match=true;
    if(streq(pattern,".*")){
        isAll=true;
        return;
    }
    if(streq(pattern,"*")){
        isAll=true;
        return;
    }
    //complex expression will be managed as pure regex
    if(NULL!=strchr(pattern,'\\') ||
            NULL!=strchr(pattern,'(') ||
            NULL!=strchr(pattern,'|')){
        isRegex=true;
        return;
    }
    if(ptr_pattern[0]=='^'){
       ptr_pattern++; 
       bfirst_match=true;
    }
    else if(ptr_pattern[0]=='*' ){
       bfirst_match=false;
       ptr_pattern++;
    }
    else if(ptr_pattern[0]=='.' && ptr_pattern[1]=='*' ){
       bfirst_match=false;
       ptr_pattern+=2;
    }
    
    if (bfirst_match){
        /* check the first token (start with) */
        char* first_token = strchr(ptr_pattern, '*');
        if(first_token!=NULL){
            if(*(first_token-1)=='.'){
                *(first_token-1)=0;
            }else{
                *(first_token)=0;
            }
            first=ptr_pattern;
            first_len=strlen(first);
            ptr_pattern=first_token;
            ptr_pattern+=1;
        }
        else {
            //Not ending by * => so exactly match
            first=ptr_pattern;
            first_len=strlen(first);
            return;
        }
        if(*ptr_pattern==0)
            return;
    }
     
    /* check about second token */
    char* second_token = strchr(ptr_pattern, '*');
        if(second_token!=NULL){
            if(*(second_token-1)=='.'){
                *(second_token-1)=0;
            }else{
                *(second_token)=0;
            }
            second=ptr_pattern;
            second_len=strlen(second);
            ptr_pattern=second_token;
            ptr_pattern+=1;
        }
        else {
            //Not ending by * => so exactly match
            second=ptr_pattern;
            return;
        }
    if(*ptr_pattern==0)
        return;
    //else case
    isRegex = true;
    assert(isRegex==true);
    
}

bool match_simple_test(char* value,zrex_t *rex){
    if(isAll)
        return true;
    int lenVal=strlen(value);
    char *ptrValue=value;
    if(NULL!=first){
        //check value start with "first"
        if(lenVal<first_len)
            return false;
        else{
            char *ptrFirst=first;
            int n=0;
            int found=0;
            while(*(ptrValue)==*(ptrFirst) && (n++)!=first_len){
                found++;
                ptrValue++;
                ptrFirst++;
            }
                
            //not found first string
            if(found!=first_len)return false;
            //found first string and no second sting => so return OK !
            if(NULL==second)return true;
            lenVal-=first_len;
        }
    }
    if(NULL!=second){
        if(lenVal<second_len)
            return false;
        else{
            char *ptrSecond=second;
            int n=0;
            int found=0;
            while(*(ptrValue)!=*(ptrSecond) && n!=lenVal){
                n++;
                ptrValue++;
            }
            //test not found
            if(n==lenVal)return false;
            //found first character of second string, now check all second string
            lenVal-=n;
            if(lenVal<second_len)
                return false;
            n=0;
            found=0;
            while(*(ptrValue)==*(ptrSecond) && (n++)!=second_len){
                found++;
                ptrValue++;
                ptrSecond++;
            }
            //found second string
            return(found==second_len);
        }
    }
    if(isRegex){
        return zrex_matches (rex, value);
    }
    //default => we should never arrive there ..
    return false;

}

bool match_simple(char* pattern, char* value, int loop){
    zrex_t *rex = zrex_new (pattern);
    assert(rex!=NULL);
    bool result = true;
    compile(pattern);
    if(isAll)printf("all is true\n");
    if(isRegex)printf("regex is true\n");
    if(first!=NULL)printf("first %s\n",first);
    if(second!=NULL)printf("second %s\n",second);
    for(int n=0;n<loop;n++){
        result&=match_simple_test(value,rex);
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
        puts ("match_simple [options] pattern value");
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
    zsys_info ("match_simple - match %s %s %d",pattern, value, loop);
    bool rv =  match_simple(pattern, value, loop);
    if(rv)
        zsys_info ("match_simple return true");
    else
        zsys_info ("match_simple return false");
    return 0;
}

