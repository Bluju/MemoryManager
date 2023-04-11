/*
 * Virtual Memory Manager
 */
#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>

using namespace std;
int main() {
    //read input file and translate each logical address to a physical address
    //output the contents of the signed byte stored at each physical address.

    //Variables needed for statistics
    int totalAddresses = 0;
    int pageFaults = 0;
    int TLBhits = 0;
    float pageFaultRate;
    float TLBhitRate;
    bool tlbHit = false;

    //variables needed for program functionality
    int virtualAddress{0}; // virtual address
    int physicalAddress{0}; // physical address
    int nextFrame{0};
    int pageNumber{0};
    int frameNumber{0};
    int offset{0};
    int value{0};
    char buffer[256]; // temporary storage when reading from backing store

    // initialize the value of every spot in the Page Table to -1
    int* pageTable = new int[256];
    for(int i = 0; i < 256; i++){
        pageTable[i] = -1;
    }
    // initialize the value of every space in memory as -1
    int* memory = new int[256*256];
    for(int i = 0; i < 256; i++){
        memory[i] = -1;
    }
    // initialize the value of every space in the TLB as integer minimum
    int* TLB = new int[16];
    for(int i = 0; i < 16; i++){
        TLB[i] = INT_MIN;
    }
    string inputAddress; //holds the virtual address given in the file
    ifstream file("addresses.txt");
    while(getline(file,inputAddress)){
        stringstream ss(inputAddress);
        ss >> virtualAddress;
        ++totalAddresses;

        pageNumber = (virtualAddress >> 8) & 0xFF; //shift the virtual address 8 bits right so that the page number is the last 8 bits, then gives the value of the last 8 bits.
        offset = virtualAddress & 0xFF; //gives the value of the last 8 bits in virtualAddress
        // check if the frame number stored at pageTable[pageNumber] is in the TLB
        for(int i = 0; i < 16; i++){
            if(TLB[i] == pageTable[pageNumber]){
                ++TLBhits;
                tlbHit = true;
                frameNumber = TLB[i];
                break;
            }
        }
        //Check for Page Faults
        if(pageTable[pageNumber] == -1 && !tlbHit)
        {
            //page fault occurred
            ++pageFaults;
            //read 256-byte page from BACKING_STORE and store in available page frame in physical memory
            FILE *backingStore = fopen("BACKING_STORE.bin", "rb"); // open Backing Store binary file
            fseek(backingStore,pageNumber*256,SEEK_SET); // find the page number
            fread(buffer,1,256,backingStore); // read the page number into the temporary buffer variable
            for(int i = 0; i < 256; i++){
                memory[nextFrame * 256 + i] = static_cast<int>(buffer[i]); //put the contents of buffer into the correct frame in memory
            }
            nextFrame++;
            frameNumber = nextFrame - 1;
            pageTable[pageNumber] = frameNumber;
            TLB[frameNumber % 16] = frameNumber; // mod(16) will replace the oldest page with the newest page
            fclose(backingStore);
        }
        else if(!tlbHit){
            frameNumber = pageTable[pageNumber];
        }
        physicalAddress = frameNumber * 256 + offset;
        value = memory[physicalAddress];
        printf("Virtual address: %d Physical Address: %d Value: %d\n", virtualAddress, physicalAddress, value);
        tlbHit = false;
    }

    //Statistics
    pageFaultRate = ((float)pageFaults/(float)totalAddresses);
    TLBhitRate = (float)TLBhits/(float)totalAddresses;
    printf("Number of Translated Addresses: %d\nPage Faults: %d\nPage-fault rate: %0.3f\n",totalAddresses,pageFaults,pageFaultRate);
    printf("TLB Hits: %d\nTLB Hit Rate: %0.3f",TLBhits, TLBhitRate);
    return 0;
}

