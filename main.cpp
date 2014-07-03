// opencv_sample.cpp : ÉRÉìÉ\Å[Éã ÉAÉvÉäÉPÅ[ÉVÉáÉìÇÃÉGÉìÉgÉä É|ÉCÉìÉgÇíËã`ÇµÇ‹Ç∑ÅB
//

//#include <direct.h>
#include "CameraImage.h"
#include <string.h>
#include "Labeling.h"
#include "handClip.h"
#include "opencv2/opencv.hpp"

using namespace cv;

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

//à»â∫ñ{ìñÇÕÉNÉâÉXëÆê´ÇÃÉXÉRÅ[ÉvÇ™ìKÇµÇƒÇ¢ÇÈÇ™ÅAÉNÉâÉXÇ™Ç»Ç¢ÇÃÇ≈âºÇ≈ÉOÉçÅ[ÉoÉãÉXÉRÅ[ÉvÇ∆Ç∑ÇÈ
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

	//âÊëúÉfÅ[É^ÇÃì«çû
	src_img = cv::imread(input_filepath_str,0);

	cv::Mat imageResult;

	cv::Mat gray_img;
	cv::Mat bin_img;
	//cv::cvtColor(src_img, gray_img, CV_BGR2GRAY);
	cv::threshold(src_img, bin_img, 200, 255, cv::THRESH_BINARY);// LabelingÇÃåãâ ÇéÛÇØéÊÇÈ

	// ÉâÉxÉäÉìÉOåãâ ÇèoóÕÇ∑ÇÈÅAê^Ç¡îíÇ»èÛë‘Ç≈èâä˙âª
    cv::Mat out_img(bin_img.size(), CV_8UC1, cv::Scalar(255));
	cv::Mat label(bin_img.size(), CV_16SC1);
 
    // ÉâÉxÉäÉìÉOÇé¿é{ ÇQílâªÇµÇΩâÊëúÇ…ëŒÇµÇƒé¿çsÇ∑ÇÈÅB
    LabelingBS  labeling;
    labeling.Exec(bin_img.data, (short *)label.data, bin_img.cols, bin_img.rows, true, 2000);  //ëÂÇ´Ç≥2,000à»â∫ÇÃÉâÉxÉãÇÕêÿÇËéÃÇƒ
      
    // ÉâÉxÉãÇ≥ÇÍÇΩóÃàÊÇÇ–Ç∆Ç¬Ç∏Ç¬ï`âÊ
    for( int i = 0; i < labeling.GetNumOfRegions(); i++)
    {
        // ÉâÉxÉäÉìÉOåãâ Ç≈ÉCÉçÉCÉçÇ∑ÇÈÅB
        // ÉâÉxÉäÉìÉOåãâ ÇÃóÃàÊÇíäèoÇ∑ÇÈÅB
        cv::Mat labelarea;
        cv::compare(label, i + 1, labelarea, CV_CMP_EQ);
        // íäèoÇµÇΩóÃàÊÇ…ÉâÉìÉ_ÉÄÇ»êFÇê›íËÇµÇƒèoóÕâÊëúÇ…í«â¡ÅB
        cv::Mat color(src_img.size(), CV_8UC1, randomColor());
        color.copyTo(out_img, labelarea);
    }

	HandInfo leftHand;
	HandInfo rightHand;

	if(getHandsInfo(bin_img, leftHand, rightHand)){
		cv::rectangle(out_img, leftHand.handRect, randomColor(), 3);
		cv::rectangle(out_img, rightHand.handRect, randomColor(),3);
	}
    
    //ここまで

	//cv::Mat cut_left_img(src_img, Rect(leftHand.handRect));
	//cv::imwrite("left_cut.bmp", cut_left_img);
	//cv::Mat cut_left_img2 = cv::imread("left_cut.bmp",0);

	cv::Mat cut_right_img = src_img(Rect(rightHand.handRect)).clone();
	
	//cv::namedWindow("cut_left", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
	//cv::imshow("cut_left", cut_left_img);

	cv::namedWindow("cut_right", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
	cv::imshow("cut_right", cut_right_img);

	//EHAND hand = judgeScissorsPaperRock(cut_left_img2);
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
	bool at_is_sort_size_order = false;//óÃàÊÇÃëÂÇ´Ç≥èáÇ…É\Å[ÉgÇµÇ»Ç¢
	int at_delete_size = 0;    //è¡ãéÇ∑ÇÈè¨óÃàÊÇÃç≈ëÂÉTÉCÉY(int) - Ç±ÇÍà»â∫ÇÃÉTÉCÉYÇÃóÃàÊÇè¡ãéÇ∑ÇÈ
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

	// ÉgÉbÉvÉåÉxÉãÇ…Ç†ÇÈÇ∑Ç◊ÇƒÇÃó÷äsÇâ°ífÇµÅAäeòAåãê¨ï™ÇÉâÉìÉ_ÉÄÇ»êFÇ≈ï`âÊ
	for (int i = 0; i < rv_contours.size(); i++){
		Scalar color(rand() & 255, rand() & 255, rand() & 255);
		contour_area = cv::contourArea(rv_contours[i]);
		if (contour_area > contour_area_thresh){
			if (max_contour_area[0] < contour_area){
				max_contour_area[0] = contour_area;
				drawContours(dst_img, rv_contours, idx, color, 1, 8, rv_hierarchy);//ÉfÉoÉbÉOèàóù
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
				//ó÷äsÇÃâsäpÇåvéZ
				norm_a = calcNorm(contours[i][j].x, contours[i][j].y, contours[i][j + interval].x, contours[i][j + interval].y);
				norm_b = calcNorm(contours[i][j + 2 * interval].x, contours[i][j + 2 * interval].y, contours[i][j + interval].x, contours[i][j + interval].y);
				inner_product_ab = calcInnerProduct(contours[i][j].x, contours[i][j].y, contours[i][j + interval].x, contours[i][j + interval].y, contours[i][j + 2 * interval].x, contours[i][j + 2 * interval].y);
				theta = acos(inner_product_ab / (norm_a * norm_b));
				outer_product_ab = calcOuterProduct(contours[i][j].x, contours[i][j].y, contours[i][j + interval].x, contours[i][j + interval].y, contours[i][j + 2 * interval].x, contours[i][j + 2 * interval].y);
				is_sharp_angle[i][j + interval] = isSharpAngle(theta);
				//*the_output_file << contours[i][j].x << "\t" << contours[i][j].y << "\t" << norm_a << "\t" << norm_b << "\t" << inner_product_ab << "\t" << theta << "\t" << outer_product_ab << std::endl;
			}
			//ó÷äsÇÃâsäpóÃàÊÇÃÉâÉxÉãêîÇì¡í•ó Ç∆ÇµÇƒéZèo
			contour_id[k] = i;
			labelContour(contour_id[k], contours[i].size());
			k++;
			*the_output_file << label_index_max << std::endl;
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
	//2ílâª
	cv::threshold(rv_src_img, mask_img, c_integral_thresh, 255, THRESH_BINARY);

	cv::namedWindow("mask", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
	cv::imshow("mask", mask_img);
	
	//ó÷äsíäèo
	vector<Vec4i> hierarchy;
	findContours(mask_img, contours, hierarchy,	CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	
	calcContourFeature();

	if(label_index_max == 1)return EHAND::eHAND_ROCK;
	else if(label_index_max == 3)return EHAND::eHAND_SCISSORS;
	else if(label_index_max == 9)return EHAND::eHAND_PAPER;
	else return EHAND::eHAND_ERROR;
}
