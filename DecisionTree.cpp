

#include "DecisionTree.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <vector>
#include <map>
using namespace std;

//*****************************************************
//函数功能：初始化一棵孩子链表表示的决策树
//输入参数：孩子链表表示数组
//输出参数：初始化后的决策树
//返回值：	无
//*****************************************************
void Init( TreeList &Tree )
{
	//初始化每个结点的data信息
	for ( int i = 0; i < MAXN; i++ )
	{
		strcpy( Tree.pArr[ i ].Data.Feature, "empty" ); //特征信息初始化为empty
		Tree.pArr[ i ].Data.label = '#'; //标签初始化为#
		Tree.pArr[ i ].Head = NULL;
	}
	Tree.size = 0;
}

//*****************************************************
//函数功能：生成每个特征的字典，以字典数组形式存储
//输入参数：待写字典，特征数，训练集大小，文件指针引用
//输出参数：写好后的字典
//返回值：	true or false
//*****************************************************
bool getDicts( int feaNum, map<string, int> * &dicts, int &train_size, FILE * &fp )
{
	//fp是已经打开的文件指针，直接用就行,trainsize用于遍历
	int i, j;
	int *fea = new ( nothrow ) int[ feaNum ]{0};//以数组形式存放每个特征的可能取值
	char str[ 100 ], *p;
	string message;
	if ( fea == NULL )
	{
		cout << "memory allocate failed.";
		return false;
	}
	fseek( fp, 0, SEEK_SET );
	fgets( str, 100, fp );	//第一次读，跳过表头
	for ( i = 0; i < train_size; i++ )
	{
		fgets( str, 100, fp );
		p = strtok( str, "\t" );//先略过第一列，即行号
		for ( j = 0; j < feaNum; j++ )//然后遍历feanum个特征
		{
			p = strtok( NULL, "\t" );//p依次指向每个特征的取值
			message = p;
			if ( dicts[ j ].count( message ) == 0 )	//如果该key不在字典中
			{
				dicts[ j ][ message ] = fea[ j ]; //就把该键值对写入字典
				//cout << "写入" << dicts[j][message];
				fea[ j ]++; //该特征对应取值加一
			}
		}
	}
	delete[] fea;
	fea = NULL;
	return true;
}

//*****************************************************
//函数功能：获取训练数据集
//输入参数：指向指针的指针（二维数组），标签集，训练集大小引用，特征个数，文件指针引用
//输出参数：二维训练集，一维标签集，训练集大小
//返回值：	true or false
//*****************************************************
bool getTrainData( int ** &X_train, int y_train[ ], int &train_size,
				   int feaNum, map<string, int> * &dicts, FILE *&fp )
{
	int i, j;
	char str[ 100 ], *p;
	string message;
	fseek( fp, 0, SEEK_SET );//先定位到文件开头
	fgets( str, 100, fp );	//第一次读，跳过表头
	//依次遍历每行，把每个特征的字符串当作键，先判断是否在特征字典数组里，在就不管，
	//否则把键值对写入字典中，然后再把键对应的值写入训练数组里
	for ( i = 0; i < train_size; i++ )
	{
		fgets( str, 100, fp );
		p = strtok( str, "\t" );//先略过第一列，即行号
		for ( j = 0; j < feaNum; j++ )//然后遍历feanum个特征
		{
			p = strtok( NULL, "\t" );//p依次指向每个特征的取值
			message = p;
			X_train[ i ][ j ] = int(dicts[ j ][ message ]);
		}
		p = strtok( NULL, "\t" );//最后让p指向标签
		y_train[i] = (strcmp(p,"Y\n") == 0 ) ? 1 : 0;
	}
	return true;
}

//*****************************************************
//函数功能：获得某个特征的可能取值个数
//输入参数：训练集，特征索引
//输出参数：无
//返回值：	可能取值个数
//*****************************************************
int getfeatureNum( int **X_train, int train_size, int index )
{
	int subset = 0, i, j;
	vector<int> container;//利用容器来计算该属性的可能取值个数
	bool isthere;
	//遍历该属性所有取值，将不在容器里的值放入容器，最后计算容器大小，就是该属性可能取值个数
	for ( i = 0; i < train_size; i++ )
	{
		isthere = false;
		for ( j = 0; j < container.size(); j++ )
		{
			if ( X_train[ i ][ index ] == container[ j ] )
			{
				isthere = true;
				break;
			}
		}
		if ( !isthere )
		{
			container.push_back( X_train[ i ][ index ] );
		}
	}
	subset = container.end() - container.begin();  //该特征不同取值个数
	return subset;
}

