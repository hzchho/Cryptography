#include<iostream>
#include<string>
#include<vector>
#include<random>
#include<algorithm>
using namespace std;
//ģ26�˷� 
int Mod_x(int a,int b){
	return (a*b)%26;
}
//����������Ԫģ26���� 
int findinv(int a){
	for(int i=1;i<26;i++){
		if(a*i%26==1){
			return i;
		}
	}
	return -1;
} 
//��˹��Ԫ���������� 
bool inverse(vector<vector<int>> A,vector<vector<int>> &A_,int m){
	//����һ��������� 
	vector<vector<int>> T(m,vector<int>(2*m));
	for(int i=0;i<m;i++){
		for(int j=0;j<m;j++){
			//��벿��Ϊ A�����Ұ벿��Ϊ��λ���� 
			T[i][j]=A[i][j];
			if(i==j){
				T[i][j+m]=1;
			}else{
				T[i][j+m]=0;
			}
		}
	}
	//Gauss elimination
	for(int i=0;i<m;i++){
		//����ԪΪ0����ģ26�治����ʱ���� 
		if(T[i][i]==0){
			return false;
		}
		int v=findinv(T[i][i]);
		if(v==-1){
			return false;
		}
		//����Ԫ������ģ26����Ԫ��ģ26�� 
		for(int j=0;j<2*m;j++){
			T[i][j]=Mod_x(T[i][j],v);
		}
		//���������� 
		for(int k=0;k<m;k++){
			if(k==i){
				continue;
			}
			int w=T[k][i];
			for(int j=i;j<2*m;j++){
				T[k][j]=(T[k][j]-(w*T[i][j])%26+26)%26;
			}
		}
	}
	//��ȡ�Ұ벿�֣�������� 
	for(int i=0;i<m;i++){
		for(int j=0;j<m;j++){
			A_[i][j]=T[i][j+m];
		}
	}
	return true;
}
//����˷� 
void matrix_x(vector<vector<int>> A,vector<vector<int>> B,int **C,int m){
	for(int i=0;i<m;i++){
		for(int j=0;j<m;j++){
			C[i][j]=0;
			for(int l=0;l<m;l++){
				C[i][j]+=A[i][l]*B[l][j];
			}
			C[i][j]=(C[i][j]+26*26*3)%26;
		}
	}
}
//��⺯������ 
void solve(int m,string mingwen,string miwen,int **K,int *B){
	int len=mingwen.length();
	if(len<2*m*m){
		cout << "Can't Solve!" << endl;
		return;
	}
	int matrix1[len/m][m];
	int matrix2[len/m][m];
	int idx1=0;
	int idx2=0;
	//���Ƚ����ġ�����ת���ɶ�Ӧ���֣�������ÿ��m������ 
	for(int i=0;i<2*m*m;i++){
		matrix1[idx1][idx2]=mingwen[i]-'A';
		matrix2[idx1][idx2]=miwen[i]-'A';
		idx2++;
		if(idx2==m){
			idx2=0;
			idx1++;
		}
	}
	//����������ӣ��������������� 
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(0,2*m-1);
	vector<vector<int>> X(m,vector<int>(m));
	vector<vector<int>> Y(m,vector<int>(m));
    vector<int> box(2*m);
    for(int i=0;i<2*m;i++){
    	box[i]=i;
	}
	vector<vector<int>> X_(m,vector<int>(m));
	//���ô�matrix1��matrix2�����ѡȡm*m���󣬲���ȥʣ�ಿ�־��󣬽�����X�������棬ֱ������������
	//ԭ�����ݴ���ϵ����X��K+B=Y������������������Եõ�X'��K=Y',Ϊ��η��̣�������о������� 
	while(1){
		shuffle(box.begin(),box.end(),gen);
		for(int i=0;i<m;i++){
			for(int j=0;j<m;j++){
				X[i][j]=(matrix1[box[i]][j]-matrix1[box[i+m]][j]+26)%26;
				Y[i][j]=(matrix2[box[i]][j]-matrix2[box[i+m]][j]+26)%26;
			}
		}
		bool isok=inverse(X,X_,m);
		if(isok){
			break;
		}
	}
	//����˷�K=X^(-1)��Y 
	matrix_x(X_,Y,K,m);
    //���ط���B=Y-X��K ���B 
	for(int i=0;i<m;i++){
		int tmp=0;
		for(int j=0;j<m;j++){
			tmp+=matrix1[0][j]*K[j][i];
		}
		B[i]=(26*26*3+matrix2[0][i]-tmp)%26;
	}
}

int main(){
	int m;
	string mingwen;
	string miwen;
	cin >> m;
	cin >> mingwen;
	cin >> miwen;

	int** K=new int*[m];
	for(int i=0;i<m;i++){
		K[i]=new int[m];
	}
	int *B=new int[m];
	solve(m,mingwen,miwen,K,B);
	
	for(int i=0;i<m;i++){
		for(int j=0;j<m;j++){
			cout << K[i][j] << " ";
		}
		cout << endl;
	}
	for(int i=0;i<m;i++){
		cout << B[i] << " ";
	}
	for(int i=0;i<m;i++){
		delete []K[i];
	}
	delete []K;
	delete []B;
} 
