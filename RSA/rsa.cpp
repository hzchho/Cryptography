#include<iostream>
#include<cstdint>
#include<bitset>
#include<vector>
#include<algorithm>
#include<fstream>
#include<random>
#include<cstring>
using namespace std;

#ifdef _WIN32
#include<fcntl.h>
#include<io.h>
#endif

/* ************************* SHA部分 ********************* */
// 将轮换函数改编成宏定义
#define rightRotate(x, n) ((x >> n) | (x << (32 - n)))

// 初始哈希值
uint32_t h1=0x6a09e667;
uint32_t h2=0xbb67ae85;
uint32_t h3=0x3c6ef372;
uint32_t h4=0xa54ff53a;
uint32_t h5=0x510e527f;
uint32_t h6=0x9b05688c;
uint32_t h7=0x1f83d9ab;
uint32_t h8=0x5be0cd19;
const uint32_t h[8]={0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
            0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

// 常数表
const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// 将32位的数分解成4个8位的数
void uint32_to_4uint8(uint32_t value, uint8_t* x){
    x[0]=(value>>24)&0xff;
    x[1]=(value>>16)&0xff;
    x[2]=(value>>8)&0xff;
    x[3]=value&0xff;
}

vector<uint8_t> sha(vector<uint8_t> mingwen){
    int len=mingwen.size();
    mingwen.push_back(0x80);
    uint32_t res[8];
    memcpy(res, h, sizeof(uint32_t)*8);
    uint64_t length=uint64_t(len*8);

    int need;
    if(((len+1)*8)%512<=448){
        need=(448-((len+1)*8)%512)/8;
    }else{
        need=(960-((len+1)*8)%512)/8;
    }

    while(need--){
        mingwen.push_back(0x00);
    }

    for(int i=0;i<8;i++){
        mingwen.push_back((uint8_t)(length>>(56-8*i)));
    }
    
    int iteration=mingwen.size()/64;
    int iter=0;
    while(iter<iteration){
        uint32_t w[64]={0};
        for(int i=0;i<16;i++){
            for(int j=0;j<4;j++){
                w[i]+=((uint32_t)mingwen[iter*64+i*4+j])<<(24-8*j);
            }
        }
        
        for(int i=16;i<64;i++){
            uint32_t s0=rightRotate(w[i-15],7)^rightRotate(w[i-15],18)^(w[i-15]>>3);
            uint32_t s1=rightRotate(w[i-2],17)^rightRotate(w[i-2],19)^(w[i-2]>>10);
            w[i]=w[i-16]+s0+w[i-7]+s1;
        }

        uint32_t a=res[0],b=res[1],c=res[2],d=res[3],e=res[4],f=res[5],g=res[6],h=res[7];
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

        res[0]+=a;
        res[1]+=b;
        res[2]+=c;
        res[3]+=d;
        res[4]+=e;
        res[5]+=f;
        res[6]+=g;
        res[7]+=h;

        iter++;
    }

    vector<uint8_t> result;
    uint8_t tmp_[4];
    uint32_to_4uint8(res[0],tmp_);
    result.insert(result.end(),tmp_,tmp_+4);
    uint32_to_4uint8(res[1],tmp_);
    result.insert(result.end(),tmp_,tmp_+4);
    uint32_to_4uint8(res[2],tmp_);
    result.insert(result.end(),tmp_,tmp_+4);
    uint32_to_4uint8(res[3],tmp_);
    result.insert(result.end(),tmp_,tmp_+4);
    uint32_to_4uint8(res[4],tmp_);
    result.insert(result.end(),tmp_,tmp_+4);
    uint32_to_4uint8(res[5],tmp_);
    result.insert(result.end(),tmp_,tmp_+4);
    uint32_to_4uint8(res[6],tmp_);
    result.insert(result.end(),tmp_,tmp_+4);
    uint32_to_4uint8(res[7],tmp_);
    result.insert(result.end(),tmp_,tmp_+4);

    return result;
}

// 生成seed
vector<uint8_t> getRandomSeed(int size){
    vector<uint8_t> seed(size);
    ifstream urandom("/dev/urandom", ios::in | ios::binary);

    urandom.read(reinterpret_cast<char*>(seed.data()), size);

    return seed;
}

vector<uint8_t> extendbyte(vector<uint8_t> mingwen, uint32_t count){
    vector<uint8_t> res=mingwen;
    for(int i=0;i<4;i++){
        res.push_back((uint8_t)(count>>(24-i*8)));
    }
    return res;

}

vector<uint8_t> mgf(vector<uint8_t> mingwen, uint8_t len){
    uint32_t count=0;
    uint16_t length=0;
    vector<uint8_t> res;
    while(length<len){
        vector<uint8_t> fill=sha(extendbyte(mingwen, count));
        length+=32;
        count+=1;
        res.insert(res.end(), fill.begin(), fill.end());
    }

    while(length-len){
        res.pop_back();
        length--;
    }

    return res;
}

const uint64_t barrier = 4294967296;
const string barrier_ = "4294967296";

uint32_t pow2[32] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192,
                     16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152,
                     4194304, 8388608, 16777216, 33554432, 67108864, 134217728,
                     268435456, 536870912, 1073741824, 2147483648};

