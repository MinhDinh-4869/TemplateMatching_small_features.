#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define STB_IMAGE_IMPLEMENTATION
#include "./.h/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./.h/stb_image_write.h"
#include<math.h>
#include<string.h>

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void printProgress(double percentage);
long int SAD(unsigned char* arr_a, unsigned char* arr_b, int length );
long int sum_sq_arr(unsigned char* array, int length);
long int sum_arr(unsigned char* array, int length);
long int sum_xy(unsigned char* arr_a, unsigned char* arr_b, int length);
double find_corr(unsigned char* arr_a, unsigned char* arr_b, int length);
double find_NCC(unsigned char* arr_a, unsigned char* arr_b, int length);
void copyImage(unsigned char* src, unsigned char* des, int length);
void colorBG(unsigned char* img, unsigned char color, int length );
void pasteImg(unsigned char* src, unsigned char* des, int h_loc, int w_loc, int h_src, int w_src, int h_des, int w_des, int c);
void getFeature(unsigned char* temp,unsigned char* feature,int h_temp, int w_temp, int h_loc, int w_loc,int channels, int win_size);
void TempMatch(unsigned char* img, unsigned char* temp, double* corr_mat, int h_img, int w_img, int h_temp, int w_temp, int channels);
void maxLoc(double* corr_mat,int* matchLoc, int h_temp, int w_temp, int h_img, int w_img);
void mkDot(unsigned char* mask,int h_img, int w_img,int height, int width, int channels);


