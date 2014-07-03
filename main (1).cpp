// opencv_sample.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <direct.h>
#include <string.h>
#include "Labeling.h"
#include "handClip.h"

const unsigned char c_integral_thresh = 200;
const unsigned char c_file_name_size = 50;
#define NUMBER_OF_HANDS 2

enum EHAND{
	eHAND_ROCK,
	eHAND_SCISSORS,
	eHAND_PAPER,
	eHAND_ERROR
};

enum EBATTLE_STATE{
	eUSER1_WIN,
	eUSER2_WIN,
	eDRAW
};

//以下本当はクラス属性のスコープが適しているが、クラスがないので仮でグローバルスコープとする
cv::Mat src_img;
cv::Mat dst_img;
cv::Mat mask_img;
vector<vector<Point>> contours;
vector<vector<Point>> max_contours;

void *memset(void *s, int c, size_t n);
double calcNorm(double rv_x1, double rv_y1, double rv_x2, double rv_y2);
double calcInnerProduct(double rv_x1, double rv_y1, double rv_x2, double rv_y2, double rv_x3, double rv_y3);
double calcOuterProduct(double rv_x1, double rv_y1, double rv_x2, double rv_y2, double rv_x3, double rv_y3);
void labelContour(unsigned int contour_id, unsigned int rv_size);
double getContourAreaMax(vector<vector<Point>> rv_contours, vector<Vec4i> rv_hierarchy);
void calcContourFeature();
bool isSharpAngle(double rv_theta);
EHAND judgeScissorsPaperRock(cv::Mat rv_src_img);

bool is_sharp_angle[NUMBER_OF_HANDS][10000] = { false };
unsigned short label_index[NUMBER_OF_HANDS][10000] = { 0 };
unsigned short label_index_max = 0;
std::ofstream* the_output_file = NULL;


int _tmain(int argc, _TCHAR* argv[])
{
	char input_filepath_str[c_file_name_size] = { 0 };
	std::cout << "Please input your file name.Please input 'end' when finishing process." << std::endl;
	std::cin >> input_filepath_str;
	sprintf_s(input_filepath_str, "%s.bmp", input_filepath_str);

	//画像データの読込
	src_img = cv::imread(input_filepath_str,0);

	cv::Mat imageResult;

	cv::Mat gray_img;
	cv::Mat bin_img;
	//cv::cvtColor(src_img, gray_img, CV_BGR2GRAY);
	cv::threshold(src_img, bin_img, 200, 255, cv::THRESH_BINARY);// Labelingの結果を受け取る

	// ラベリング結果を出力する、真っ白な状態で初期化
    cv::Mat out_img(bin_img.size(), CV_8UC1, cv::Scalar(255));
	cv::Mat label(bin_img.size(), CV_16SC1);
 
    // ラベリングを実施 ２値化した画像に対して実行する。
    LabelingBS  labeling;
    labeling.Exec(bin_img.data, (short *)label.data, bin_img.cols, bin_img.rows, true, 2000);  //大きさ2,000以下のラベルは切り捨て

	HandInfo leftHand;
	HandInfo rightHand;
	cv::Mat cut_left_img;
	cv::Mat cut_right_img;
	EHAND hand1 = eHAND_ERROR;
	EHAND hand2 = eHAND_ERROR;

	if(getHandsInfo(bin_img, leftHand, rightHand)){
		cv::rectangle(out_img, leftHand.handRect, randomColor(), 3);
		cv::rectangle(out_img, rightHand.handRect, randomColor(),3);
		cut_left_img = src_img(Rect(leftHand.handRect)).clone();
		cut_right_img = src_img(Rect(rightHand.handRect)).clone();
		hand1 = judgeScissorsPaperRock(cut_left_img);
		hand2 = judgeScissorsPaperRock(cut_right_img);
	}


	
	waitKey(0);

	return 0;
}


double calcNorm(double rv_x1, double rv_y1, double rv_x2, double rv_y2)
{
	double norm = 0.0;
	norm = sqrt((rv_x1 - rv_x2) * (rv_x1 - rv_x2) + (rv_y1 - rv_y2) * (rv_y1 - rv_y2));
	return norm;
}

double calcInnerProduct(double rv_x1, double rv_y1, double rv_x2, double rv_y2, double rv_x3, double rv_y3)
{
	double inner_product = 0.0;
	inner_product = (rv_x1 - rv_x2) * (rv_x3 - rv_x2) + (rv_y1 - rv_y2) * (rv_y3 - rv_y2);
	return inner_product;
}

double calcOuterProduct(double rv_x1, double rv_y1, double rv_x2, double rv_y2, double rv_x3, double rv_y3)
{
	double outer_product = 0.0;
	outer_product = (rv_x1 - rv_x2) * (rv_y3 - rv_y2) - (rv_y1 - rv_y2) * (rv_x3 - rv_x2);
	return outer_product;
}