string de_to_he(string dec){
    string num = dec;
    string result = "";
    string alpha = "0123456789ABCDEF";
    while(num != "0")
    {
        int r = 0;
        string q = "";
        // 逐位除以16
        for(size_t i = 0; i < num.length(); i++)
        {
            // 把上一位除完的余数加上下一位(模拟除法过程)
            int cur = r * 10 + (num[i] - '0');
            // 商依次往后补
            q += (cur / 16) + '0';
            // 模拟除法中的减法
            r = cur % 16;
        }

        // 去除前导零
        size_t pos = q.find_first_not_of('0');
        if(pos != string::npos){
            q = q.substr(pos);
        }
        else
        {
            q = "0";
        }
        
        // 将余数转化成十六进制字符前插
        result = alpha[r] + result;
        num = q;
    }

    return result.empty() ? "0" : result;
}

uint32_t change_hex(char a){
    uint32_t x = 0;
    if(a <= '9')
    {
        x = a - '0';
    }
    else
    {
        x = a - 'A' + 10;
    }
    return x;
}

class BigInt
{
    public:
    uint32_t opt[128];
    size_t size = 0;

    BigInt()
    {
        size = 0;
        for (size_t i = 0; i < 128 ; i++)
        {
            opt[i] = 0;
        }
    }

    BigInt(uint64_t num)
    {
        size = 0;
        for (size_t i = 0; i < 128 ; i++)
        {
            opt[i] = 0;
        }
        uint32_t num1 = (uint32_t)num;
        opt[size++] = num1;
        uint32_t tmp = num >> 32;
        if(tmp != 0)
        {
            opt[size++] = tmp;
        }
    }

    BigInt(vector<uint8_t> num)
    {
        size = 0;
        for(int i = 255; i >= 0; i-=4)
        {
            uint32_t value = (uint32_t)num[i-3]<<24 | (uint32_t)num[i-2]<<16 | (uint32_t)num[i-1]<<8 | (uint32_t)num[i];
            opt[size++] = value;
        }
    }

    BigInt(string num)
    {
        size = 0;
        for (size_t i = 0; i < 128 ; i++)
        {
            opt[i] = 0;
        }
        string hex = de_to_he(num);
        int len = hex.length();

        int pos = len;
        while (pos > 0)
        {
            int start = (pos >= 8) ? pos - 8 : 0;
            string hexpart = hex.substr(start, pos - start);

            while (hexpart.length() < 8)
            {
                hexpart = '0' + hexpart;
            }

            uint32_t value = stoul(hexpart, nullptr, 16);
            opt[size++] = value;

            pos -= 8;
        }
    }
    
    void out()
    {
        for (int i = size -1; i >= 0; i--)
        {
            printf("%u ", opt[i]);
        }
        printf("\n");
    }