int main(void)
{

	char file[] = "image11.jpg";
	
	char file_path[40] = "./image/";
	char temp_path[40]="./template/";
	char result_path[40]="./result/";
	char mask_path[40]="./mask/";
	//char match_temp_path[40]="./template/";
	strcat(file_path, file);
	strcat(temp_path,file);
	strcat(result_path,file);
	strcat(mask_path,file);
	//strcat(match_temp_path,file);
	int w_i, h_i,w_t, h_t, c_i, c_t;
	unsigned char *img = stbi_load(file_path, &w_i, &h_i, &c_i, 0);
	puts("Loading...\n");
	if(img == NULL) {
        printf("Error in loading the image\n");
        exit(1);
    }
	unsigned char *temp = stbi_load(temp_path, &w_t, &h_t, &c_t, 0);
	if(temp == NULL) {
        printf("Error in loading the template\n");
        exit(2);
    }
    puts("loaded");
    printf("Image size: %dx%d\nTemplate size: %dx%d\n",w_i,h_i,w_t,h_t);
	int size_img = (h_i*w_i*c_i);
	int temp_size = (h_t*w_t*c_t); //??????????
	unsigned char *out_img  = malloc(size_img);
	unsigned char *out_img_test  = malloc((h_i + h_t)*(w_i + w_t)*c_i);
	//unsigned char *out_img_fin  = malloc((h_i + h_t)*(w_i + w_t)*c_i);
	unsigned char *mask  = malloc(size_img);
	unsigned char *pro_mask = malloc(size_img);
	int d_h = h_i - h_t;
	int d_w = w_i - w_t;
	//const int window_size = 6;
	//const int feature_size = window_size*window_size*c_t;

	int mask_mat[h_i*w_i];// =malloc((sizeof(int)*h_i*w_i));
	int cp_mask_mat[h_i*w_i];// =malloc((sizeof(int)*h_i*w_i));
	for(int i=0; i<h_i*w_i; i++)
	{
		mask_mat[i] = 0;
		cp_mask_mat[i] = 0;
	}
	//int* density = malloc((sizeof(int)*h_i*w_i));

	srand(time(NULL));

	//int h_loops = (int)(h_t/window_size);
	//int w_loops = (int)(w_t/window_size);


	colorBG(out_img_test, 255, (h_i+h_t)*(w_i+w_t)*c_i);
	colorBG(mask, 255, h_i*w_i*c_i);
	colorBG(pro_mask, 255, h_i*w_i*c_i);
	colorBG(out_img_test, 255, (h_i+h_t)*(w_i+w_t)*c_i);
	pasteImg(img, out_img_test,0, 0,h_i, w_i, (h_i+h_t), (w_i+w_t),c_i);
	pasteImg(temp, out_img_test,0, w_i,h_t, w_t, (h_i+h_t), (w_i+w_t),c_i);
	//copyImage(img, out_img, h_i*w_i*c_i);
	puts("Processing...\n");
	const int NUM_FEA = 20;

	unsigned char temp_arr[temp_size];
	for(int i=0;i<temp_size;i++)
	{
		temp_arr[i] =temp[i];
	}

//for(int vert=0; vert< h_t-window_size; vert+=window_size){//index of small temp{
//	for(int horz=0;horz < w_t - window_size; horz+=window_size){
for(int window_size=25; window_size<=30; window_size++)
{
int feature_size = window_size*window_size*c_t;
for(int loop = 0; loop<NUM_FEA; loop++)
{

	int vert = rand()%(h_t - window_size);
	int horz = rand()%(w_t - window_size);
	
	unsigned char feature_arr[window_size*window_size*c_t];
	getFeature(temp, feature_arr, h_t, w_t, vert, horz,c_t, window_size);

	
	double* corr_matrix = malloc((sizeof(double*))*(h_i - window_size)*(w_i - window_size));
	TempMatch(img, feature_arr, corr_matrix, h_i, w_i, window_size, window_size,c_i );

	int matchLoc[2];
	maxLoc(corr_matrix,matchLoc, window_size, window_size, h_i, w_i );
	int height = matchLoc[0];
	int width = matchLoc[1];

	free(corr_matrix);

	/*for(int k=0;k<c_i;k++)
	{
		mask[height*w_i*c_i + width*c_i +k ] = 0;
		//pro_mask[height*w_i*c_i + width*c_i +k ] = 0;
	}*/
	mkDot(mask, h_i, w_i, height, width, c_i);
	mask_mat[height*w_i + width] = 1;
	//cp_mask_mat[height*w_i + width] = 1;
	//puts("Template was found at:");
	//printf("Height pixel:\t%d\n",height);
	//printf("Width pixel:\t%d\n",width);
	for(int i=0; i<h_i*w_i; i++)
{
	cp_mask_mat[i] = 0;
}


	int color = 0;
		
	{	for(int k=0;k<window_size;k++)

		{
			for(int c=0;c<c_i;c++)
			{
			out_img[height*w_i*c_i + (width+k)*c_i +c ] = color;
			out_img[(height+window_size)*w_i*c_i + (width+k)*c_i +c ] = color;
			out_img_test[height*(w_i+w_t)*c_i + (width+k)*c_i +c ] = color;
			out_img_test[(height+window_size)*(w_i+w_t)*c_i + (width+k)*c_i +c ] = color;
			out_img_test[vert*(w_i+w_t)*c_i + (horz + w_i +k)*c_i +c ] = color;
			out_img_test[(vert+window_size)*(w_i+w_t)*c_i + (horz +w_i+k)*c_i +c ] = color;
			}
		}

		for(int k=0;k<window_size;k++)

		{
			for(int c=0;c<c_i;c++)
			{
			out_img[(height+k)*w_i*c_i + (width)*c_i +c ] = color;
			out_img[(height+k)*w_i*c_i + (width+window_size)*c_i +c ] = color;
			out_img_test[(height+k)*(w_i+w_t)*c_i + (width)*c_i +c ] = color;
			out_img_test[(height+k)*(w_i+w_t)*c_i + (width+window_size)*c_i +c ] = color;
			out_img_test[(vert+k)*(w_i+w_t)*c_i + (horz + w_i)*c_i +c ] = color;
			out_img_test[(vert+k)*(w_i+w_t)*c_i + (horz + w_i +window_size)*c_i +c ] = color;
			}
		}
	}
	//printProgress((double)(vert*w_t + horz)/(w_t*h_t));
	printProgress((double)(loop+1)/NUM_FEA);



	//draw lines: i!= height, j!= widths
	for(int i=0;i<h_i+h_t;i++)
	{
		for(int j=0;j<w_i+w_t;j++)
		{

			//unsigned char array_a[] = {vert - height, horz + w_i - width};
			//unsigned char array_b[] = {i - height, j - width};
			//printf("%f\n",find_NCC(array_b,array_a, 2) );
			double x = (double)(i - height)/(vert - height);
			double y = (double)(j - width)/(horz + w_i - width);
			//printf("%f\t%f\n",x,y );
			if ((i!= height) && (j!= width) && x==y && x > 0 && y > 0)
			{
				for (int k =0;k<c_i;k++)
				{
				out_img_test[i*(w_t+w_i)*c_i + j*c_i + k]=0;			}
			}			
		}
	}



//	}
//}
}
printf("%d\n",window_size );
}
/*putchar('\n');

for(int i=0; i<80; i++)
{
	for(int j=90; j <145; j++)
	{
		printf("%d ",cp_mask_mat[i*w_i + j]);
	}
	putchar('\n');
}*/


//CHECKOUT THIS PART
int ker_size = 10;

for(int i=ker_size;i<h_i-ker_size;i++)
{
	for(int j =ker_size; j<w_i-ker_size;j++)
	{
		//if(i <= ker_size && j <= ker_size && i > h_i - ker_size && j>w_i - ker_size)
		//{
		//	cp_mask_mat[i*w_i + j] = 0;
		//}
		//else{
		int den = 0;
		for(int p = -10; p < 10; p++)
		{
			for(int q = -10; q < 10; q++)
			{
				den = den +  mask_mat[(i+p)*w_i + (q+j)];
			}
		}
		//if(den > 0)
		//{
		//	printf("%d\n",den );
		//}
		//printf("%d\n",den );
		//density[i*w_i + j] = res;
		if(/*mask_mat[i*w_i + j] == 1 && */den > (int)(ker_size/2))//including 0 entries with den> 1/2 size
		{
			cp_mask_mat[i*w_i + j] = 1;
		}
		//}
	}
}

//find highest density.

/*int h_den=0, w_den=0, result=density[0];
for (int i=0;i<h_i;i++)
{
	for(int j=0; j<w_i;j++)
	{
		if(density[i*w_i + j] > result)
		{
			result = density[i*w_i + j];
			h_den = i;
			w_den = j;		
		}
	}
}
int detec_size = 25;
int color =0;
	{	for(int k=-detec_size;k<detec_size;k++)

		{
			for(int c=0;c<c_i;c++)
			{
			out_img_fin[h_den*w_i*c_i + (w_den+k)*c_i +c ] = color;
			out_img_fin[(h_den+2*detec_size)*w_i*c_i + (w_den+k)*c_i +c ] = color;
			}
		}

		for(int k=-detec_size;k<detec_size;k++)

		{
			for(int c=0;c<c_i;c++)
			{
			out_img_fin[(h_den+k)*w_i*c_i + (w_den)*c_i +c ] = color;
			out_img_fin[(h_den+k)*w_i*c_i + (w_den+2*detec_size)*c_i +c ] = color;
			}
		}
	}

*/

for(int i=0;i<h_i;i++)
{
	for(int j=0;j<w_i;j++)
	{
		if(cp_mask_mat[i*w_i + j] == 1)
		{
			for(int k=0;k<c_i;k++)
			{
				pro_mask[i*w_i*c_i + j*c_i +k ] = 0;
			}
		}
		else
		{
			for(int k=0;k<c_i;k++)
			{
				pro_mask[i*w_i*c_i + j*c_i +k ] = 255;
			}
		}
	}
}

	putchar('\n');
	puts("Saving result...");
	//stbi_write_png("result.jpg", w_i, h_i,c_i, out_img, w_i*c_i);
	stbi_write_png("./mask/mask.jpg", w_i, h_i,c_i, mask, w_i*c_i);
	//stbi_write_png("fin.jpg", w_i, h_i,c_i, out_img_fin, w_i*c_i);
	stbi_write_png(mask_path, w_i, h_i,c_i, pro_mask, w_i*c_i);
	stbi_write_png(result_path, w_i+w_t, h_i+h_t,c_i, out_img_test, (w_i+w_t)*c_i);
	//free(mask_mat);
	//free(cp_mask_mat);
	//free(density);
	stbi_image_free(mask);
	stbi_image_free(pro_mask);
    stbi_image_free(img);
    stbi_image_free(out_img);
    stbi_image_free(temp);
    stbi_image_free(out_img_test);
    //FILE *ptr;
    //ptr = fopen("corr_data.txt", "a");
    //fprintf(ptr, "%d,%d\n", height, width);
    //fclose(ptr);
    puts("Finish!!");
	return(0);
}



