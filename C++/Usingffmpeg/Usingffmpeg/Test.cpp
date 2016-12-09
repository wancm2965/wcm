#include <iostream>
#include <stdio.h>
#include "SplitVideo.h"
using std::cout;
using std::endl;

int main(int argc, char * argv[])
{
	string inputfile = "E:/video/remux/ts.mp4";
	string outputfile = "E:/video/remux/mp4.mp4";
	vector<string> vec;
	SplitVideo *sv = new SplitVideo(inputfile, outputfile);
	if (sv->executeSplit(60))
	{
		//�õ����е�����ļ���
		vec = sv->getResultName();
		cout << "split success" << endl;
	}
	else{
		cout << "failed" << endl;
	}
	delete sv;

	return 0;
}