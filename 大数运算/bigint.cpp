#include<iostream>
#include<vector>
#include<chrono>
#include<math.h>
using namespace std;

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

uint64_t vec[128] = {0};

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

    BigInt(uint8_t num[256])
    {
        size = 0;
        for(int i = 255; i >= 0; i++)
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

    void print()
    {
        BigInt ten("10");
        BigInt ling("0");
        BigInt tmp;
        vector<uint32_t> res;
        BigInt m;
        m.size = size;
        for (size_t i = 0; i < size; i++)
        {
            m.opt[i] = opt[i];
        }

        while (judge(m, ling) == 1)
        {
            tmp = m % ten;
            res.push_back(tmp.opt[0]);
            m = m / ten;
        }

        if (res.size() == 0)
        {
            res.push_back(0);
        }

        for (int i = res.size() - 1; i >= 0; i--)
        {
            printf("%d", res[i]);
        }
        printf("\n");
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
        
        for (size_t i = 0; i < size; i++)
        {
            for (size_t j = 0; j < other.size; j++)
            {
                vec[i + j] += (uint64_t)opt[i] * (uint64_t)other.opt[j];
                if (vec[i+j] >= barrier)
                {
                    vec[i + j + 1] += (vec[i + j] / barrier);
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
            vec[i] = 0;
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

    // Fermat
    BigInt inverse2(const BigInt &p)
    {
        BigInt res;
        res.size = size;
        for (size_t i = 0; i < size; i++)
        {
            res.opt[i] = opt[i];
        }
        BigInt b = p;
        BigInt two(2);
        b = b - two;
        res = res.e(b, p);

        return res;
    }

};

int main()
{
    int iter;
    cin >> iter;
    string MOD;
    cin >> MOD;
    BigInt p(MOD);
    while (iter--)
    {
        string num1;
        cin >> num1;
        string num2;
        cin >> num2;
        BigInt a(num1);
        // a.print();
        BigInt b(num2);
        // b.print();
        BigInt result;
        // 加法
        result = (a + b) % p;
        result.print();

        // 减法
        result = (a + p - b) % p;
        result.print();

        // 乘法
        result = (a * b) % p;
        result.print();
        
        // 求逆
        result = a.inverse(p);
        result.print();

        // 幂
        result = a.e(b, p);
        result.print();
    }
}