void printProgress(double percentage) {
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}
long int SAD(unsigned char arr_a[], unsigned char arr_b[], int length )
{
	long int sad = 0;
	for(int i=0;i<length;i++)
	{
		sad+=abs(arr_a[i] - arr_b[i]);
	}
	return(sad);
}
long int sum_sq_arr(unsigned char* array, int length)
{
	long int result = 0;
	for (int i = 0; i < length; i++)
	{
		result += (array[i] * array[i]);
	}
	return result;
}

long int sum_arr(unsigned char* array, int length)
{
	long int result = 0;
	for (int i = 0; i < length; i++)
	{
		result += (array[i]);
	}
	return result;
}
long int sum_xy(unsigned char* arr_a, unsigned char* arr_b, int length)
{
	long int result = 0;
	for (int i = 0; i < length; i++)
	{
		result += (arr_a[i] * arr_b[i]);
	}
	return result;
}
double find_corr(unsigned char* arr_a, unsigned char* arr_b, int length)
{
	long int sum_x_y = sum_xy(arr_a, arr_b, length);
	long int sum_x = sum_arr(arr_a, length);
	long int sum_y = sum_arr(arr_b, length);
	long int sum_sq_x = sum_sq_arr(arr_a, length);
	long int sum_sq_y = sum_sq_arr(arr_b, length);
	long int nominator = length * sum_x_y - sum_x * sum_y;
	double denominator_1 = sqrt(length * sum_sq_x - sum_x * sum_x);
	double denominator_2 = sqrt(length * sum_sq_y - sum_y * sum_y);
	double result = (double)nominator / (denominator_1 * denominator_2);
	return result;
}

