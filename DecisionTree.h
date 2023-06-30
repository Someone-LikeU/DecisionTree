
#ifndef DecisionTree_H
#define DecisionTree_H
#define MAXN 100
#include <cstdlib>
#include <map>
using namespace std;
//由于样本中一个特征可能取多个不同的值，因此决策树适合用孩子链表来存储，并且算法也要基于孩子链表
typedef struct TreeData
{
	char Feature[ 10 ];//结点特征字符串
	char label; // 结点标签，‘Y’或‘N’，初始为‘#’
}DSTdata;
typedef struct Child *pChild;
struct Child
{
	int EdgeNum; //父结点与子结点的边的整数，代表一个特征的可能取值
	int ChildIndex; //子结点的索引
	pChild next; //孩子结点链表指针
};
typedef struct DSTChildLink
{
	DSTdata Data; //一个树结点
	pChild Head; //孩子链表的头结点
}DSTnode;

struct TreeList		  //孩子链表表示的整个树
{
	DSTnode *pArr = new DSTnode[MAXN];
	int size;
};
void Init( TreeList &Tree );//树的初始化
int getfeatureNum( int **X_train, int train_size, int FeaNum );	 //获取特征可能取值的个数
//数据预处理
void getTrainSetSize( int &sampleSize, int &featureSize,  FILE *&fp); //获取训练集规格，
bool getTrainData( int ** &X_train, int y_train[ ], int &train_size,
				   int feaNum, map<string, int> * &dicts, FILE *&fp );  //获取训练数据
void getTestSetSize( int &testSize, FILE *&fp ); //获取测试集规格
bool getFeatureSet( char ** &Feature, int &FeaNum, FILE * &fp ); //获取特征集合
bool getDicts( int feaNum, map<string, int> * &dicts, int &train_size, FILE * &fp );//生成一个特征字典
//训练与预测
void fit( int **X_train, int *y_train, int train_size, int *tindices, int &subtrainsize,
		  char **featureSet, int FeaNum, TreeList &Tree, int curNode, int &totalNode );//训练出决策树
void predict( int &test_size, int feaNum, map<string, int> * &dicts,
						TreeList &Tree, FILE *&fp, char **featureSet );  //对新数据分类
void showTree( TreeList &Tree ); //打印出树
void storeTree( TreeList &Tree); //将树以文件形式存到外存
#endif
