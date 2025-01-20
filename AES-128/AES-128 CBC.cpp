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
//s盒
const uint8_t s_box[256]={0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
                            0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
                            0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
                            0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
                            0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
                            0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
                            0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
                            0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
                            0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
                            0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
                            0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
                            0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
                            0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
                            0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
                            0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
                            0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16};
//逆s盒
const uint8_t inv_s_box[256]={0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
                                0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
                                0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
                                0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
                                0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
                                0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
                                0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
                                0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
                                0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
                                0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
                                0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
                                0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
                                0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
                                0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
                                0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
                                0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D};

const uint8_t col_box[4][4]={2,3,1,1,
                            1,2,3,1,
                            1,1,2,3,
                            3,1,1,2};

const uint8_t inv_col_box[4][4]={0x0e, 0x0b, 0x0d, 0x09,
                                0x09, 0x0e, 0x0b, 0x0d,
                                0x0d, 0x09, 0x0e, 0x0b,
                                0x0b, 0x0d, 0x09, 0x0e};
//移位
vector<uint8_t> RotWord(vector<uint8_t> temp){
    vector<uint8_t> result(4);
    for(int i=0;i<4;i++){
        result[i]=temp[(i+1)%4];
    }
    return result;
}
//字节替换
vector<uint8_t> SubByte(vector<uint8_t> temp){
    vector<uint8_t> result(4);
    for(int i=0;i<4;i++){
        result[i]=s_box[int(temp[i])];
    }
    return result;
}
//32位的异或
vector<uint8_t> Iand(vector<uint8_t> temp,uint32_t num){
    vector<uint8_t> result(4);
    result[0]=temp[0]^(uint8_t)(num>>24);
    for(int i=1;i<4;i++){
        result[i]=temp[i];
    }
    return result;
}
//密钥扩展
vector<vector<uint8_t>> Key_expansion(uint8_t key[]){
    vector<uint32_t> Rcon(10);
    Rcon[0]=(1<<24);
    for(int i=1;i<8;i++){
        Rcon[i]=Rcon[i-1]*2;
        // cout << hex << Rcon[i] << " ";
    }
    Rcon[8]=0x1b000000;
    Rcon[9]=0x36000000;
    
    vector<vector<uint8_t>> w(44,vector<uint8_t>(4));
    for(int i=0;i<4;i++){
        w[i][0]=key[4*i];
        w[i][1]=key[4*i+1];
        w[i][2]=key[4*i+2];
        w[i][3]=key[4*i+3];
    }
    for(int i=4;i<44;i++){
        vector<uint8_t> temp(4);
        for(int j=0;j<4;j++){
            temp[j]=w[i-1][j];
        }
        if(i%4==0){
            temp=SubByte(RotWord(temp));
            temp=Iand(temp,Rcon[i/4-1]);
        }
        for(int k=0;k<4;k++){
            w[i][k]=w[i-4][k]^temp[k];
        }
    }
    return w;
}
//字节替换（矩阵下的）
vector<vector<uint8_t>> SubByte_2d(vector<vector<uint8_t>> temp){
    vector<vector<uint8_t>> result(4,vector<uint8_t>(4));
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            result[i][j]=s_box[int(temp[i][j])];
        }
    }
    return result;
}
//行移位
vector<vector<uint8_t>> Rotword_2d(vector<vector<uint8_t>> temp){
    vector<vector<uint8_t>> result(4,vector<uint8_t>(4));
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            result[i][j]=temp[i][(j+i)%4];
        }
    }
    return result;
}
//G(2^8)下的乘法
uint8_t mul(uint8_t a, uint8_t b){
    uint8_t result=0;
    uint8_t temp_b=b;
    //对a和b的每一位进行处理
    for(int i=0;i<8;i++) {
        if(temp_b&0x01){
            result^=a;  // 如果 b 的最低位是 1，则将 a 加到结果中
        }
        //乘以x,即左移一位，并判断是否超过了8位
        a=(a<<1)^((a&0x80)?0x11B:0x00); //如果a最高位为1，则需要取模x^8+x^4+x^3+x+1
        temp_b=temp_b>>1;  // b 向右移一位
    }
    return result;
}
//列混淆
vector<vector<uint8_t>> Mixcol(vector<vector<uint8_t>> temp){
    vector<vector<uint8_t>> result(4,vector<uint8_t>(4,uint8_t(0)));
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            for(int k=0;k<4;k++){
                result[i][j]^=mul(col_box[i][k],temp[k][j]);
            }
        }
    }
    return result;
}
//解密的字节替换
vector<vector<uint8_t>> inv_SubByte_2d(vector<vector<uint8_t>> temp){
    vector<vector<uint8_t>> result(4,vector<uint8_t>(4));
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            // int x=int((temp[i][j])>>4);
            // int y=int(temp[i][j]&0x0f);
            result[i][j]=inv_s_box[int(temp[i][j])];
        }
    }
    return result;
}
//解密的行移位
vector<vector<uint8_t>> inv_Rotword_2d(vector<vector<uint8_t>> temp){
    vector<vector<uint8_t>> result(4,vector<uint8_t>(4));
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            result[i][j]=temp[i][(j+4-i)%4];
        }
    }
    return result;
}
//解密的列混淆
vector<vector<uint8_t>> inv_Mixcol(vector<vector<uint8_t>> temp){
    vector<vector<uint8_t>> result(4,vector<uint8_t>(4,uint8_t(0)));
    for(int i=0;i<4;i++){
        for(int k=0;k<4;k++){
            for(int j=0;j<4;j++){
                result[i][j]^=mul(inv_col_box[i][k],temp[k][j]);
            }
        }
    }
    return result;
}
//输出结果
void print_result(uint8_t result[]){
    for(int i=0;i<16;i+=1){
        fwrite(&result[i],sizeof(uint8_t),1,stdout);
    }
}

