#include<iostream>
#include<cstdint>
#include<bitset>
#include<vector>
#include<algorithm>
using namespace std;

#ifdef _WIN32
#include<fcntl.h>
#include<io.h>
#endif

   // 初始哈希值
uint32_t h1=0x6a09e667;
uint32_t h2=0xbb67ae85;
uint32_t h3=0x3c6ef372;
uint32_t h4=0xa54ff53a;
uint32_t h5=0x510e527f;
uint32_t h6=0x9b05688c;
uint32_t h7=0x1f83d9ab;
uint32_t h8=0x5be0cd19;

// 常数表
uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};
//将轮换函数改编成宏定义
#define rightRotate(x, n) ((x >> n) | (x << (32 - n)))
//将32位数转化成4个8位的数
void uint32_to_4uint8(uint32_t value, uint8_t* x){
    x[0]=(value>>24)&0xff;
    x[1]=(value>>16)&0xff;
    x[2]=(value>>8)&0xff;
    x[3]=value&0xff;
}

int main(){
#ifdef _WIN32
    setmode(fileno(stdin), O_BINARY);
    setmode(fileno(stdout), O_BINARY);
#endif
    //数据读取
    FILE *inputFile = fopen("1.bin", "rb");
    int need=0;
    uint64_t length;
    int len=0;
    int num=0;

    while(1){
        vector<uint8_t> mingwen;
        mingwen.reserve(64);
        uint8_t tmp;
        num=64;
        while(num && fread(&tmp,sizeof(uint8_t),1,inputFile)!=0){
            mingwen.push_back(tmp);
            num--;
            len++;
        }

        if(num!=0 && need==0){
            num--;
            mingwen.push_back(0x80);
            length=uint64_t(len*8);

            if(((len+1)*8)%512<=448){
                need=(448-((len+1)*8)%512)/8;
            }else{
                need=(960-((len+1)*8)%512)/8;
            }
        }

        while(num && need){
            num--;
            need--;
            mingwen.push_back(0x00);
        }
        
        if(mingwen.size()<64){
            for(int i=0;i<8;i++){
                mingwen.push_back((uint8_t)(length>>(56-8*i)));
            }
        }

        uint32_t w[64]={0};
        for(int i=0;i<16;i++){
            for(int j=0;j<4;j++){
                w[i]+=((uint32_t)mingwen[i*4+j])<<(24-8*j);
            }
        }
        
        for(int i=16;i<64;i++){
            uint32_t s0=rightRotate(w[i-15],7)^rightRotate(w[i-15],18)^(w[i-15]>>3);
            uint32_t s1=rightRotate(w[i-2],17)^rightRotate(w[i-2],19)^(w[i-2]>>10);
            w[i]=w[i-16]+s0+w[i-7]+s1;
        }

        uint32_t a=h1,b=h2,c=h3,d=h4,e=h5,f=h6,g=h7,h=h8;
        //主循环
        for(int i=0;i<64;i++){
            uint32_t sigma1=rightRotate(e,6)^rightRotate(e,11)^rightRotate(e,25);
            uint32_t ch=(e&f)^((~e)&g);
            uint32_t t1=h+sigma1+ch+k[i]+w[i];
            uint32_t sigma0=rightRotate(a,2)^rightRotate(a,13)^rightRotate(a,22);
            uint32_t Maj=(a&b)^(a&c)^(b&c);
            uint32_t t2=sigma0+Maj;

            h=g;
            g=f;
            f=e;
            e=d+t1;
            d=c;
            c=b;
            b=a;
            a=t1+t2;
        }

        h1+=a;
        h2+=b;
        h3+=c;
        h4+=d;
        h5+=e;
        h6+=f;
        h7+=g;
        h8+=h;
        
        if(num>0){
            break;
        }
    }

    vector<uint8_t> result;
    uint8_t tmp_[4];
    uint32_to_4uint8(h1,tmp_);
    result.insert(result.end(),tmp_,tmp_+4);
    uint32_to_4uint8(h2,tmp_);
    result.insert(result.end(),tmp_,tmp_+4);
    uint32_to_4uint8(h3,tmp_);
    result.insert(result.end(),tmp_,tmp_+4);
    uint32_to_4uint8(h4,tmp_);
    result.insert(result.end(),tmp_,tmp_+4);
    uint32_to_4uint8(h5,tmp_);
    result.insert(result.end(),tmp_,tmp_+4);
    uint32_to_4uint8(h6,tmp_);
    result.insert(result.end(),tmp_,tmp_+4);
    uint32_to_4uint8(h7,tmp_);
    result.insert(result.end(),tmp_,tmp_+4);
    uint32_to_4uint8(h8,tmp_);
    result.insert(result.end(),tmp_,tmp_+4);

    for(int i=0;i<32;i++){
        cout << hex << (int)result[i] << " ";
        if(i%16==15){
            cout << endl;
        }
    }

    fclose(inputFile);
    return 0;
}