void labelContour(unsigned int contour_id, unsigned int rv_size)
{
	LabelingBS	labeling;
	bool at_is_sort_size_order = false;//領域の大きさ順にソートしない
	int at_delete_size = 0;    //消去する小領域の最大サイズ(int) - これ以下のサイズの領域を消去する
	unsigned char *src = new unsigned char[rv_size];
	short *result = new short[rv_size];

	for (int i = 0; i < rv_size; i++){
		src[i] = (unsigned char)is_sharp_angle[contour_id][i];
	}
	labeling.Exec(src, result, rv_size, 1, at_is_sort_size_order, at_delete_size);
	label_index_max = labeling.GetNumOfResultRegions();

	for (int i = 0; i < rv_size; i++){
		is_sharp_angle[contour_id][i] = (unsigned short)result[i];
	}
	for (int i = 0; i < label_index_max; i++){
		label_index[contour_id][i] = (labeling.GetResultRegionInfo(i))->GetResult();
	}

	delete[] src;
	delete[] result;

}

double getContourAreaMax(vector<vector<Point>> rv_contours, vector<Vec4i> rv_hierarchy)
{
	double max_contour_area[NUMBER_OF_HANDS] = { 0.0 };
	double contour_area = 0;
	double contour_area_thresh = 8000;
	int maxidx = 0;
	int idx = 0;

	// トップレベルにあるすべての輪郭を横断し、各連結成分をランダムな色で描画
	for (int i = 0; i < rv_contours.size(); i++){
		Scalar color(rand() & 255, rand() & 255, rand() & 255);
		contour_area = cv::contourArea(rv_contours[i]);
		if (contour_area > contour_area_thresh){
			if (max_contour_area[0] < contour_area){
				max_contour_area[0] = contour_area;
				drawContours(dst_img, rv_contours, idx, color, 1, 8, rv_hierarchy);//デバッグ処理
			}
		}
		idx = rv_hierarchy[idx][0];
	}
	return max_contour_area[0];
}

void calcContourFeature()
{
	double theta = 0.0;
	double norm_a = 0.0;
	double norm_b = 0.0;
	double inner_product_ab = 0.0;
	double outer_product_ab = 0.0;
	int interval = 10;
	unsigned short contour_id[5] = {0};
	
	int k = 0;
	for (int i = 0; i < contours.size(); i++){
		if (contours[i].size() >  2 * interval){
			for (int j = 0; j < contours[i].size() - 2 * interval; j++){
				//輪郭の鋭角を計算
				norm_a = calcNorm(contours[i][j].x, contours[i][j].y, contours[i][j + interval].x, contours[i][j + interval].y);
				norm_b = calcNorm(contours[i][j + 2 * interval].x, contours[i][j + 2 * interval].y, contours[i][j + interval].x, contours[i][j + interval].y);
				inner_product_ab = calcInnerProduct(contours[i][j].x, contours[i][j].y, contours[i][j + interval].x, contours[i][j + interval].y, contours[i][j + 2 * interval].x, contours[i][j + 2 * interval].y);
				theta = acos(inner_product_ab / (norm_a * norm_b));
				outer_product_ab = calcOuterProduct(contours[i][j].x, contours[i][j].y, contours[i][j + interval].x, contours[i][j + interval].y, contours[i][j + 2 * interval].x, contours[i][j + 2 * interval].y);
				is_sharp_angle[i][j + interval] = isSharpAngle(theta);
				//*the_output_file << contours[i][j].x << "\t" << contours[i][j].y << "\t" << norm_a << "\t" << norm_b << "\t" << inner_product_ab << "\t" << theta << "\t" << outer_product_ab << std::endl;
			}
			//輪郭の鋭角領域のラベル数を特徴量として算出
			contour_id[k] = i;
			labelContour(contour_id[k], contours[i].size());
			k++;
			//*the_output_file << label_index_max << std::endl;
		}
	}
}

bool isSharpAngle(double rv_theta)
{
	double sharp_angle_thresh = 1.04719;//[rad]:60deg
	if (rv_theta < sharp_angle_thresh){
		//*the_output_file << contours[i][j + interval].x << "\t" << contours[i][j + interval].y << "\t" << theta << "\t" << outer_product_ab << std::endl;
		return true;
	}
	else{
		return false;
	}
}

EHAND judgeScissorsPaperRock(cv::Mat rv_src_img)
{
	//2値化
	cv::threshold(rv_src_img, mask_img, c_integral_thresh, 255, THRESH_BINARY);
	
	//輪郭抽出
	vector<Vec4i> hierarchy;
	findContours(mask_img, contours, hierarchy,	CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	
	calcContourFeature();

	if(label_index_max == 0)return EHAND::eHAND_ROCK;
	else if(label_index_max == 3)return EHAND::eHAND_SCISSORS;
	else if(label_index_max == 9)return EHAND::eHAND_PAPER;
	else return EHAND::eHAND_ERROR;
}