double find_NCC(unsigned char* arr_a, unsigned char* arr_b, int length)
{
	long int sum_x_y = sum_xy(arr_a, arr_b, length);
	long int sum_sq_x = sum_sq_arr(arr_a, length);
	long int sum_sq_y = sum_sq_arr(arr_b, length);
	double denominator = sqrt(sum_sq_x*sum_sq_y);
	double result = (double)sum_x_y/denominator;
	return result;
}

void copyImage(unsigned char* src, unsigned char* des, int length)
{
	for(int i=0;i<length;i++)
	{
		des[i] = src[i];
	}
}
void colorBG(unsigned char* img, unsigned char color, int length )
{
	for(int i=0;i<length;i++)
	{
		img[i] = color;
	}
}
void pasteImg(unsigned char* src, unsigned char* des, int h_loc, int w_loc, int h_src, int w_src, int h_des, int w_des, int c)
{
	for(int i=0;i<h_src;i++)
	{
		for(int j=0;j<w_src; j++)
		{
			for(int k=0;k<c;k++)
			{
				des[(i + h_loc)*(w_des)*c + (j+w_loc)*c + k] = src[i*w_src*c + j*c + k];
			}
		}
	}
}

void getFeature(unsigned char* temp,unsigned char* feature,int h_temp, int w_temp, int h_loc, int w_loc,int channels, int win_size )
{
for(int i=0;i<win_size;i++)
	{
		for(int j=0;j<win_size;j++)
		{
			for(int k=0;k<channels;k++)
			{
				feature[i*win_size*channels + j*channels + k] = temp[(h_loc+i)*w_temp*channels + (w_loc+j)*channels +k];
			}
		}
	}
}
void TempMatch(unsigned char* img, unsigned char* temp, double* corr_mat, int h_img, int w_img, int h_temp, int w_temp, int channels)
{
	for(int i=0;i<h_img - h_temp;i++)
	{
		for(int j=0;j<w_img - w_temp;j++)
		{
			unsigned char sample[h_temp*w_temp*channels];
			for(int p=0;p<h_temp;p++)
			{
				for(int q=0;q<w_temp;q++)
				{
					for(int k=0;k<channels;k++)
						{
							sample[p*w_temp*channels + q*channels + k] = img[(i+p)*w_img*channels + (j+q)*channels + k];
						}
				}
			}
			//corr_matrix[i*d_w +j] = find_corr(sample, temp_arr, temp_size);
			double a = find_corr(sample, temp, h_temp*w_temp*channels);
			corr_mat[i*(w_img - w_temp) +j] =a;// find_corr(sample, feature_arr, feature_size);
		}
	}
}
void maxLoc(double* corr_mat,int* matchLoc, int h_temp, int w_temp, int h_img, int w_img)
{
	matchLoc[0] = 0;
	matchLoc[1] =0;
	double max_value=corr_mat[0];
	for(int i=0;i<h_img - h_temp;i++)
	{
		for(int j=0;j<w_img - w_temp;j++)
		{
			if(corr_mat[i*(w_img - w_temp) + j] > max_value)
			{
				max_value = corr_mat[i*(w_img - w_temp) + j];
				matchLoc[0] = i;
				matchLoc[1] = j;
			}
		}
	}
}
void mkDot(unsigned char* mask,int h_img, int w_img,int height, int width, int channels)
{
	for(int k=0;k<channels;k++)
	{
		mask[height*w_img*channels + width*channels +k ] = 0;
		//pro_mask[height*w_i*c_i + width*c_i +k ] = 0;
	}
}


/*char file[20];
char template[20];
char result[20];
puts("file name, temp name, result name");
scanf("%19s", file);
scanf("%19s", template);
scanf("%19s", result);

char file_path[40] = "./image/";
char temp_path[40]="./template/";
char result_path[40]="./result/";
strcat(file_path, file);
strcat(temp_path,template);
strcat(result_path,result);
*/
