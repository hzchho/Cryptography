#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<algorithm>
#include<cmath>
using namespace std;
//�α��е���ĸƵ�� 
double freq_table[26]={0.082,0.015,0.028,0.043,0.127,0.022,0.020,0.061,0.070,0.002,0.008,0.040,0.024,0.067,
					0.075,0.019,0.001,0.060,0.063,0.091,0.028,0.010,0.023,0.001,0.020,0.001};
//����ĳ���Ӿ���غ�ָ��					
double calculate_fre(string clause){
	int n=clause.length();
	if(n==1 || n==0){
		return 0.0;
	}
	//����Ӿ����Ƶ 
	double fre[26]={0};
	for(int i=0;i<n;i++){
		fre[clause[i]-'a']+=1;
	}
	double chonghezhishu=0.0;
	//���ݹ�ʽ���� 
	for(int i=0;i<26;i++){
		chonghezhishu+=(fre[i]*(fre[i]-1));
	}
	return chonghezhishu/(n*(n-1));
}
//����������������� 
int gcd(int a,int b){
    return b==0?a:gcd(b,a % b);
}
//�������������� 
int Arr_gcd(vector<int> a){
	int num=a[0];
	for(long unsigned int i=1;i<a.size();i++){
		num=gcd(num,a[i]);
	}
	return num;
}
//������Կ�ĳ��� 
int find_key_length(string miwen,int limit){
	int len=miwen.length();
	map<int,double> zhishu;
	//���Կ��ܵ���Կ 
	for(int m=1;m<limit;m++){
		double p[m];
		//�����İ���Կ���ȷ��� 
		for(int i=0;i<m;i++){
			string sub_str="";
			for(int j=i;j<len;j=j+m){
				sub_str+=miwen[j];
			}
			p[i]=calculate_fre(sub_str);
		}
		double sum=0.0;
		//����ƽ���غ�ָ�� 
		for(int k=0;k<m;k++){
			sum+=p[k];
		}
		zhishu[m]=sum/m;
	}
	//�ҳ��غ�ָ�����ϣ�0.055��0.075����Χ�Ķ�Ӧ��Կ���� 
	vector<int> list;
	for(auto i=zhishu.begin();i!=zhishu.end();i++){
		if(i->second>0.055 && i->second<0.075){
			list.push_back(i->first);
		}
	}
	//�����ε���Կ���ȳ���1����ȡ������� 
	if(list.size()==1){
		return list[0];
	}else{
		return Arr_gcd(list);
	}
} 
//�ڲ�����Կ�ľ������ʱ������λ����Ӿ���غ�ָ�� 
double calculate_fre2(string clause){
	int n=clause.length();
	if(n==1 || n==0){
		return 0.0;
	}
	
	int fre[26]={0};
	for(int i=0;i<n;i++){
		fre[clause[i]-'a']+=1;
	}
	double chonghezhishu=0.0;
	for(int i=0;i<26;i++){
		chonghezhishu+=(freq_table[i]*fre[i]*1.0/n);
	}
	return chonghezhishu;
}
//������Կ��� 
string analyse(string miwen,int key_len){
	string *group=new string[key_len];
	int len=miwen.length();
	if(len==0){
		return "";
	}
	//����Կ���ȷ��� 
	for(int i=0;i<len;i++){
		group[i%key_len]+=miwen[i];
	}
	//��ÿ�鵥������ 
	string key="";
	for(int i=0;i<key_len;i++){
		int alpha='a';
		double min_freq=1.0;
		//������ԿΪa-zʱ���غ�ָ�� 
		for(int j='a';j<='z';j++){
			string test_str="";
			for(long unsigned int k=0;k<group[i].length();k++){
				//��������ĸ��Ӧ��������ĸ 
				test_str+=char((group[i][k]-'a'+26-(j-'a'))%26+'a');
			}
			double num=calculate_fre2(test_str);
			//ȡ�غϹ�ϵ��0.065��ӽ�����ĸ��Ϊ��λ����Կ 
			if(abs(num-0.065)<min_freq){
				min_freq=abs(num-0.065);
				alpha=j;
			}
		}
		key+=char(alpha);
	}
	delete []group;
	return key;
}
//������Կ�����ĸ�ԭ 
string solve(string miwen,string key){
	int len=key.length();
	int index=0;
	string mingwen="";
	for(long unsigned int i=0;i<miwen.length();i++){
		char c;
		if(isalpha(miwen[i])){
			//��Сд���ı� 
			if(miwen[i]>='a' && miwen[i]<='z'){
				c=(miwen[i]-'a'+26-(key[index]-'a'))%26+'a';
			}else{
				c=(miwen[i]-'A'+26-(key[index]-'a'))%26+'A';
			}
			index=(index+1)%len;
		}else{
			c=miwen[i];
		}
		mingwen+=char(c);
	}
	return mingwen;
}

int main(){
	string miwen,input;
	string testcode="";
	while(cin >> input){
		miwen+=input+' ';
	}
//    getline(cin,miwen); 
	for(long unsigned int i=0;i<miwen.length();i++){
		if(isalpha(miwen[i])){
			testcode+=tolower(miwen[i]);
		}
	}

	int key_len=find_key_length(testcode,miwen.length()/50);
	string key=analyse(testcode,key_len);
	string mingwen=solve(miwen,key);
	//����Կת���ɴ�д��� 
	for(long unsigned int i=0;i<key.length();i++){
		key[i]=toupper(key[i]);
	}
	
	cout << key << endl;
	cout << mingwen << endl; 
}

