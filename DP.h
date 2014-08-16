#pragma once
#include <vector>
#include <map>
#include <string>
#define DATALENGTHMAX 50

using namespace std;

template <typename T> class DP{
public:
	void init(double (* func)(T t1,T t2),double);//データベースを初期化
	double dp(vector<T> d1, vector<T> d2);//マッチングを行う
	double dp(T* a,T* b,int al,int bl);
	map<double,string> matchN(vector<T> t,int n,int dataTag=-1);//データベースとマッチングを行い、上位n件をmapで返す
	map<double,string> matchN(vector<T> t,int n,vector<string> checkList,int dataTag=-1);
	string match(vector<T> t,int dataTag=-1,double* sim=NULL);
	string match(vector<T> data,vector<string> checkList,int dataTag = -1);
	void setData(vector<T> data,string dataName,int dataTag=-1);//データを追加する
	double (*localLength)(T t1, T t2);//ローカルな比較関数
	vector<vector<T>*> database;//データベース
	vector<int> dataLength;
	void clear();
	int numData;
	int lastMatched;
	double diagonalCoeff;
	
	vector<string> name;//データ名
	vector<int> tag;
};

template <typename T>
void DP<T>::clear(){
	numData=0;
	database.clear();
	name.clear();
	tag.clear();
}

template <typename T> 
void DP<T>::init(double (*func)(T t1,T t2),double diagonalCo){
	localLength = func;
	diagonalCoeff=diagonalCo;
	numData=0;
}

template<typename T>
void DP<T>::setData(vector<T> t,string dataName,int dataTag){
	
	vector<T> *t2= new vector<T>;
	t2->insert(t2->begin(),t.begin(),t.end());
	database.push_back(t2);
	dataLength.push_back(t.size());
	name.push_back(dataName);
	tag.push_back(dataTag);
	numData++;
}

template<typename T>
double DP<T>::dp(vector<T> d1,vector<T> d2){

	double dpMap[DATALENGTHMAX][DATALENGTHMAX];
	int i,j;
	int s1=d1.size();
	int s2=d2.size();
	if(s1==0 || s2==0){
		return 10000;
	}
	dpMap[0][0]=2*localLength(d1[0],d2[0]);


	for(j=1;j<s2;j++){
		dpMap[0][j]=dpMap[0][j-1]+localLength(d1[0],d2[j]);
	}

	for(i=1;i<s1;i++){
		dpMap[i][0]=dpMap[i-1][0]+localLength(d1[i],d2[0]);
		for(j=1;j<s2;j++){
			double k;
			if(dpMap[i-1][j]<dpMap[i][j-1]){
				k=dpMap[i-1][j];
			}
			else{
				k=dpMap[i][j-1];
			}
			if(k<localLength(d1[i],d2[j])+dpMap[i-1][j-1]){
				dpMap[i][j]=k+localLength(d1[i],d2[j]);
			}
			else{
				dpMap[i][j]=dpMap[i-1][j-1]+diagonalCoeff*localLength(d1[i],d2[j]);
			}
		}
	}

	return dpMap[s1-1][s2-1]/(s1+s2);
}

template<typename T>
string DP<T>::match(vector<T> data,int dataTag,double* sim){
	int min=100000;
	int argMin=0;
	int m=dataTag;
	for(int i=0;i<database.size();i++){
		if(dataTag!=tag[i])
			continue;
		double distance = dp(data,*database[i]);
		if(distance<min){
			min=distance;
			argMin=i;
		}
	}
	if(sim!=NULL){
		*sim=min;
	}
	lastMatched=argMin;
	return name[argMin];
}

template<typename T>
string DP<T>::match(vector<T> data,vector<string> checkList,int dataTag){
	int min=100000;
	int argMin=0;
	int m=dataTag;
	for(int i=0;i<database.size();i++){
		if(dataTag!=tag[i])
			continue;
		vector<string>::iterator itr;
		itr=find(checkList.begin(),checkList.end(),name[i]);
		if(itr==checkList.end()) 
			continue;
		double distance = dp(data,*(database[i]));
		if(distance<min){
			min=distance;
			argMin=i;
		}
	}
	lastMatched=argMin;
	return name[argMin];

}

template<typename T>
map<double,string> DP<T>::matchN(vector<T> data,int n,int dataTag){
	
	map<double,string> result;
	for(int i=0;i<database.size();i++){
		if(dataTag!=tag[i]) continue;

		double distance = dp(data,*(database[i]));
		map<double,string>::iterator itrMap = result.begin();
		while(true){
			if(itrMap == result.end() || itrMap->second == name[i] ) break;
			itrMap++;
		}
	
		if(itrMap!=result.end()){
			if(itrMap->first > distance){
				result.erase(itrMap);
				result.insert(make_pair(distance,name[i]));
			}
		}
		else{
			result.insert(make_pair(distance,name[i]));
			
			if(result.size()>n){
				map<double,string>::iterator end = result.end();
				end--;
				result.erase(end);
			}
			
		}
	}
	return result;
}

template<typename T>
map<double,string> DP<T>::matchN(vector<T> data,int n,vector<string> checkList,int dataTag){
	
	map<double,string> result;
	for(int i=0;i<database.size();i++){
		if(dataTag!=tag[i]) continue;
				
		vector<string>::iterator itr;
		itr=find(checkList.begin(),checkList.end(),name[i]);
		if(itr==checkList.end()) 
			continue;
		
		double distance = dp(data,*(database[i]));
		map<double,string>::iterator itrMap = result.begin();
		while(true){
			if(itrMap == result.end() || itrMap->second == name[i] ) break;
			itrMap++;
		}
		if(itrMap!=result.end()){
			if(itrMap->first > distance){
				result.erase(itrMap);
				result.insert(make_pair(distance,name[i]));
			}
		}
		else{
			result.insert(make_pair(distance,name[i]));
			
			if(result.size()>n){
				map<double,string>::iterator end = result.end();
				end--;
				result.erase(end);
			}
			
		}
	}
	return result;
}