int main(){
#ifdef _WIN32
    setmode(fileno(stdin), O_BINARY);
    setmode(fileno(stdout), O_BINARY);
#endif
    uint8_t op;
    //input
    fread(&op,sizeof(uint8_t),1,stdin);
    uint8_t key[16];
    uint8_t IV[16];
    for(int i=0;i<16;i++){
        fread(&key[i],sizeof(uint8_t),1,stdin);
    }
    for(int i=0;i<16;i++){
        fread(&IV[i],sizeof(uint8_t),1,stdin);
    }
    
    uint8_t l[4];
    for(int i=0;i<4;i++){
        fread(&l[i],sizeof(uint8_t),1,stdin);
    }

    uint32_t len=(l[3] << 24) | (l[2] << 16) | (l[1] << 8) | l[0];
    int Nr=10;
    //密钥扩展成11组轮密钥
    vector<vector<uint8_t>> w=Key_expansion(key);
    uint8_t temp[4][4];
    
    if(op==0x01){
        //jiami
        uint32_t mi_len;
        if(len%16==0){
            mi_len=len+16;
        }else{
            mi_len=(len/16+1)*16;
        }
        int num=mi_len/16;
        int begin=0;

        uint8_t data[16];
        uint8_t result[16];
        
        for(int i=0;i<16;i++){
            result[i]=IV[i];
        }

        uint8_t ipl=(uint8_t)(mi_len-len);
        while(begin<num){
            for(int i=0;i<16;i++){
                if(16*begin+i<int(len)){
                    fread(&data[i],sizeof(uint8_t),1,stdin);
                }else{
                    data[i]=ipl;
                }
            }
            //Addxi
            uint8_t state[4][4];
            for(int i=0;i<16;i++){
                state[i%4][i/4]=data[i]^result[i]; 
            }

            for(int r=0;r<Nr-1;r++){
                //AddroundKey
                for(int j=0;j<4;j++){
                    for(int k=0;k<4;k++){
                        state[k][j]^=w[r*4+j][k];
                    }
                }
                //SubBytes
                //以下效果同SubByte_2d(state);
                for(int i=0;i<4;i++){
                    for(int j=0;j<4;j++){
                        state[i][j]=s_box[int(state[i][j])];
                    }
                }
                //ShiftRows
                //以下效果同Rotword_2d(state);
                uint8_t a=state[1][0];
                state[1][0]=state[1][1];
                state[1][1]=state[1][2];
                state[1][2]=state[1][3];
                state[1][3]=a;

                uint8_t b=state[2][0];
                uint8_t c=state[2][1];
                state[2][0]=state[2][2];
                state[2][1]=state[2][3];
                state[2][2]=b;
                state[2][3]=c;

                uint8_t d=state[3][0];
                state[3][0]=state[3][3];
                state[3][3]=state[3][2];
                state[3][2]=state[3][1];
                state[3][1]=d;
                //MixColumns
                //以下效果同Mixcol(state);
                for(int i=0;i<4;i++){
                    for(int j=0;j<4;j++){
                        uint8_t t=uint8_t(0);
                        for(int k=0;k<4;k++){
                            t^=mul(col_box[i][k],state[k][j]);
                        }
                        temp[i][j]=t;
                    }
                }
                for(int i=0;i<4;i++){
                    for(int j=0;j<4;j++){
                        state[i][j]=temp[i][j];
                    }
                }
            }
            //最后一轮
            //AddroundKey
            for(int j=0;j<4;j++){
                for(int k=0;k<4;k++){
                    state[k][j]^=w[36+j][k];
                }
            }
            //SubBytes
            // state=SubByte_2d(state);
            for(int i=0;i<4;i++){
                for(int j=0;j<4;j++){
                    state[i][j]=s_box[int(state[i][j])];
                }
            }
            //ShiftRows
            uint8_t a=state[1][0];
            state[1][0]=state[1][1];
            state[1][1]=state[1][2];
            state[1][2]=state[1][3];
            state[1][3]=a;

            uint8_t b=state[2][0];
            uint8_t c=state[2][1];
            state[2][0]=state[2][2];
            state[2][1]=state[2][3];
            state[2][2]=b;
            state[2][3]=c;

            uint8_t d=state[3][0];
            state[3][0]=state[3][3];
            state[3][3]=state[3][2];
            state[3][2]=state[3][1];
            state[3][1]=d;
            //AddroundKey
            for(int j=0;j<4;j++){
                for(int k=0;k<4;k++){
                    state[k][j]^=w[40+j][k];
                }
            }

            begin++;
            for(int i=0;i<16;i++){
                result[i]=state[i%4][i/4];
            }
            
            for(int i=0;i<16;i+=1){
                fwrite(&result[i],sizeof(uint8_t),1,stdout);
            }
        }   
    }else{
        //jiemi
        int begin=0;
        int num=len/16;

        uint8_t data[16];
        uint8_t result[16];

        for(int i=0;i<16;i++){
            result[i]=IV[i];
        }
        
        while(begin<num){
            for(int i=0;i<16;i++){
                fread(&data[i],sizeof(uint8_t),1,stdin);
            }
            //Addxi
            uint8_t state[4][4];
            for(int i=0;i<16;i++){
                state[i%4][i/4]=data[i];
            }
            //AddRoundkey
            for(int j=0;j<4;j++){
                for(int k=0;k<4;k++){
                    state[k][j]^=w[40+j][k];
                }
            }
            for(int r=Nr-1;r>=1;r--){
                //ShiftRows
                //以下效果同inv_Rotword_2d(state);
                uint8_t a=state[1][0];
                state[1][0]=state[1][3];
                state[1][3]=state[1][2];
                state[1][2]=state[1][1];
                state[1][1]=a;

                uint8_t b=state[2][0];
                uint8_t c=state[2][1];
                state[2][0]=state[2][2];
                state[2][1]=state[2][3];
                state[2][2]=b;
                state[2][3]=c;

                uint8_t d=state[3][0];
                state[3][0]=state[3][1];
                state[3][1]=state[3][2];
                state[3][2]=state[3][3];
                state[3][3]=d;
                //SubByte
                //以下效果同inv_SubByte_2d(state);
                for(int i=0;i<4;i++){
                    for(int j=0;j<4;j++){
                        state[i][j]=inv_s_box[int(state[i][j])];
                    }
                }
                //AddRoundkey
                for(int j=0;j<4;j++){
                    for(int k=0;k<4;k++){
                        state[k][j]^=w[r*4+j][k];
                    }
                }
                //MixColumn
                //以下效果同inv_Mixcol(state);
                for(int i=0;i<4;i++){
                    for(int j=0;j<4;j++){
                        uint8_t t=uint8_t(0);
                        for(int k=0;k<4;k++){
                            t^=mul(inv_col_box[i][k],state[k][j]);
                        }
                        temp[i][j]=t;
                    }
                }
                for(int i=0;i<4;i++){
                    for(int j=0;j<4;j++){
                        state[i][j]=temp[i][j];
                    }
                }
            }
            //ShiftRows
            // state=inv_Rotword_2d(state);
            uint8_t a=state[1][0];
            state[1][0]=state[1][3];
            state[1][3]=state[1][2];
            state[1][2]=state[1][1];
            state[1][1]=a;

            uint8_t b=state[2][0];
            uint8_t c=state[2][1];
            state[2][0]=state[2][2];
            state[2][1]=state[2][3];
            state[2][2]=b;
            state[2][3]=c;

            uint8_t d=state[3][0];
            state[3][0]=state[3][1];
            state[3][1]=state[3][2];
            state[3][2]=state[3][3];
            state[3][3]=d;
            //SubByte
            // state=inv_SubByte_2d(state);
            for(int i=0;i<4;i++){
                for(int j=0;j<4;j++){
                    state[i][j]=inv_s_box[int(state[i][j])];
                }
            }
            //AddRoundkey
            for(int j=0;j<4;j++){
                for(int k=0;k<4;k++){
                    state[k][j]^=w[j][k];
                }
            }
            for(int i=0;i<16;i++){
                state[i%4][i/4]^=IV[i];
                IV[i]=data[i];
            }
            begin++;
            for(int i=0;i<16;i++){
                result[i]=state[i%4][i/4];
            }

            if(begin<num){
                for(int i=0;i<16;i+=1){
                    fwrite(&result[i],sizeof(uint8_t),1,stdout);
                }
            }else{
                int a=16-int(result[15]);
                for(int i=0;i<a;i++){
                    fwrite(&result[i],sizeof(uint8_t),1,stdout);
                }
            }
        }
    }
    return 0;
}