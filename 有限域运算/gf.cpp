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

//将输入的两个多项式转化成二进制的形式 
void change_bin(uint64_t a[3][2],vector<vector<int>> &c){
	for(int i=0;i<2;i++){
		for(int j=0;j<3;j++){
			bitset<64> binary(a[j][i]);
			for(size_t k=0;k<64;k++){
				c[i][j*64+k]=binary[k]?1:0;
			}
		}
	}
}
//将结果输出到stdout 
void gf_print(vector<int> a){
#ifdef _WIN32
    setmode(fileno(stdout), O_BINARY);
#endif
	uint64_t result[3]={0,0,0};
	//将二进制结果组合移位得到三个64位结果 
	for(int i=0;i<192;i+=8){
		uint8_t value=0;
		for(int j=0;j<8;j++){
			value=value|(a[i+j]<<j);
		}
		
		result[i/64] |= (static_cast<uint64_t>(value))<< (i%64);
	} 
	for(int i=0;i<3;i++){
		fwrite(&result[i],sizeof(uint64_t),1,stdout);
	} 
}
//多项式取模 
vector<int> gf_mod(vector<int> a){
	//x^131+x^13+x^2+x+1
	int m[5]={0,1,2,13,131};
	vector<int> a_c(261);
	vector<int> b(192);
	int max_n=0;
	//确定最高的次数 
	for(int i=0;i<261;i++){
		if(a[260-i]==1 && 260-i>max_n){
			max_n=260-i;
		}
		a_c[i]=a[i];
	}
	//从最高次数开始除以不可约多项式 
	for(int i=max_n;i>=131;i--){ 
		if(a_c[i]!=0){
			//更新当前最高次 
			max_n=i;
			for(int j=0;j<5;j++){
				if(i-131+m[j]<=max_n){
					//对于模2，+1-1得到结果一致 
					a_c[i-131+m[j]]=(a_c[i-131+m[j]]+1)%2;
				}
			}
		}
	}
	
	for(int i=0;i<192;i++){
		b[i]=a_c[i];
	}	
	return b;
} 
//多项式加法 
void gf_add(vector<vector<int>> a){
	vector<int> result(192);
	for(int i=0;i<192;i++){
		//按位加 
		result[i]=(a[0][i]+a[1][i])%2;
	}
	gf_print(result);
}
//多项式乘法 
void gf_mul(vector<vector<int>> a){
	vector<int> tmp_result(261,0);
	//普通的按位乘法运算 
	for(int i=0;i<131;i++){
		if(a[0][i]!=0){
			for(int j=0;j<131;j++){
				tmp_result[i+j]=(tmp_result[i+j]+a[1][j])%2;
			}
		}
	}
	//取模 
	vector<int> result=gf_mod(tmp_result);
	gf_print(result);
} 
//多项式取平方 
void gf_pow(vector<vector<int>> a){
	vector<int> b(261,0);
	//根据域的特性，比普通乘法快 
	for(int i=0;i<131;i++){
		if(a[0][i]==1){
			b[2*i]=1;
		}
	}
	//取模 
	vector<int> result=gf_mod(b);
	gf_print(result);
}
//以下为扩展欧几里得算法所需的额外函数（与上面函数相比，返回结果都为长度261的数组） 
//计算两个多项式相除的商 
vector<int> get_quotient(vector<int> a, vector<int> b){
	vector<int> a_c(261);
	vector<int> m(261);
	vector<int> ans(261,0);
	int max_a=0;
	int max_b=0;
	//求两个多项式的最高次数 
	for(int i=0;i<261;i++){
		if(a[260-i]==1 && 260-i>max_a){
			max_a=260-i;
		}
		if(b[260-i]==1 && 260-i>max_b){
			max_b=260-i;
		}
		a_c[i]=a[i];
		m[i]=b[i];
	}
	//同取模步骤 
	for(int i=max_a;i>=max_b;i--){ 
		if(a_c[i]!=0){
			//更新当前最高次 
			max_a=i;
			//记录商 
			ans[i-max_b]=1;
			for(int j=0;j<=max_b;j++){
				if(m[j]==1 && i-131+j<=max_a){
					//对于模2，+1-1得到结果一致 
					a_c[i-max_b+j]=(a_c[i-max_b+j]+1)%2;
				}
			}
		}
	}
	
	return ans;
}
//计算两个多项式相除的余数 
vector<int> get_rest(vector<int> a, vector<int> b){
	vector<int> a_c(261);
	vector<int> m(261);
	vector<int> ans(261);
	int max_a=0;
	int max_b=0;
	for(int i=0;i<261;i++){
		if(a[260-i]==1 && 260-i>max_a){
			max_a=260-i;
		}
		if(b[260-i]==1 && 260-i>max_b){
			max_b=260-i;
		}
		a_c[i]=a[i];
		m[i]=b[i];
	}
	//与求商相似，不需要记录商 
	for(int i=max_a;i>=max_b;i--){ 
		if(a_c[i]!=0){
			max_a=i;
			for(int j=0;j<=max_b;j++){
				if(m[j]==1 && i-131+j<=max_a){
					a_c[i-max_b+j]=(a_c[i-max_b+j]+1)%2;
				}
			}
		}
	}
	
	for(int i=0;i<261;i++){
		ans[i]=a_c[i];
	}	
	return ans;
}
//计算两个多项式相减（二进制中加法减法一致） 
vector<int> get_eli(vector<int> a, vector<int> b){
	vector<int> result(261,0);
	for(int i=0;i<131;i++){
		result[i]=(a[i]+b[i])%2;
	}
	return result;
} 
//计算两个多项式的积
vector<int> get_mul(vector<int> a, vector<int> b){
	vector<int> tmp_result(261,0);
	for(int i=0;i<131;i++){
		if(a[i]!=0){
			for(int j=0;j<131;j++){
				tmp_result[i+j]=(tmp_result[i+j]+b[j])%2;
			}
		}
	}
	vector<int> p=gf_mod(tmp_result);
	for(int i=0;i<261;i++){
		if(i>130){
			tmp_result[i]=0;
		}else{
			tmp_result[i]=p[i];
		}
	}
	return tmp_result;
}
//扩展欧几里得算法主函数 
void gf_inv_Euclid(vector<vector<int>> a){
    vector<int> x(261);
	vector<int> y(261,0);
	vector<int> v0(261,0),w0(261,0),v1(261,0),w1(261,0);
	y[0]=1;
	y[1]=1;
	y[2]=1;
	y[13]=1;
	y[131]=1;
	v0[0]=1;
	w1[0]=1;
	for(int i=0;i<131;i++){
		x[i]=a[0][i];
	}	
	//算法同整数的欧几里得算法求乘法逆元原理一致 
	while(1){
        //由于多项式最高次一定小于不可约多项式，因此可以直接将不可约多项式作为被除数 
        //计算商和余数 
		vector<int> q=get_quotient(y,x);
		vector<int> r=get_rest(y,x);
		//辗转相除法迭代 
		y=x;
		x=r;
		//判断除数是否为0 
		if(all_of(x.begin(),x.end(),[](int val){return val==0;})){
            break; // 退出循环
        }	
		//更新v0,v1,w0,w1 
		vector<int> tmp_v=v0;
		vector<int> tmp_w=w0;
		v0=v1;
		w0=w1;
		v1=get_eli(tmp_v,get_mul(q,v1));
		w1=get_eli(tmp_w,get_mul(q,w1));		
	}
	vector<int> result=gf_mod(w1);
	gf_print(result);
}
//以下为费马小定理所需的额外函数 
//快速幂乘法计算a^(2^131-2)=1 mod m 
vector<int> get_pow(vector<int> a, vector<int> n){
	vector<int> tmp_result(261,0);
    tmp_result[0]=1;
    int count=0;
    //直接采用判断二进制位将指数次的复杂度降为线性复杂度 
	while(count<131){
		if(n[count]==1){
			//乘上a 
			tmp_result=get_mul(tmp_result,a);
		}
		//a平方 
		a=get_mul(a,a);
		count++;
	}
	return tmp_result;
}
//费马小定理求逆元主函数 
void gf_inv_Fermat(vector<vector<int>> a){
	vector<int> n(131,1);
	n[0]=0;
	vector<int> a_(261,0);
	for(int i=0;i<131;i++){
		a_[i]=a[0][i];
	}
    vector<int> result=get_pow(a_,n);
    gf_print(gf_mod(result));
}

