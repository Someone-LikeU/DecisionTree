

#include "DecisionTree.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <vector>
#include <map>
using namespace std;

//*****************************************************
//�������ܣ���ʼ��һ�ú��������ʾ�ľ�����
//������������������ʾ����
//�����������ʼ����ľ�����
//����ֵ��	��
//*****************************************************
void Init( TreeList &Tree )
{
	//��ʼ��ÿ������data��Ϣ
	for ( int i = 0; i < MAXN; i++ )
	{
		strcpy( Tree.pArr[ i ].Data.Feature, "empty" ); //������Ϣ��ʼ��Ϊempty
		Tree.pArr[ i ].Data.label = '#'; //��ǩ��ʼ��Ϊ#
		Tree.pArr[ i ].Head = NULL;
	}
	Tree.size = 0;
}

//*****************************************************
//�������ܣ�����ÿ���������ֵ䣬���ֵ�������ʽ�洢
//�����������д�ֵ䣬��������ѵ������С���ļ�ָ������
//���������д�ú���ֵ�
//����ֵ��	true or false
//*****************************************************
bool getDicts( int feaNum, map<string, int> * &dicts, int &train_size, FILE * &fp )
{
	//fp���Ѿ��򿪵��ļ�ָ�룬ֱ���þ���,trainsize���ڱ���
	int i, j;
	int *fea = new ( nothrow ) int[ feaNum ]{0};//��������ʽ���ÿ�������Ŀ���ȡֵ
	char str[ 100 ], *p;
	string message;
	if ( fea == NULL )
	{
		cout << "memory allocate failed.";
		return false;
	}
	fseek( fp, 0, SEEK_SET );
	fgets( str, 100, fp );	//��һ�ζ���������ͷ
	for ( i = 0; i < train_size; i++ )
	{
		fgets( str, 100, fp );
		p = strtok( str, "\t" );//���Թ���һ�У����к�
		for ( j = 0; j < feaNum; j++ )//Ȼ�����feanum������
		{
			p = strtok( NULL, "\t" );//p����ָ��ÿ��������ȡֵ
			message = p;
			if ( dicts[ j ].count( message ) == 0 )	//�����key�����ֵ���
			{
				dicts[ j ][ message ] = fea[ j ]; //�ͰѸü�ֵ��д���ֵ�
				//cout << "д��" << dicts[j][message];
				fea[ j ]++; //��������Ӧȡֵ��һ
			}
		}
	}
	delete[] fea;
	fea = NULL;
	return true;
}

//*****************************************************
//�������ܣ���ȡѵ�����ݼ�
//���������ָ��ָ���ָ�루��ά���飩����ǩ����ѵ������С���ã������������ļ�ָ������
//�����������άѵ������һά��ǩ����ѵ������С
//����ֵ��	true or false
//*****************************************************
bool getTrainData( int ** &X_train, int y_train[ ], int &train_size,
				   int feaNum, map<string, int> * &dicts, FILE *&fp )
{
	int i, j;
	char str[ 100 ], *p;
	string message;
	fseek( fp, 0, SEEK_SET );//�ȶ�λ���ļ���ͷ
	fgets( str, 100, fp );	//��һ�ζ���������ͷ
	//���α���ÿ�У���ÿ���������ַ��������������ж��Ƿ��������ֵ�������ھͲ��ܣ�
	//����Ѽ�ֵ��д���ֵ��У�Ȼ���ٰѼ���Ӧ��ֵд��ѵ��������
	for ( i = 0; i < train_size; i++ )
	{
		fgets( str, 100, fp );
		p = strtok( str, "\t" );//���Թ���һ�У����к�
		for ( j = 0; j < feaNum; j++ )//Ȼ�����feanum������
		{
			p = strtok( NULL, "\t" );//p����ָ��ÿ��������ȡֵ
			message = p;
			X_train[ i ][ j ] = int(dicts[ j ][ message ]);
		}
		p = strtok( NULL, "\t" );//�����pָ���ǩ
		y_train[i] = (strcmp(p,"Y\n") == 0 ) ? 1 : 0;
	}
	return true;
}

//*****************************************************
//�������ܣ����ĳ�������Ŀ���ȡֵ����
//���������ѵ��������������
//�����������
//����ֵ��	����ȡֵ����
//*****************************************************
int getfeatureNum( int **X_train, int train_size, int index )
{
	int subset = 0, i, j;
	vector<int> container;//������������������ԵĿ���ȡֵ����
	bool isthere;
	//��������������ȡֵ���������������ֵ����������������������С�����Ǹ����Կ���ȡֵ����
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
	subset = container.end() - container.begin();  //��������ͬȡֵ����
	return subset;
}