    void print(){
        for(int i = size - 1; i >= 0; i--)
        {
            for(size_t j = 0; j < 4; j++)
            {
                uint8_t cur = (uint8_t)(opt[i] >> (24 - 8 * j));
                fwrite(&cur, sizeof(uint8_t), 1, stdout);
            } 
        }
    }

    // Add
    BigInt operator+(const BigInt &other)
    {
        BigInt result;
        size_t i = 0;
        size_t j = 0;
        uint64_t carry = 0;

        while(i < size || j < other.size)
        {
            uint64_t sum = carry;
            if(i < size)
            {
                sum += opt[i++];
            }
            if(j < other.size)
            {
                sum += other.opt[j++];
            }
            carry = sum / barrier;
            result.opt[result.size++] = (uint32_t)(sum % barrier);
        }

        if (carry != 0)
        {
            result.opt[result.size++] = (uint32_t)carry;
        }

        return result;
    }

    // Sub
    BigInt operator-(const BigInt &other)
    {
        BigInt result;
        long long borrow = 0;
        size_t i = 0;
        size_t j = 0;

        while(i < size || j < other.size)
        {
            long long diff = borrow;
            if(i < size)
            {
                diff += opt[i++];
            }
            if(j < other.size)
            {
                diff -= other.opt[j++];
            }

            if(diff < 0)
            {
                borrow = -1;
                diff += barrier;
            }
            else
            {
                borrow = 0;
            }

            result.opt[result.size++] = (uint32_t)diff;
        }
        // 删除前导0
        while(result.size > 1 && result.opt[result.size-1] == 0)
        {
            result.size -= 1;
        }

        return result;
    }

    // Mul
    BigInt operator*(const BigInt &other)
    {
        BigInt result;
        size_t v_size = size + other.size;
        uint64_t vec[v_size] = {0};
        
        for (size_t i = 0; i < size; i++)
        {
            for (size_t j = 0; j < other.size; j++)
            {
                vec[i + j] += (uint64_t)opt[i] * (uint64_t)other.opt[j];
                if (vec[i+j] >= barrier)
                {
                    vec[i + j + 1] += (vec[i+j] / barrier);
                    vec[i + j] = vec[i + j] % barrier;
                }
            }
        }

        while (v_size > 1 && vec[v_size - 1] == 0)
        {
            v_size -= 1;
        }

        result.size = v_size;
        for (size_t i = 0; i < result.size; i++)
        {
            result.opt[i] = (uint32_t)vec[i];
        }

        return result;
    }

    // compare
    int judge(const BigInt &a, const BigInt &b)
    {
        // 小于
        if(a.size < b.size)
        {
            return 0;
        }// 大于
        else if(a.size > b.size)
        {
            return 1;
        }

        for(int i = a.size - 1; i >= 0; i--)
        {
            if(a.opt[i] > b.opt[i])
            {
                return 1;
            }
            else if(a.opt[i] < b.opt[i])
            {
                return 0;
            }
            else
            {
                continue;
            }
        }
        // 等于
        return 2;    
    }
    
    // left shift
    BigInt operator<<(int num)
    {
        BigInt res;
        res.size = size + num;
        for (int i = res.size - 1; i >= num; i--)
        {
            res.opt[i] = opt[i - num];
        }

        return res;
    }

