/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * This file contains the skeleton for an LRU pager
 */

#include <stdio.h> 
#include <stdlib.h>
#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];

    /* Local vars */
    int proc;
    int page;

    /* initialize timestamp array on first run */
    if (!initialized) {
        for (proc=0; proc < MAXPROCESSES; proc++) {
            for (page=0; page < MAXPROCPAGES; page++) {
                timestamps[proc][page] = 0; 
            }
        }
        initialized = 1;
    }
    
    /* TODO: Implement LRU Paging */
    for (proc = 0; proc < MAXPROCESSES; proc++) {
	    if (q[proc].active != 1) continue;              // select only active processes    

        int pc = q[proc].pc; 		            	    // program counter for process
        page = pc/PAGESIZE; 		        	        // current page the PC is on                  
       
        // if page is not in memory:
        if (!q[proc].pages[page]){
            // try to page in, if it doesn't work:
            if(!pagein(proc,page)){
                // variables for oldest timestamp and victim page to page out
                int old = tick;
                int vic;
                // find lru page
                for (int i = 0; i < MAXPROCPAGES; i++){
                    // if page is in memory and timestamp of page is older than current oldest timestamp
                    if (q[proc].pages[i] && timestamps[proc][i]<old){
                        // update oldest timestamp and victim page to page out
                        old = timestamps[proc][i];
                        vic = i;
                    } 
                }
                // page it out and we're done if successful 
                if (pageout(proc,vic) == 1) break;
            }
        } else {
            // if page is in memory, update timestamp of the page
            timestamps[proc][page] = tick;
        }
    }

    /* advance time for next pageit iteration */
    tick++;
} 
