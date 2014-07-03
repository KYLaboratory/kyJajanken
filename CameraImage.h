#ifndef CAMERA_H
#define CAMERA_H

#include "opencv2/opencv.hpp"

//最大値を設定する変数とする
const unsigned short c_number_of_horizontal_pixel_max = 1920;//752;
const unsigned short c_number_of_vertical_pixel_max = 1080;//480;

const unsigned short c_delta_x = 1;
const unsigned short c_delta_y = 1;
const unsigned short c_index_num = 255;
const unsigned short c_pixcel_value_max = 255;
const unsigned short c_integral_thresh = 200;

struct illuminant
{
	double x;//X座標
	double y;//Y座標
	unsigned short number_of_used_pixcel;//座標算出に用いられたピクセル数
	unsigned short number_of_saturated_pixcel;//座標算出に用いられたピクセルで輝度値が飽和しているものの数
	double max_pixcel_intensity;//最高輝度
	double pixcel_total_intensity;//輝度積算値
	unsigned short width;//横幅
	unsigned short length;//縦幅
	unsigned short max_y;
	unsigned short min_y;
};


class CameraImage{
private :
	
	
	//画素の出力値
	static unsigned long s_sencer_output[c_number_of_vertical_pixel_max][c_number_of_horizontal_pixel_max];
	
	//背景出力を差し引いた画素の出力値
	static unsigned long s_sencer_output_backgroundless[c_number_of_vertical_pixel_max][c_number_of_horizontal_pixel_max];

	//歪み補正後のカメラ座標
	static double s_undistort_output_x[c_number_of_vertical_pixel_max][c_number_of_horizontal_pixel_max];
	static double s_undistort_output_y[c_number_of_vertical_pixel_max][c_number_of_horizontal_pixel_max];

	//周辺変更に対する輝度補正後の画像の濃度値
	static double s_vignetting_reduction_intensity[c_number_of_vertical_pixel_max][c_number_of_horizontal_pixel_max];
	static double s_vignetting_reduction_intensity_backgroundless[c_number_of_vertical_pixel_max][c_number_of_horizontal_pixel_max];
	
	//画素が有効か否か
	static bool is_valid_pixel[c_number_of_vertical_pixel_max][c_number_of_horizontal_pixel_max];
	
	//画素毎に持っているラベル番号
	static unsigned short label[c_number_of_vertical_pixel_max][c_number_of_horizontal_pixel_max];
	
	//ラベルのカウント値
	unsigned short label_count;
	
	//ラベル番号を覚えておくためのインデックス
	unsigned short label_index[c_index_num];
	
	//画素が取りうるラベル番号の最大値
	unsigned short label_index_max;

	//発光体
	illuminant image[c_index_num];

	//有効ピクセルの閾値
	unsigned short integral_thresh;

	//入力画像の水平方向の画素数
	unsigned short number_of_horizontal_pixel;
	
	//入力画像の垂直方向の画素数
	unsigned short number_of_vertical_pixel;


public :
	CameraImage();
	~CameraImage();
	void initialize();

	
	//発光画像用領域の初期化
	void initImage();
	
	//指定サイズ(水平×垂直)のカメラ画素を読込む
	void readData(cv::Mat rv_input_data);
		
	//指定サイズ(水平×垂直)を読込みファイル出力
	void readDataDebug(cv::Mat rv_input_data, std::ofstream* rv_output_file);
	
	//指定サイズ(水平×垂直)のカメラ画素(背景出力との差分)を読込む
	void readBackgroundlessData(cv::Mat rv_input_data);
	
	//指定サイズ(水平×垂直)のカメラ画素(背景出力との差分)を読込みファイル出力
	void readBackgroundlessDataDebug(cv::Mat rv_input_data, std::ofstream* rv_output_file);
	
	//指定サイズ(水平×垂直)の画像の閾値より大きい画素に対して積分値を計算
	double calcIntegral(std::ofstream* rv_output_file);
	
	//指定サイズ(水平×垂直)の画像の閾値より大きい画素に対して積分値(背景出力との差分)を計算
	double calcIntegralBackgroundless(std::ofstream* rv_output_file);

	//閾値を減算した状態で画像の重心座標を取得
	illuminant* getCenterOfGravityBackgroundlessData(std::ofstream* rv_output_file);

	//指定サイズ(水平×垂直)の画像の閾値より大きい画素に対して閾値を減算して重心座標を計算
	void calcFeatureQuantity(std::ofstream* rv_output_file);

	//指定サイズ(水平×垂直)のカメラ画像を２値化する
	void digitalizeData(std::ofstream* rv_output_file);
	
	//指定の閾値より大きい画素をラベリングする
	void labelData(std::ofstream* rv_output_file);
	
	//座標を昇順に並び替える
	void sortCoordinateAscendingOrder(illuminant* rv_data, unsigned short rv_num);

	//座標をピクセルの多い順に並び替える
	void sortCoordinatePixelDecendingOrder(illuminant* rv_data, unsigned short rv_num);

	//発光体の特徴量の格納先の前後の入れ替えを行う
	void swapIlluminants(illuminant* rv_data1, illuminant* rv_data2);

	//水平方向の画素数のセッター
	void setHorizontalPixel(unsigned short setValue);

	//垂直方向の画素数のセッター
	void setVerticalPixel(unsigned short setValue);

	//歪み補正後の発光画像を取得
	illuminant* getUndistortArea(std::ofstream* rv_output_file);

	//輝度分布に対する歪み補正
	void calcUndistortArea(std::ofstream* rv_output_file);

	//周辺減光に対する輝度補正後の発光画像を取得
	illuminant* getReduceVignettingArea(std::ofstream* rv_output_file);

	//周辺減光に対する輝度補正
	void reduceVignetting(std::ofstream* rv_output_file);

	

};
#endif
	
