#pragma once

#include "HuffmanTree.h"
#define  _CRT_SECURE_NO_WARNINGS


typedef long LongType;

// #define _DEBUG_ //���������������Ϣ
//#define _TIME_  //���ܲ���

// �ļ���Ϣ
struct FileInfo
{
	unsigned char _ch;		// �ַ�
	LongType _appearCount;	// ���ִ���
	string _code;	// �ַ���Ӧ��huffman����

	FileInfo(int appearCount = 0)
		:_ch(0)
		, _appearCount(appearCount)
	{}

	bool operator < (const FileInfo& info) const
	{
		return _appearCount < info._appearCount;
	}

	bool operator != (const FileInfo& info) const
	{
		return _appearCount != info._appearCount;
	}

	FileInfo operator +(const FileInfo& info)
	{
		FileInfo tmp;
		tmp._appearCount = _appearCount + info._appearCount;
		return tmp;

		//return FileInfo(_appearCount + info._appearCount);
	}
};

ostream& operator<<(ostream& os, const FileInfo& info)
{
	os << info._ch << ":" << info._appearCount;
	return os;
}

class FileCompress
{
private:
	FileInfo _fileInfos[256];
public:
	FileCompress()
	{
		for (int i = 0; i < 256; ++i)
		{
			_fileInfos[i]._ch = i;
		}
	}

	//��ȡ�ļ�һ��
	bool ReadLine(FILE* fOut, string& line)
	{
		assert(fOut);

		char ch = fgetc(fOut);
		//����
		if (ch == EOF)
			return false;

		while (ch != '\n' && ch != EOF)
		{
			line += ch;
			ch = fgetc(fOut);
		}

		return true;
	}

	void Compress(const char* fileName)
	{
		assert(fileName);
		long long charCount = 0;//�ļ����ַ�����
		// 1.��ȡ�ļ�ͳ���ַ������ܹ��Ĵ�����
		FILE* fOut = fopen(fileName, "rb");//�Զ���������
		char ch = fgetc(fOut);


#ifdef _TIME_
		cout << "Compress:: ��ȡ�ļ�ͳ���ַ������ܹ��Ĵ�����ʱ�䣺";
		int begin = GetTickCount();
#endif
		while (ch != EOF)
		{
			_fileInfos[(unsigned char)ch]._appearCount++;
			ch = fgetc(fOut);
			++charCount;
		}
#ifdef _TIME_
		int end = GetTickCount();
		cout << end - begin << endl;
#endif
		// 2.�����ַ����ֵĴ�������Huffman����������ÿ���ַ���Ӧ��Huffman����
		HuffmanTree<FileInfo> tree;
		FileInfo invalid(0);
#ifdef _TIME_
		cout << "Compress::CreateHuffmanTree()ʱ�䣺";
		begin = GetTickCount();
#endif
		tree.CreateHuffmanTree(_fileInfos, 256, invalid);
#ifdef _TIME_
		end = GetTickCount();
		cout << end - begin << endl;
#endif

#ifdef _DEBUG_
		tree.LevelOrder();
#endif
#ifdef _TIME_
		cout << "Compress::_GenerateHuffmanCode()ʱ��  ";
		begin = GetTickCount();
#endif
		_GenerateHuffmanCode(tree.GetRootNode());

#ifdef _TIME_
		end = GetTickCount();
		cout << end - begin << endl;
#endif
		string compressFileName = fileName;
		compressFileName += ".Compress";
		FILE* fIn = fopen(compressFileName.c_str(), "wb");

		// 3.��ѹ������д��ѹ���ļ���
		fseek(fOut, 0, SEEK_SET);//��ͷ��ʼ��ȡ
		ch = fgetc(fOut);
		int pos = 0;
		char value = 0;
		while (ch != EOF)
		{
			string& code = _fileInfos[(unsigned char)ch]._code;
#ifdef _DEBUG_
			cout << _fileInfos[(unsigned char)ch]<<code << "->";
#endif
			//
			for (size_t i = 0; i < code.size(); ++i)
			{
				value <<= 1;

				if (code[i] == '1')
				{
					value |= 1;
				}

				if (++pos == 8)
				{
					fputc(value, fIn);
					pos = 0;
					value = 0;
				}
			}

			ch = fgetc(fOut);
		}

		if (pos)
		{
			value <<= (8 - pos);
			fputc(value, fIn);
		}

		// 4.��Huffman������Ϣд�������ļ���
		string configFileName = fileName;
		configFileName += ".config";
		FILE* fInConfig = fopen(configFileName.c_str(), "wb");
		assert(fInConfig);
		string infoStr;
		char countStr[20];

		// ��д���ַ�����
		_itoa(charCount >> 32, countStr, 10);//charCount  �ļ����ַ�����
		fputs(countStr, fInConfig);
		fputc('\n', fInConfig);

		_itoa(charCount & 0xFFFFFFFF, countStr, 10);
		fputs(countStr, fInConfig);
		fputc('\n', fInConfig);

		for (int i = 0; i < 256; ++i)
		{
			if (_fileInfos[i] != invalid)
			{
				infoStr = _fileInfos[i]._ch;
				infoStr += ',';
				_itoa(_fileInfos[i]._appearCount, countStr, 10);
				infoStr += countStr;
				infoStr += '\n';

				fputs(infoStr.c_str(), fInConfig);
			}
		}

		fclose(fIn);
		fclose(fOut);
		fclose(fInConfig);
	}

