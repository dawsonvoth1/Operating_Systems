#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pager_lru(Pentry q[MAXPROCESSES]) { 

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

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for a predictive pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int prevPage[MAXPROCESSES];
    static int pageNum[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];
    static int pageFreq[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];

    /* Local vars */
    int proc;
    int page;
    int pc;
    int future_page;
    int prev_page;

    /* initialize static vars on first run */
    if(!initialized){
	/* Init complex static vars here */
        for(int i=0; i<MAXPROCESSES; i++){
            for (int j=0; j<MAXPROCPAGES; j++){
                for(int k=0; k<MAXPROCPAGES; k++){
                    pageNum[i][j][k] = -1;
                    pageFreq[i][j][k] = 0;
                }
            }
            prevPage[i] = 0;
        }
        initialized = 1;
    }
    
    /* TODO: Implement Predictive Paging */
    // used a transition matrix, but no probabilities, solely based on previous references

    // first count how many times a page is referenced, based on previous page
    for(proc = 0; proc < MAXPROCESSES; proc++){
        if (q[proc].active != 1) continue;              // select only active processes  
        
        pc = q[proc].pc; 		            	        // program counter for process
        page = pc/PAGESIZE; 		        	        // current page the PC is on  

        // previous page 
        prev_page = prevPage[proc];
        prevPage[proc] = page;
         
        for(int i = 0; i < MAXPROCPAGES; i++){
            // ensure previous page and current page are not the same 
            if (prev_page != page) {
                // page out previous page as it is only used for counting access here 
                pageout(proc, prev_page);
                
                // if first time page is being accessed 
                if(pageNum[proc][prev_page][i] == -1){
                    pageNum[proc][prev_page][i] = page;
                    pageFreq[proc][prev_page][i] = 1;
                    break;

                // if not first time page is being accessed
                }else if(pageNum[proc][prev_page][i] == page){
                    pageNum[proc][prev_page][i] = page;
                    pageFreq[proc][prev_page][i] ++;
                    break;
                }
            }
        }
    }

    // then use those calculations for predicting future pages to page in
    for(proc = 0; proc < MAXPROCESSES; proc++){
        if (q[proc].active != 1) continue;              // select only active processes
        
        // future page
        future_page = (q[proc].pc + PAGEWAIT)/PAGESIZE;
        
        // for each page following next page, if it has been referenced, page it in 
        for (int i = 0; i < MAXPROCPAGES; i++){
            if(pageFreq[proc][future_page][i] > 0){
                pagein(proc, pageNum[proc][future_page][i]);
            }
        }
    }

    // then use LRU to do current paging
    pager_lru(q);

    /* advance time for next pageit iteration */
    tick++;
}