//*****************************************************
//�������ܣ���ȡ������
//���������������ά����, �ļ�ָ������
//�������������������������
//����ֵ��	true or false
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
	fseek( fp, 0, SEEK_SET );	 //�ص��ļ���ͷ
	fgets( str, 100, fp ); //������һ��
	pstr = strtok( str, "\t" );	  //��������
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
//�������ܣ���ȡѵ�������
//������������������ã����������ã��ļ�ָ������
//�����������������������
//����ֵ��	��
//*****************************************************
void getTrainSetSize( int &sampleSize, int &featureSize,  FILE *&fp )
{
	char str[ 100 ], *p;
	fseek(fp, 0, SEEK_SET);
	fgets( str, 100, fp );
	p = strtok( str, "\t" );
	while ( p != NULL ) //���õ�һ�м�������
	{
		p = strtok( NULL, "\t" );
		if ( strcmp( p, "����\n" ) == 0 )
		{
			break;
		}
		featureSize++;
	}
	while ( !feof( fp ) ) //Ȼ����������
	{
		fgets( str, 100, fp );
		sampleSize++;
	}
    sampleSize--;
}

//*****************************************************
//�������ܣ���ȡ���Լ����
//������������Լ���С���ã��ļ�ָ������
//������������Լ���С
//����ֵ��	��
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
//�������ܣ�ѵ��ģ��
//���������ѵ�����ݺ�ѵ����ǩ�������������������ʾ�ľ�����
//���������ѵ���õľ�����
//����ֵ��	�ɹ�����true��ʧ�ܷ���false
//*****************************************************
void fit( int **X_train, int *y_train, int train_size, int *tindices, int &subtrainsize,
		  char **featureSet, int FeaNum, TreeList &Tree, int curNode, int &totalNode )
{
	if ( totalNode > MAXN )
	{
		cout << "�����Ѵﵽ����������˳�";
		return;
	}
	//tindices��Ϊѵ�����������飬ÿ���Ը�����ֵ��Ϊ����
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
	for(i=0;i<FeaNum;i++) //tempFeaSet��Ϊ���ر��������������ӽ��ѵ����ȷ��һ�����ڵ�
	{												//���ӽڵ㶼ʹ��ͬһѵ����
		tempFeaSet[i] = new(nothrow) char [10];
		if(tempFeaSet[i] == NULL)
		{
			cout << "memory allocated failed.";
			exit(EXIT_FAILURE);
		}
		strcpy(tempFeaSet[i], featureSet[i]);
	}
	for(i = 0 ; i < subtrainsize ; i++)  //����Ҫ���±�����
		temptindices[i] = tindices[i];
	//���ж�ѵ������������Ƿ���ͬ��
	if( subtrainsize < 2) // ��ֹѵ���Ӽ�ֻ��һ��Ԫ��ʱ�����
		j = tindices[0];
	for ( i = 1; i < subtrainsize; i++ )
	{
		j = tindices[ i ];		  //i����������ڶ���Ԫ�ؿ�ʼ
		k = tindices[ i - 1 ];	  //j��k��Ϊ��ǩ��������
		if ( y_train[ j ] != y_train[ k ] ) //����߼�����������λ�õı�ǩ��һ����˵��û����
		{
			isEnd = false;
			break;
		}
	}
	if ( isEnd == true )	//�жϽ������
	{
		label = y_train[ j ]; //lableΪ��ǩ��0ΪN��1ΪY
		Tree.pArr[ curNode ].Data.label = (label == 1) ? 'Y' : 'N';
		return;
	}
	//û������ѡ�����Լ���һ����Ϊempty�����ԣ���������д�뵱ǰ��㣬Ȼ���޸�������
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
	subset = getfeatureNum( X_train, train_size, k ); //��������ȡֵ���������subset
	//subsetֵ��һ���Ƕ�Ӧ���Կ���ȡֵ֮һ,��0��subset������n���Ӽ�
	pChild cur, newchild;
	Tree.pArr[ curNode ].Head = new ( nothrow ) Child; //����ǰ����ͷָ������ڴ�
	if ( Tree.pArr[ curNode ].Head == NULL )
	{
		cout << "memory allocate failed.";
		exit( EXIT_FAILURE );
	}
	cur = Tree.pArr[ curNode ].Head;
	for ( i = 0; i < subset; i++ )//ѭ��subset�Σ�������ǰ���ĺ�������
	{
		newchild = new ( nothrow ) Child;
		if ( newchild == NULL )
		{
			cout << "memory allocate failed.";
			exit( EXIT_FAILURE );
		}
		newchild->ChildIndex = totalNode + i;	//ÿ�ε�ǰ��totalnode�����ӽڵ���±�λ��
		newchild->EdgeNum = i;
		cur->next = newchild;
		cur = newchild;
	}
	totalNode += i; //�����ܽ����
	Tree.size = totalNode;	//����Tree�Ĵ�С
	cur->next = NULL;	//�������һ�����nextָ��
	cur = Tree.pArr[ curNode ].Head->next; //Ȼ����curָ����Ԫ�ؽ��
	localSetSize = subtrainsize;    //localSize����������ʱ���Ӽ���С
	while ( cur != NULL )		   //�����������Ըý����ӽڵ���Ϊ���ڵ㴴����
	{
		subtrainsize = 0;//ÿ���������Ӽ���С
		for ( i = 0; i < localSetSize; i++ )	 //	���for�ڸ��ڵ�ʱ���Ӽ��ϻ��ֳ�һ���Ӽ�
		{
			j = temptindices[i];
			if ( X_train[ j ][ k ] == cur->EdgeNum )//k��ǰ��ѡ�������Ե�������
			{
				tindices[ subtrainsize ] = j;//�Ѹ�ȡֵ������j�Ž����������Ȼ��subtrainsize++
				++subtrainsize;
			}
		}
		//Ȼ���ø��Ӽ��ݹ����fit������ѵ��
		fit( X_train, y_train, train_size, tindices, subtrainsize,
			 tempFeaSet, FeaNum, Tree, cur->ChildIndex, totalNode );
		cur = cur->next;
		for(i=0;i<FeaNum;i++) //�ݹ�ѵ��һ���ӽڵ������������ƻ�ȥ
		{
			strcpy(tempFeaSet[i], featureSet[i]);
		}
	}
	//�����ڴ�
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
//�������ܣ�����Ԥ�⼯��������
//���������Ԥ�⼯��Ԥ�⼯��С��������������������
//�����������
//����ֵ��
//*****************************************************
void predict(  int &test_size, int feaNum, map<string, int> * &dicts,
							TreeList &Tree, FILE *&fp, char **featureSet )
{
	int i, j, nodeindex, num;
	char str[ 100 ], *p;
	string mess;
	pChild cur;
    fseek(fp,0, SEEK_SET); //�ص��ļ���ͷ��֮ǰ�õ�����ļ�ָ��ĺ������ܶ�λ���ļ�����ȥ��
    fgets( str, 100, fp);   //�����ļ���ͷ
    nodeindex = 0;
    cout << "���\t���\t�¶�\tʪ��\t���\t���\n";
    for(i = 0; i < test_size ; i++)
	{
		fgets( str, 100, fp); //ÿ�ζ���һ��
		p = strchr(str, '\n'); //�ָ��������
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
				p = strtok(NULL ,"\t"); //����ָ��ÿ������
				mess = p;
				for(j = 0 ; j < feaNum ; j++) //���ҵ���ǰ���������ַ������ֵ��е����
				{
					if(strcmp(featureSet[j], Tree.pArr[nodeindex].Data.Feature) == 0)
					{
						break;
					}
				}
				//j���ǵ�ǰ��������ַ������ֵ��е���ţ����� ��� ���Ϊ0
				num = dicts[j][mess]; //��ǰ��Ԥ���ĳ��������ֵ
				cur = Tree.pArr[nodeindex].Head->next;
				while(cur->EdgeNum != num)
					cur =  cur->next;
				nodeindex = cur->ChildIndex;
			}
		}
	}
}