	void Uncompress(const char* fileName)
	{
		// 1.��ȡ�����ļ���Huffman������Ϣ
		string configFileName = fileName;
		configFileName += ".config";
		FILE* fOutConfig = fopen(configFileName.c_str(), "rb");
		string line;
		char ch = 0;
		LongType appearCount = 0;

		// ��ȡ�ļ����ַ�����
		long long charCount = 0;
		ReadLine(fOutConfig, line);
		charCount = atoi(line.c_str());
		charCount <<= 32;
		ReadLine(fOutConfig, line);
		charCount += atoi(line.c_str());
		line.clear();

		while (ReadLine(fOutConfig, line))
		{
			// ������һ�����У����Ӧ�ַ�Ϊ���з�
			if (!line.empty())
			{
				//sscanf(line.c_str(), "%s,%d", ch, appearCount);
				ch = line[0];
				_fileInfos[(unsigned char)ch]._appearCount = atoi(line.substr(2).c_str());
				line.clear();
			}
			else
			{
				line = '\n';
			}
		}

		// 2.���ݶ�����������Ϣ���ؽ�Huffman��
		HuffmanTree<FileInfo> tree;
		FileInfo invalid(0);
		tree.CreateHuffmanTree(_fileInfos, 256, invalid);

#ifdef _DEBUG_
		tree.LevelOrder();
#endif
		HuffmanNode<FileInfo>* root = tree.GetRootNode();

		// 3. ��ȡѹ����Ϣ�������ؽ���Huffman����ѹ��
		string uncompressFileName = fileName;
		uncompressFileName += ".Uncompress";
		FILE* fIn = fopen(uncompressFileName.c_str(), "wb");

		string compressFileName = fileName;
		compressFileName += ".Compress";
		FILE* fOut = fopen(compressFileName.c_str(), "rb");

		// 3.1����ѹ���ļ��ַ�������Huffman����Ѱ�Ҷ�Ӧ���ַ�
		int pos = 8;
		HuffmanNode<FileInfo>* cur = root;
		ch = fgetc(fOut);
		while (charCount > 0)
		{
			//�ҵ�Ҷ�ӽڵ�
			while (cur && cur->_left && cur->_right)
			{
				if (pos == 0)
				{
					pos = 8;
					ch = fgetc(fOut);
				}

				--pos;

				if (ch & 1 << pos)//�õ�ch�е�posλ
				{
					//
					cur = cur->_right;
#ifdef _DEBUG_
					cout << "1";
#endif
				}
				else
				{
					cur = cur->_left;
#ifdef _DEBUG_
					cout << "0";
#endif
				}
			}
			//cur��Ҷ�ӽڵ㣬
			if (cur && charCount--)
			{
#ifdef _DEBUG_
				cout << "->";
#endif
				fputc(cur->_weight._ch, fIn);
				cur = root;
			}
		}

		cout << endl;

		fclose(fIn);
		fclose(fOut);
		fclose(fOutConfig);
	}
protected:
	void _GenerateHuffmanCode(HuffmanNode<FileInfo>* root)
	{
		if (root)
		{
			_GenerateHuffmanCode(root->_left);
			_GenerateHuffmanCode(root->_right);

			// Ҷ�ӽڵ���Ϊ����ڵ㣬˳��Ҷ�ӽڵ�����Ѱ��Huffman����
			if (root->_left == NULL && root->_right == NULL)
			{
				// code��_fileInfos�����б����ַ�����������á�
				string& code = _fileInfos[root->_weight._ch]._code;

				HuffmanNode<FileInfo>* cur = root;
				HuffmanNode<FileInfo>* parent = root->_parent;
				while (parent)
				{
					if (parent->_left == cur)
					{
						code += '0';
					}
					else
					{
						code += '1';
					}

					cur = parent;
					parent = cur->_parent;
				}

				// Ѱ�ұ����Ǵ�Ҷ�ӽڵ㵽���ڵ㣬����Ҫ�Ա����������
				reverse(code.begin(), code.end());

			}
		}
	}

};

void TestCompress()
{
	FileCompress fc;
	int begin = GetTickCount();

	// ѹ��
	fc.Compress("in.txt");
 

	int end = GetTickCount();
	cout << "Compress:" << end - begin << endl;


}
void TestUncompress()
{
	FileCompress fc;
	int	begin = GetTickCount();

	// ��ѹ��

	fc.Uncompress("in.txt");
 

	int end = GetTickCount();
	cout << "Uncompress:" << end - begin << endl;
}