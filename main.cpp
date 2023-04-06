/*
 * Virtual Memory Manager
 *
 */
#include <iostream>
#include <fstream>

using namespace std;
int main() {
    //read input file and translate each logical address to a physical address
    //output the contents of the signed byte stored at each physical address.


    int virtualAddress{0}; // virtual address
    int physicalAddress{0}; // physical address
    int nextFrame{0};
    int pageNumber{0};
    int frameNumber{0};
    int offset{0};
    char value{0};
    char buffer[256]; // temporary storage when reading from backing store

    int* pageTable = new int[256];
    int* memory = new int[256*256];
    for(int i = 0; i < 256; i++){
        pageTable[i] = -1; // initialize the value of every spot in the Page Table to -1


    }
    //cout << "Before loop\n";
    while(cin >> virtualAddress){
        //read through entire input file
        //cout << "In loop\n";
        pageNumber = (virtualAddress >> 8) & 0xFF; //shift the virtual address 8 bits right so that the page number is the last 8 bits, then gives the value of the last 8 bits.
        offset = virtualAddress & 0xFF; //gives the value of the last 8 bits in virtualAddress
        //printf("Offset: %d\n",offset);
        //Check for Page Faults
        if(pageTable[pageNumber] == -1)
        {
            //page fault occurred
            //read 256-byte page from BACKING_STORE and store in available page frame in physical memory
            ifstream backingStore ("BACKING_STORE.bin");//ios::out|ios::binary|ios::ate
            backingStore.seekg(pageNumber*256);
            backingStore.read(buffer,sizeof(buffer));
            for(int i = 0; i < 256; i++){
                memory[nextFrame * 256 + i] = static_cast<int>(buffer[i]);
            }
            pageTable[pageNumber] = nextFrame;
            nextFrame++;
            backingStore.close();
        }
        frameNumber = pageTable[pageNumber];
        //printf("frameNumber: %d\n",frameNumber);
        physicalAddress = frameNumber * 256 + offset;
        value = memory[physicalAddress];
        printf("Virtual address: %d Physical Address: %d Value: %d\n", virtualAddress, physicalAddress, value);

    }
    return 0;
}