//*****************************************************
//�������ܣ���ӡ����������Ϣ
//�������������������
//�����������
//����ֵ�� 	 ��
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
//�������ܣ� ��ѵ���õľ������������ļ���
//�������������������
//�����������
//����ֵ�� 	 ��
//*****************************************************
void storeTree( TreeList &Tree)
{
		FILE *fp;
		pChild cur;
		int i;
		fp = fopen("tree.txt", "w");//��ֻ����ʽ��һ���ı�
		if (fp == NULL )
		{
			cout << "File open failed. ";
			exit(EXIT_FAILURE);
		}
        fprintf(fp, "//���ļ��洢ѵ���õľ���������Ϣ\n");
        fprintf(fp, "//Լ����ÿ�б���һ���������Ľ�㣬�ɶ�����ɣ�\n");
        fprintf(fp, "//��һ����������ţ�\n");
        fprintf(fp, "//�ڶ���Ϊ�ý�����������empty����ʾ��������\n");
        fprintf(fp, "//������Ϊ�ý��ı�ǩ����#����ʾ�ޱ�ǩ��\n");
        fprintf(fp, "//������Ժ�Ϊ�ý����ӽڵ��֧��ź��ӽڵ��ţ����߳ɶԸ�����");
        fprintf(fp, "//����ý��û���ӽڵ㣬���������һ��Ϊ��empty������ǩһ��Ϊ��Y����N����");
        fprintf(fp, "//������ļ����Ǿ������ļ���\n");
        fprintf(fp, "//�������%d\n\n", Tree.size);
        for(i = 0 ; i < Tree.size; i++)
		{
			fprintf(fp, "%d\t%s\t%c\t", i, Tree.pArr[i].Data.Feature,
																		  Tree.pArr[i].Data.label);
			if(Tree.pArr[i].Head != NULL ) //ͷָ�벻Ϊ�գ�˵�����ӽ��
			{
                cur = Tree.pArr[i].Head->next;//��ͨ������ı���
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
