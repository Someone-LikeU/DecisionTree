
#ifndef DecisionTree_H
#define DecisionTree_H
#define MAXN 100
#include <cstdlib>
#include <map>
using namespace std;
//����������һ����������ȡ�����ͬ��ֵ����˾������ʺ��ú����������洢�������㷨ҲҪ���ں�������
typedef struct TreeData
{
	char Feature[ 10 ];//��������ַ���
	char label; // ����ǩ����Y����N������ʼΪ��#��
}DSTdata;
typedef struct Child *pChild;
struct Child
{
	int EdgeNum; //��������ӽ��ıߵ�����������һ�������Ŀ���ȡֵ
	int ChildIndex; //�ӽ�������
	pChild next; //���ӽ������ָ��
};
typedef struct DSTChildLink
{
	DSTdata Data; //һ�������
	pChild Head; //���������ͷ���
}DSTnode;

struct TreeList		  //���������ʾ��������
{
	DSTnode *pArr = new DSTnode[MAXN];
	int size;
};
void Init( TreeList &Tree );//���ĳ�ʼ��
int getfeatureNum( int **X_train, int train_size, int FeaNum );	 //��ȡ��������ȡֵ�ĸ���
//����Ԥ����
void getTrainSetSize( int &sampleSize, int &featureSize,  FILE *&fp); //��ȡѵ�������
bool getTrainData( int ** &X_train, int y_train[ ], int &train_size,
				   int feaNum, map<string, int> * &dicts, FILE *&fp );  //��ȡѵ������
void getTestSetSize( int &testSize, FILE *&fp ); //��ȡ���Լ����
bool getFeatureSet( char ** &Feature, int &FeaNum, FILE * &fp ); //��ȡ��������
bool getDicts( int feaNum, map<string, int> * &dicts, int &train_size, FILE * &fp );//����һ�������ֵ�
//ѵ����Ԥ��
void fit( int **X_train, int *y_train, int train_size, int *tindices, int &subtrainsize,
		  char **featureSet, int FeaNum, TreeList &Tree, int curNode, int &totalNode );//ѵ����������
void predict( int &test_size, int feaNum, map<string, int> * &dicts,
						TreeList &Tree, FILE *&fp, char **featureSet );  //�������ݷ���
void showTree( TreeList &Tree ); //��ӡ����
void storeTree( TreeList &Tree); //�������ļ���ʽ�浽���
#endif