//*****************************************************
//函数功能：获取特征集
//输入参数：特征二维数组, 文件指针引用
//输出参数：特征个数，特征集
//返回值：	true or false
//*****************************************************
bool getFeatureSet( char ** &Feature, int &FeaNum, FILE * &fp )
{
	char *pstr, str[100];
	int i;
	Feature = new ( nothrow ) char *[ FeaNum ];
	if ( Feature == NULL )
	{
		cout << "memory allocate failed.";
		return false;
	}
	fseek( fp, 0, SEEK_SET );	 //回到文件开头
	fgets( str, 100, fp ); //跳过第一行
	pstr = strtok( str, "\t" );	  //跳过数字
	for ( i = 0; i < FeaNum; i++ )
	{
		pstr = strtok( NULL, "\t" );
		Feature[ i ] = new ( nothrow ) char[ 10 ];
		if ( Feature[ i ] == NULL )
		{
			cout << "memory allocate failed.";
			return false;
		}
		strcpy( Feature[ i ], pstr );
	}
	return true;
}

//*****************************************************
//函数功能：获取训练集规格
//输入参数：样本数引用，特征数引用，文件指针引用
//输出参数：样本数，特征数
//返回值：	无
//*****************************************************
void getTrainSetSize( int &sampleSize, int &featureSize,  FILE *&fp )
{
	char str[ 100 ], *p;
	fseek(fp, 0, SEEK_SET);
	fgets( str, 100, fp );
	p = strtok( str, "\t" );
	while ( p != NULL ) //利用第一行计数特征
	{
		p = strtok( NULL, "\t" );
		if ( strcmp( p, "分类\n" ) == 0 )
		{
			break;
		}
		featureSize++;
	}
	while ( !feof( fp ) ) //然后数样本数
	{
		fgets( str, 100, fp );
		sampleSize++;
	}
    sampleSize--;
}

//*****************************************************
//函数功能：获取测试集规格
//输入参数：测试集大小引用，文件指针引用
//输出参数：测试集大小
//返回值：	无
//*****************************************************
void getTestSetSize( int &testSize, FILE *&fp )
{
	char str[ 100 ];
	fseek(fp,0,SEEK_SET);
	fgets( str, 100, fp );
	while ( !feof( fp ) )
	{
		fgets( str, 100, fp );
		testSize++;
	}
	testSize--;
}

