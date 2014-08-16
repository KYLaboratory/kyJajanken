#include "HandRecognizer.h"

using namespace std;

bool operator < (const RecognizeResult& result1, const RecognizeResult& result2) {
	return result1.rate >result2.rate; 
}

bool operator > (const RecognizeResult& result1, const RecognizeResult& result2) {
	return result1.rate < result2.rate; 
}


double distance(int i, int j){
	return (i-j)*(i-j) + 1;
}

HandRecognizer::HandRecognizer(void)
{
	dpMatcher.init(distance,1.0);
}


HandRecognizer::~HandRecognizer(void)
{
}


EHAND HandRecognizer::recognize(const Mat& image)
{
	Mat resized;
    resize(image, resized, cv::Size(recognizeWidth, recognizeHeight));
	vector<int> line = lineScan(resized);

	map<double,string> mp = dpMatcher.matchN(line,5);
	map<double, string>::iterator itr = mp.begin();
    
	if(itr->second == "gu"){
		return eHAND_ROCK;
	}
	else if(itr->second == "choki"){
		return eHAND_SCISSORS;
	}
	else if(itr->second == "pa"){
		return eHAND_PAPER;
	}
    
    return eHAND_ERROR;
}

void HandRecognizer::addSample(const string& fileName, const EHAND& trick)
{
    Mat img = ci::toOcv(ci::loadImage(fileName));
	//Mat img = imread(fileName,0);
	Mat resized;
	resize(img, resized, cv::Size(recognizeWidth, recognizeHeight));
	vector<int> line = lineScan(resized);
    
    switch(trick)
    {
        case eHAND_ROCK:
            dpMatcher.setData(line, "gu");
            break;
        case eHAND_SCISSORS:
            dpMatcher.setData(line, "choki");
            break;
        case eHAND_PAPER:
            dpMatcher.setData(line, "pa");
            break;
        default:
            break;
    }
}

int HandRecognizer::countMatchingPix(const Mat& img1, const Mat& img2)
{

	//double count =0;
	//for(int y = 0; y < img1.rows; y++){
	//	for(int x = img1.cols/2; x < img1.cols; x++){
	//		unsigned char pixVal1 = img1.data[y * img1.step + x];
	//		unsigned char pixVal2 = img2.data[y * img2.step + x];
	//		int p = 255-abs((int)pixVal1-(int)pixVal2);
	//		count+= p;
	//
	//	}
	//}
	//imwrite("neko.bmp",img2);
	vector<int> v2 = lineScan(img2);
	vector<int> v1 = lineScan(img1);
    
	int count = 0;
	for(int i = 0; i < v1.size(); i++)
    {
		if(v1[i] == v2[i])
        {
			count++;
		}
	}
	return count;
}

vector<RecognizeResult> HandRecognizer::matchingAll(const Mat& question)
{
	vector<RecognizeResult> resultVec;
	resize(question, question, cv::Size(50, 30));
	for(int i=0; i<samples.size(); i++){
		RecognizeResult result;
		result.number = i;
		result.answer = answers[i];
		result.rate = countMatchingPix(samples[i], question);
		resultVec.push_back(result);
	}

	sort(resultVec.begin(), resultVec.end());
	return resultVec;
}

//Mat HandRecognizer::rightSideImage(Mat img){
//	Mat outImg(img.size(),CV_8UC1, Scalar(255));
//	string name = "out.csv";
//	ofstream file(name.c_str(),ios::app);
//
//	for(int y = 0; y < img.rows; y++){
//		int count = 0;
//		for(int x = img.cols - 1; x >= 0 ; x--){
//			unsigned char pixVal1 = img.data[y * img.step + x];
//			if(pixVal1 <100){
//				outImg.data[y * outImg.step + x] = 0;
//				count++;
//			}
//			else{
//				file<<count<<",";
//				goto label1;
//
//			}
//		}
//		label1:;
//	}
//	file << endl;
//	file.close();
//	return outImg;
//}

vector<int> HandRecognizer::lineScan(const Mat& img)
{
	Mat outImg(img.size(),CV_8UC1, Scalar(255));
	vector<int> result;
	
	for(int x = 0;/*img.cols/2;*/ x < img.cols - 5 ; x++)
    {
		int whitecount=0;
		bool whiteNow = false;
		for(int y = 0; y < img.rows; y++){
            
			unsigned char pixVal1 = img.data[y * img.step + x];
			if(pixVal1 >100)
            {
				if(!whiteNow)
                {
					whitecount++;
					whiteNow = true;
				}
			}
			else{
				whiteNow = false;
			}
		}
	}
    
	return result;
}