    // mod
    BigInt operator%(const BigInt& p)
    {
        BigInt result;
        result.size = size;
        for(size_t i = 0; i < size; i++)
        {
            result.opt[i] = opt[i];
        }

        while (judge(result, p))
        {
            size_t big = result.size;
            size_t small = p.size;
            if(big == small)
            {
                uint64_t q = 1;
                uint32_t q1 = result.opt[big - 1];
                uint32_t q2 = p.opt[small - 1];
                while(q1 > q2)
                {
                    q1 /= 2;
                    if (q1 > q2)
                    {
                        q *= 2;
                    }
                }
                BigInt quo(q);
                BigInt need = quo * p;
                while (judge(result, need))
                {
                    result = result - need;
                }
            }
            else
            {
                if (result.opt[big - 1] > p.opt[small - 1])
                {
                    uint32_t q1 = result.opt[big - 1];
                    uint32_t q2 = p.opt[small - 1];
                    uint64_t q = 1;
                    while (q1 > q2)
                    {
                        q1 /= 2;
                        if (q1 > q2)
                        {
                            q *= 2;
                        }
                    }
                    BigInt quo(q);
                    BigInt need("1");
                    need = need << (big-small);
                    need = need * p;
                    need = need * quo;
                    while (judge(result, need))
                    {
                        result = result - need;
                    }
                }
                else
                {
                    uint64_t q1 = result.opt[big - 1] * barrier + result.opt[big - 2];
                    uint32_t q2 = p.opt[small - 1];
                    uint64_t q = 1;
                    while (q1 > q2)
                    {
                        q1 /= 2;
                        if (q1 > q2)
                        {
                            q *= 2;
                        }
                    }
                    BigInt need("1");
                    BigInt quo(q);
                    need = need << (big - small -1);
                    need = need * p;
                    need = need *quo;
                    while (judge(result, need))
                    {
                        result = result - need;
                    }
                }
            }
        }

        return result;
    }

    // div
    BigInt operator/(const BigInt& p)
    {
        BigInt result;
        BigInt ans("0");
        BigInt one("1");
        result.size = size;
        for(size_t i = 0; i < size; i++)
        {
            result.opt[i] = opt[i];
        }

        while (judge(result, p))
        {
            size_t big = result.size;
            size_t small = p.size;
            if(big == small)
            {
                uint64_t q = 1;
                uint32_t q1 = result.opt[big - 1];
                uint32_t q2 = p.opt[small - 1];
                while(q1 > q2)
                {
                    q1 /= 2;
                    if (q1 > q2)
                    {
                        q *= 2;
                    }
                }
                BigInt quo(q);
                BigInt need = quo * p;
                while (judge(result, need))
                {
                    result = result - need;
                    ans = ans + quo; 
                }
            }
            else
            {
                if (result.opt[big - 1] > p.opt[small - 1])
                {
                    uint32_t q1 = result.opt[big - 1];
                    uint32_t q2 = p.opt[small - 1];
                    uint64_t q = 1;
                    while (q1 > q2)
                    {
                        q1 /= 2;
                        if (q1 > q2)
                        {
                            q *= 2;
                        }
                    }
                    BigInt quo(q);
                    BigInt need("1");
                    need = need << (big-small);
                    BigInt add = need * quo;
                    need = need * p;
                    need = need * quo;
                    while (judge(result, need))
                    {
                        result = result - need;
                        ans = ans + add;
                    }
                }
                else
                {
                    uint64_t q1 = result.opt[big - 1] * barrier + result.opt[big - 2];
                    uint32_t q2 = p.opt[small - 1];
                    uint64_t q = 1;
                    while (q1 > q2)
                    {
                        q1 /= 2;
                        if (q1 > q2)
                        {
                            q *= 2;
                        }
                    }
                    BigInt need("1");
                    BigInt quo(q);
                    need = need << (big - small -1);
                    BigInt add = quo * need;
                    need = need * p;
                    need = need * quo;
                    while (judge(result, need))
                    {
                        result = result - need;
                        ans = ans + add;
                    }
                }
            }
        }

        return ans;
    }
    
    // 判断是否为0
    bool iszero(){
        return size == 1 && opt[0] == 0; 
    }

    // 快速幂
    BigInt e(const BigInt &exp, const BigInt &p)
    {
        BigInt base;
        base.size = size;
        for (size_t i = 0; i < size; i++)
        {
            base.opt[i] = opt[i];
        }
        BigInt x = exp;
        BigInt res("1");
        for (size_t i = 0; i < x.size; i++)
        {
            for(size_t j = 0; j < 32; j++)
            {
                if (x.opt[i] & pow2[j])
                {
                    res = (res * base) % p;
                }
                base = (base * base) % p;
            }
        }
        
        return res;
    }
    