//*****************************************************
//函数功能：训练模型
//输入参数：训练数据和训练标签，特征集，孩子链表表示的决策树
//输出参数：训练好的决策树
//返回值：	成功返回true，失败返回false
//*****************************************************
void fit( int **X_train, int *y_train, int train_size, int *tindices, int &subtrainsize,
		  char **featureSet, int FeaNum, TreeList &Tree, int curNode, int &totalNode )
{
	if ( totalNode > MAXN )
	{
		cout << "该树已达到最大结点数，退出";
		return;
	}
	//tindices作为训练集索引数组，每次以该数组值作为索引
	int subset, i, j, k, localSetSize, label, *temptindices;
	char **tempFeaSet;
	bool isEnd = true;
	temptindices = new (nothrow) int[subtrainsize];
	tempFeaSet = new(nothrow) char *[FeaNum];
	if(temptindices == NULL || tempFeaSet == NULL)
	{
		cout << "memory allocated failed.";
		exit(EXIT_FAILURE);
	}
	for(i=0;i<FeaNum;i++) //tempFeaSet作为本地变量，用它来给子结点训练，确保一个根节点
	{												//的子节点都使用同一训练集
		tempFeaSet[i] = new(nothrow) char [10];
		if(tempFeaSet[i] == NULL)
		{
			cout << "memory allocated failed.";
			exit(EXIT_FAILURE);
		}
		strcpy(tempFeaSet[i], featureSet[i]);
	}
	for(i = 0 ; i < subtrainsize ; i++)  //后面要对下标索引
		temptindices[i] = tindices[i];
	//先判断训练集里的样例是否都是同例
	if( subtrainsize < 2) // 防止训练子集只有一个元素时的情况
		j = tindices[0];
	for ( i = 1; i < subtrainsize; i++ )
	{
		j = tindices[ i ];		  //i从索引数组第二个元素开始
		k = tindices[ i - 1 ];	  //j，k作为标签集的索引
		if ( y_train[ j ] != y_train[ k ] ) //如果逻辑上相邻两个位置的标签不一样，说明没有完
		{
			isEnd = false;
			break;
		}
	}
	if ( isEnd == true )	//判断结束标记
	{
		label = y_train[ j ]; //lable为标签，0为N，1为Y
		Tree.pArr[ curNode ].Data.label = (label == 1) ? 'Y' : 'N';
		return;
	}
	//没结束，选择属性集中一个不为empty的属性，将该属性写入当前结点，然后修改特征集
	for(i=0;i<FeaNum;i++)
	{
		if(strcmp(tempFeaSet[i], "empty") != 0)
		{
			strcpy( Tree.pArr[ curNode ].Data.Feature, tempFeaSet[ i ] );
			strcpy( tempFeaSet[ i ], "empty");
			strcpy( featureSet[i], "empty");
			k = i;
			break;
		}
	}
	subset = getfeatureNum( X_train, train_size, k ); //特征可能取值个数存放在subset
	//subset值减一就是对应属性可能取值之一,从0到subset，划分n个子集
	pChild cur, newchild;
	Tree.pArr[ curNode ].Head = new ( nothrow ) Child; //给当前结点的头指针分配内存
	if ( Tree.pArr[ curNode ].Head == NULL )
	{
		cout << "memory allocate failed.";
		exit( EXIT_FAILURE );
	}
	cur = Tree.pArr[ curNode ].Head;
	for ( i = 0; i < subset; i++ )//循环subset次，创建当前结点的孩子链表
	{
		newchild = new ( nothrow ) Child;
		if ( newchild == NULL )
		{
			cout << "memory allocate failed.";
			exit( EXIT_FAILURE );
		}
		newchild->ChildIndex = totalNode + i;	//每次当前的totalnode就是子节点的下标位置
		newchild->EdgeNum = i;
		cur->next = newchild;
		cur = newchild;
	}
	totalNode += i; //更新总结点数
	Tree.size = totalNode;	//更新Tree的大小
	cur->next = NULL;	//链表最后一个结点next指空
	cur = Tree.pArr[ curNode ].Head->next; //然后让cur指向首元素结点
	localSetSize = subtrainsize;    //localSize是这个根结点时的子集大小
	while ( cur != NULL )		   //遍历该链表，以该结点的子节点作为根节点创建树
	{
		subtrainsize = 0;//每次先重置子集大小
		for ( i = 0; i < localSetSize; i++ )	 //	这个for在父节点时的子集上划分出一个子集
		{
			j = temptindices[i];
			if ( X_train[ j ][ k ] == cur->EdgeNum )//k是前面选出的属性的索引号
			{
				tindices[ subtrainsize ] = j;//把该取值的索引j放进索引数组里，然后subtrainsize++
				++subtrainsize;
			}
		}
		//然后用该子集递归调用fit，接着训练
		fit( X_train, y_train, train_size, tindices, subtrainsize,
			 tempFeaSet, FeaNum, Tree, cur->ChildIndex, totalNode );
		cur = cur->next;
		for(i=0;i<FeaNum;i++) //递归训练一个子节点后把特征集复制回去
		{
			strcpy(tempFeaSet[i], featureSet[i]);
		}
	}
	//回收内存
	for(i=0;i<FeaNum;i++)
	{
		delete[] tempFeaSet[i];
		tempFeaSet[i] = NULL;
	}
	delete  tempFeaSet;
	delete[] temptindices;
	temptindices = NULL;
	tempFeaSet = NULL;
}