int main(){
//#ifdef ONLINE_JUDGE
#ifdef _WIN32
    setmode(fileno(stdin), O_BINARY);
    setmode(fileno(stdout), O_BINARY);
#endif
    uint8_t op_num[4];
    //读运算次数 
    for(int i=0;i<4;i++){
    	fread(&op_num[i],sizeof(uint8_t),1,stdin);
    }
    //转化成整数 
    uint32_t value= (op_num[3] << 24) | (op_num[2] << 16) | (op_num[1] << 8) | op_num[0];
    while(value--){
    	//读操作数 
    	uint8_t ope;
    	fread(&ope,sizeof(uint8_t),1,stdin);
		//读数据 
		uint64_t data[3][2];
		for(int i=0;i<2;i++){
			for(int j=0;j<3;j++){
				uint64_t tmp=0;
				for(int k=0;k<8;k++){
					uint8_t b;
					//一次读取8位，通过移位组合成64位 
					fread(&b,sizeof(uint8_t),1, stdin);
					tmp= tmp | static_cast<uint64_t>(b) << k*8;		
				}
				data[j][i]=tmp;
			}
		}
		//将64位数转化成二进制(从0-192表示从低次到高次)
		vector<vector<int>> opn(2,vector<int>(192));
		change_bin(data,opn); 
		//0.+ 1.x 2.^ 3.- 
		if(ope==0){
			gf_add(opn);
		}else if(ope==1){
			gf_mul(opn);
		}else if(ope==2){
			gf_pow(opn);
		}else if(ope==3){
			int cho=0;
			if(cho){
				//cho为1时使用扩展欧几里得算法求逆元 
				gf_inv_Euclid(opn);
			}else{
				//cho为0时使用费马小定理求逆元 
				gf_inv_Fermat(opn);
			}
		}
	}
    return 0;
}
