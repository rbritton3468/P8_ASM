#include <fstream>
#include <queue>
#include <iostream>
#include <sstream>
#include <map>
#include <variant>
#include <bits/algorithmfwd.h>

/*
  encoding          instruction   description
    
  0000aaaabbbbtttt  sub rt,ra,rb  regs[t] = regs[a] - regs[b]

  1000iiiiiiiitttt  movl rt,i     regs[t] = sign_extend(i)
  1001iiiiiiiitttt  movh rt,i     regs[t] = (regs[t] & 0xff) | (i << 8)

  1110aaaa0000tttt  jz rt,ra      pc = (regs[ra] == 0) ? regs[rt] : pc + 2
  1110aaaa0001tttt  jnz rt,ra     pc = (regs[ra] != 0) ? regs[rt] : pc + 2
  1110aaaa0010tttt  js rt,ra      pc = (regs[ra] < 0) ? regs[rt] : pc + 2
  1110aaaa0011tttt  jns rt,ra     pc = (regs[ra] >= 0) ? regs[rt] : pc + 2

  1111aaaa0000tttt  ld rt,ra      regs[t] = mem[regs[a]]
  1111aaaa0001tttt  st rt,ra      mem[regs[a]] = regs[t]

  1111aaaa0010tttt  ldp rt,ra     regs[t] = mem[regs[a]]
                                  regs[(t+1)%16] = mem[regs[a]+2]

  1111aaaa0011tttt  stp rt,ra     mem[regs[a]] = regs[t]
                                  mem[regs[a]+2] = regs[(t+1)%16]
*/

void ASMjz(size_t hex){
    std::cout<<"jz "<<(hex&0xF)<<","<<((hex&0xF00)>>8)<<std::endl;
}

void ASMjnz(size_t hex){
    std::cout<<"jnz "<<(hex&0xF)<<","<<((hex&0xF00)>>8)<<std::endl;
}
void ASMjs(size_t hex){
    std::cout<<"js "<<(hex&0xF)<<","<<((hex&0xF00)>>8)<<std::endl;
}
void ASMjns(size_t hex){
    std::cout<<"jns "<<(hex&0xF)<<","<<((hex&0xF00)>>8)<<std::endl;
}

void ASMld(size_t hex){
    std::cout<<"ld "<<(hex&0xF)<<",["<<((hex&0xF00)>>8)<<"]"<<std::endl;
}
void ASMst(size_t hex){
    std::cout<<"st "<<(hex&0xF)<<",["<<((hex&0xF00)>>8)<<"]"<<std::endl;
}
void ASMldp(size_t hex){
    std::cout<<"ldp "<<(hex&0xF)<<",["<<((hex&0xF00)>>8)<<"]"<<std::endl;
}
void ASMstp(size_t hex){
    std::cout<<"stp "<<(hex&0xF)<<",["<<((hex&0xF00)>>8)<<"]"<<std::endl;
}
void ASMsub(size_t hex){
    std::cout<<"sub "<<(hex&0xF)<<","<<((hex&0xF00)>>8)<<","<<((hex&0xF0)>>4)<<std::endl;
}
void ASMmovl(size_t hex){
    std::cout<<"movl "<<(hex&0xF)<<","<<((hex&0xFF0)>>4)<<std::endl;
}
void ASMmovh(size_t hex){
    std::cout<<"movh "<<(hex&0xF)<<","<<((hex&0xFF0)>>4)<<std::endl;
}

void ASMjmp(size_t hex){
    switch (hex&0xF0>>4) {
    case 0:
        ASMjz(hex);
        break;
    case 1:
        ASMjnz(hex);
        break;
    case 2:
        ASMjs(hex);
        break;
    case 3:
        ASMjns(hex);
        break;
    default:
        std::cout<< "hault"<<std::endl;
        break;
    }
}

void ASMmem(size_t hex){
    switch (hex&0xF0>>4) {
    case 0:
        ASMld(hex);
        break;
    case 1:
        ASMst(hex);
        break;
    case 2:
        ASMldp(hex);
        break;
    case 3:
        ASMstp(hex);
        break;
    default:
        std::cout<< "hault"<<std::endl;
        break;
    }
}
void ASMcall(size_t hex){
    switch ((hex&0xF000)>>12) {
    case 0:
        ASMsub(hex);
        break;
    case 8:
        ASMmovl(hex);
        break;
    case 9:
        ASMmovh(hex);
        break;
    case 14:
        ASMjmp(hex);
        break;
    case 15:
        ASMmem(hex);
        break;
    default:
        std::cout<< "hault"<<std::endl;
        break;
    };
}

size_t hexToNum(const std::string& hex_string) {
    try {
      return std::stoul(hex_string, nullptr, 16);
    } catch (const std::invalid_argument& e) {
      std::cerr << "Invalid argument: " << e.what() << std::endl;
      return 0; 
    } catch (const std::out_of_range& e) {
       std::cerr << "Out of range: " << e.what() << std::endl;
       return 0;
    }
  }

int main(int argc, char *argv[]) {
    std::queue<size_t> HexQueue;
    std::queue<std::string> ASMQueue;

    if ((argc < 2)) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[argc-1], std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }

    std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        std::istringstream stream(fileContents);
        std::string line;
        int lineNumber = 0;

        while (std::getline(stream, line)) {
            if(lineNumber!=0)HexQueue.push(hexToNum(line));
            else   std::cout<<line<<std::endl;
            lineNumber++;
        }

        while (!HexQueue.empty()) {
            ASMcall(HexQueue.front());
            HexQueue.pop();
        }

    return 0;
}



