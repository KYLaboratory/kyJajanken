//
//  TrickEstimater.cpp
//  JajankenHunter
//
//  Created by akihito on 2014/06/21.
//
//

#include "TrickEstimater.h"
#include <fstream>

const unsigned char c_integral_thresh = 175;

bool is_sharp_angle[NUMBER_OF_HANDS][NUMBER_OF_CONTOUR_COORDINATE] = { false };
unsigned short label_index[NUMBER_OF_HANDS][NUMBER_OF_CONTOUR_COORDINATE] = { 0 };
unsigned short label_index_max = 0;
std::ofstream* the_output_file = NULL;

TrickEstimater::TrickEstimater()
{
    initialize();
	the_output_file = new std::ofstream("result.txt");
}

TrickEstimater::~TrickEstimater()
{
    
}

TrickEstimaterRef TrickEstimater::create()
{
    return TrickEstimaterRef(new TrickEstimater());
}

void TrickEstimater::initialize()
{
    label_index_max = 0;
    for(int i = 0; i < NUMBER_OF_CONTOUR_COORDINATE; i++){
        for(int j = 0; j < NUMBER_OF_HANDS; j++){
            is_sharp_angle[j][i] = false;
            label_index[j][i] = 0;
        }
    }
}

double TrickEstimater::calcNorm(double rv_x1, double rv_y1, double rv_x2, double rv_y2)
{
	double norm = 0.0;
	norm = sqrt((rv_x1 - rv_x2) * (rv_x1 - rv_x2) + (rv_y1 - rv_y2) * (rv_y1 - rv_y2));
	return norm;
}

double TrickEstimater::calcInnerProduct(double rv_x1, double rv_y1, double rv_x2, double rv_y2, double rv_x3, double rv_y3)
{
	double inner_product = 0.0;
	inner_product = (rv_x1 - rv_x2) * (rv_x3 - rv_x2) + (rv_y1 - rv_y2) * (rv_y3 - rv_y2);
	return inner_product;
}

double TrickEstimater::calcOuterProduct(double rv_x1, double rv_y1, double rv_x2, double rv_y2, double rv_x3, double rv_y3)
{
	double outer_product = 0.0;
	outer_product = (rv_x1 - rv_x2) * (rv_y3 - rv_y2) - (rv_y1 - rv_y2) * (rv_x3 - rv_x2);
	return outer_product;
}

void TrickEstimater::labelContour(unsigned int contour_id, unsigned int rv_size)
{
	LabelingBS	labeling;
	bool at_is_sort_size_order = false;
	int at_delete_size = 0;
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
/*
double getContourAreaMax(vector<vector<cv::Point>> rv_contours, vector<Vec4i> rv_hierarchy)
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
*/
void TrickEstimater::calcContourFeature()
{
	double theta = 0.0;
	double norm_a = 0.0;
	double norm_b = 0.0;
	double inner_product_ab = 0.0;
	double outer_product_ab = 0.0;
	int interval = 6;
	unsigned short contour_id[5] = {0};
	
	int k = 0;
	for (int i = 0; i < contours.size(); i++){
        std::cerr << contours[i].size() << std::endl;
		if (contours[i].size() >  2 * interval){
			for (int j = 0; j < contours[i].size() - 2 * interval; j++){
				norm_a = calcNorm(contours[i][j].x, contours[i][j].y, contours[i][j + interval].x, contours[i][j + interval].y);
				norm_b = calcNorm(contours[i][j + 2 * interval].x, contours[i][j + 2 * interval].y, contours[i][j + interval].x, contours[i][j + interval].y);
				inner_product_ab = calcInnerProduct(contours[i][j].x, contours[i][j].y, contours[i][j + interval].x, contours[i][j + interval].y, contours[i][j + 2 * interval].x, contours[i][j + 2 * interval].y);
				theta = acos(inner_product_ab / (norm_a * norm_b));
				outer_product_ab = calcOuterProduct(contours[i][j].x, contours[i][j].y, contours[i][j + interval].x, contours[i][j + interval].y, contours[i][j + 2 * interval].x, contours[i][j + 2 * interval].y);
				is_sharp_angle[i][j + interval] = isSharpAngle(theta);
				*the_output_file << contours[i][j].x << "\t" << contours[i][j].y << "\t" << norm_a << "\t" << norm_b << "\t" << inner_product_ab << "\t" << theta << "\t" << outer_product_ab << std::endl;
			}
			contour_id[k] = i;
			labelContour(contour_id[k], contours[i].size());
			k++;
			*the_output_file << label_index_max << std::endl;
		}
	}
}

bool TrickEstimater::isSharpAngle(const double rv_theta)
{
	double sharp_angle_thresh = 1.4;//[rad]:60deg:1.04719
	if (rv_theta < sharp_angle_thresh){
		//*the_output_file << contours[i][j + interval].x << "\t" << contours[i][j + interval].y << "\t" << theta << "\t" << outer_product_ab << std::endl;
		return true;
	}
	else{
		return false;
	}
}

EHAND TrickEstimater::judgeScissorsPaperRock(const cv::Mat& rv_src_img)
{

    cv::Mat mask_img;
	cv::threshold(rv_src_img, mask_img, c_integral_thresh, 255, THRESH_BINARY);
	
	vector<Vec4i> hierarchy;
    contours.clear();
	findContours(mask_img, contours, hierarchy,	CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	
	calcContourFeature();
    
	if(label_index_max  == 1 || label_index_max  == 2)return EHAND::eHAND_ROCK;
	else if(label_index_max == 3 || label_index_max == 4)return EHAND::eHAND_SCISSORS;
	else if(label_index_max > 4)return EHAND::eHAND_PAPER;
	else return EHAND::eHAND_ERROR;
}

EHAND TrickEstimater::estimateTrick(const cv::Mat& image, const cv::Rect& rect)
{
    try{
        return judgeScissorsPaperRock(image(rect));
    }
    catch(...)
    {
        std::cerr << "estimate error" << std::endl;
    }
    return eHAND_ERROR;
}