    // Euclid
    BigInt inverse(const BigInt &p)
    {
        BigInt a = p, mod = p;
        mod = mod * mod;
        BigInt m;
        m.size = size;
        for (size_t i = 0; i < size; i++)
        {
            m.opt[i] = opt[i];
        }

        BigInt v0(1), w0("0"), v1("0"), w1(1);
        BigInt ling("0");

        while (1)
        {
            BigInt quo = a / m;
            BigInt tmp = a % m;

            a = m;
            m = tmp;

            if (judge(m, ling) == 2)
            {
                break;
            }

            BigInt tmp_v = v0;
            BigInt tmp_w = w0;

            v0 = v1;
            w0 = w1;

            v1 = (tmp_v + mod - (quo * v1)) % p;
            w1 = (tmp_w + mod - (quo * w1)) % p;
        } 

        return w1;
    }

};


int main(){
#ifdef _WIN32
    setmode(fileno(stdin), O_BINARY);
    setmode(fileno(stdout), O_BINARY);
#endif
    // 读入数据
    // FILE *inputFile = fopen("1.bin", "rb");
    vector<uint8_t> seed=getRandomSeed(32);
    // vector<uint8_t> seed={0x84, 0xEE, 0x1D, 0x92, 0xBE, 0xFC, 0x55, 0x96, 0x6F, 0x20, 0xB6, 0xFD, 0x18, 0xFC, 0x45, 0x20,
                    // 0xCF, 0x17, 0x0B, 0xD8, 0x92, 0xE2, 0xE0, 0xD3, 0x4F, 0xE7, 0xA0, 0x10, 0x17, 0xC8, 0x6B, 0x67};
    
    int op1=16,op2=256,op3=256,op4=256;
    uint8_t hlen=32;
    uint8_t _;
    while(op1){
        op1--;
        fread(&_, sizeof(uint8_t), 1, stdin);
    }
    
    vector<uint8_t> n(256);
    for(int i=0;i<op2;i++){
        fread(&n[i], sizeof(uint8_t), 1, stdin);
    }
    vector<uint8_t> e(256);
    for(int i=0;i<op3;i++){
        fread(&e[i], sizeof(uint8_t), 1, stdin);
    }
    while(op4){
        op4--;
        fread(&_, sizeof(uint8_t), 1, stdin);
    }
    uint8_t mlen;
    fread(&mlen, sizeof(uint8_t), 1, stdin);
    uint8_t mingwen[256];
    for(int i=0;i<mlen;i++){
        fread(&mingwen[i], sizeof(uint8_t), 1, stdin);
    }
    uint8_t dblen=223;
    uint8_t pslen=256-hlen*2-mlen-2;
    uint8_t ps[pslen]={0};
    // 开始处理
    // 组合DB
    vector<uint8_t> L(0);
    L=sha(L);
    vector<uint8_t> db;
    for(int i=0;i<32;i++){
        db.push_back(L[i]);
    }
    for(int i=0;i<pslen;i++){
        db.push_back(ps[i]);
    }
    db.push_back(0x01);
    for(int i=0;i<mlen;i++){
        db.push_back(mingwen[i]);
    }
    

    vector<uint8_t> dbmask=mgf(seed, dblen);
    
    for(int i=0;i<dblen;i++){
        dbmask[i]=dbmask[i]^db[i];
    }
    
    vector<uint8_t> seekmask=mgf(dbmask, hlen);


    for(int i=0;i<hlen;i++){
        seekmask[i]^=seed[i];
    }

    vector<uint8_t> em;
    em.push_back(0x00);
    for(int i=0;i<hlen;i++){
        em.push_back(seekmask[i]);
    }
    for(int i=0;i<dblen;i++){
        em.push_back(dbmask[i]);
    }

    
    BigInt res(em);
    
    BigInt ee(e);
    
    BigInt p(n);

    res=res.e(ee, p);
    res.print();

    return 0;
}