//*****************************************************
//函数功能：给出预测集，输出结果
//输入参数：预测集，预测集大小，特征数，决策树引用
//输出参数：无
//返回值：
//*****************************************************
void predict(  int &test_size, int feaNum, map<string, int> * &dicts,
							TreeList &Tree, FILE *&fp, char **featureSet )
{
	int i, j, nodeindex, num;
	char str[ 100 ], *p;
	string mess;
	pChild cur;
    fseek(fp,0, SEEK_SET); //回到文件开头，之前用到这个文件指针的函数可能定位到文件后面去了
    fgets( str, 100, fp);   //跳过文件开头
    nodeindex = 0;
    cout << "编号\t天况\t温度\t湿度\t风况\t类别\n";
    for(i = 0; i < test_size ; i++)
	{
		fgets( str, 100, fp); //每次读入一行
		p = strchr(str, '\n'); //分隔，再输出
		*p = '\t';
		cout << str;
		nodeindex = 0;
		p = strtok(str, "\t");
        while(true)
		{
			if(Tree.pArr[nodeindex].Data.label != '#')
			{
				cout << Tree.pArr[nodeindex].Data.label << endl;
				break;
			}
			else
			{
				p = strtok(NULL ,"\t"); //依次指向每个特征
				mess = p;
				for(j = 0 ; j < feaNum ; j++) //先找到当前结点的特征字符串在字典中的序号
				{
					if(strcmp(featureSet[j], Tree.pArr[nodeindex].Data.Feature) == 0)
					{
						break;
					}
				}
				//j就是当前结点特征字符串在字典中的序号，比如 天况 序号为0
				num = dicts[j][mess]; //当前待预测的某个特征的值
				cur = Tree.pArr[nodeindex].Head->next;
				while(cur->EdgeNum != num)
					cur =  cur->next;
				nodeindex = cur->ChildIndex;
			}
		}
	}
}

//*****************************************************
//函数功能：打印决策树的信息
//输入参数：决策树引用
//输出参数：无
//返回值： 	 无
//*****************************************************
void showTree( TreeList &Tree )
{
	int i;
	pChild cur;
	cout << "The tree is as follow :\n";
	for(i=0;i<Tree.size;i++)
	{
		cout << i << " ";
		cout << "Feature: " << Tree.pArr[i].Data.Feature << ", label :" << Tree.pArr[i].Data.label;
		cout << " , child index and edge num: ";
		if( Tree.pArr[i].Head != NULL)
		{
			cur =Tree.pArr[i].Head->next;
			while(cur != NULL )
			{
				cout << cur->ChildIndex << "," << cur->EdgeNum << " ";
				cur = cur->next;
			}
		}
		cout << endl;
	}
}

//*****************************************************
//函数功能： 将训练好的决策树保存至文件中
//输入参数：决策树引用
//输出参数：无
//返回值： 	 无
//*****************************************************
void storeTree( TreeList &Tree)
{
		FILE *fp;
		pChild cur;
		int i;
		fp = fopen("tree.txt", "w");//以只读方式打开一个文本
		if (fp == NULL )
		{
			cout << "File open failed. ";
			exit(EXIT_FAILURE);
		}
        fprintf(fp, "//该文件存储训练好的决策树的信息\n");
        fprintf(fp, "//约定，每行保存一个决策树的结点，由多项组成：\n");
        fprintf(fp, "//第一项是树结点编号；\n");
        fprintf(fp, "//第二项为该结点的特征，“empty”表示无特征；\n");
        fprintf(fp, "//第三项为该结点的标签，“#”表示无标签；\n");
        fprintf(fp, "//第四项及以后为该结点的子节点分支标号和子节点编号，二者成对给出；");
        fprintf(fp, "//如果该结点没有子节点，则该行特征一定为“empty”，标签一定为“Y”或“N”，");
        fprintf(fp, "//否则该文件不是决策树文件。\n");
        fprintf(fp, "//结点数：%d\n\n", Tree.size);
        for(i = 0 ; i < Tree.size; i++)
		{
			fprintf(fp, "%d\t%s\t%c\t", i, Tree.pArr[i].Data.Feature,
																		  Tree.pArr[i].Data.label);
			if(Tree.pArr[i].Head != NULL ) //头指针不为空，说明有子结点
			{
                cur = Tree.pArr[i].Head->next;//普通的链表的遍历
                while( cur!= NULL )
				{
                    fprintf(fp, "(%d,%d)\t",cur->EdgeNum, cur->ChildIndex);
                    cur = cur->next;
				}
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
}
