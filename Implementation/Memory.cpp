#include"Vole_Machine.h"



Memory::Memory(Screen& screen)
    : screen(screen) {
}

bool Memory:: isEvenAddress(int address) {

    return address % 2 == 0;
};


bool Memory::fits(int start, int byteCount) {
    if (start < 0 || start >= 255 || byteCount <= 0) {
        return false;
    }

   return start + byteCount - 1 < 255;


};

void  Memory::write(uint8_t address, uint8_t value){

    if (address==255)
        screen.write(value);
    else    
        cells[address]=value;
}

uint8_t  Memory::read(uint8_t address){
    if (address == 255)
        return 0;
    else {
        return cells[address];
    }
}

bool Memory::loadBytes(int start, const std::vector<uint8_t>& bytes){
    if(fits(start ,bytes.size())){
        for(int i =0 ; i <=bytes.size()-1 ;i++){
            write(start+i,bytes[i]);
        }
        return true;
    }
    else{
        return false ;
    }

}

void Memory::clear(){
    for(int i =0 ;i<255 ;i++){
        cells[i]=0;
    }
}