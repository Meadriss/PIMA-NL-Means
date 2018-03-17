#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>

#define cimg_verbosity 1
#define cimg_plugin "plugins/nlmeans.h"
#include "CImg.h"
#include "utils.h"
using namespace cimg_library;
using namespace std;
using namespace utils;


/*
 * distance euclidienne
 */
double weightTild(vector<float>& gsvI, vector<float>& gsvJ, float h,float a){
  double eucli = euclideanDistanceGauss(gsvI,gsvJ,h);
  return exp(-((eucli))/(h*h));
}
/*
 * Constente de normalisation
 */
double normaliseConstant(CImg<unsigned char>& I, vector<float>& gsvI,int patchSize,float h,int x, int y){
  vector<float> gsvJ;
  double constant = 0;
  int size = (patchSize-1)/2;
  float a = 0, wMax = 0;
  for(int i = -patchSize; i <= patchSize; i++){
    for(int j = -patchSize; j <= patchSize; j++){
      gsvJ = getGrayScaleVector(I,x+i,y+j,size);
      a = standarDeviationGauss(i,j,h);
      float w = weightTild(gsvI,gsvJ,h,a);
      constant += w;
      //cout << i << ", " << constant << endl;
    }
  }
  //cout << "constant nlmeans : " << constant << endl;
  return constant;
}

/*
 * fonction calcul poids
 */
double weight(vector<float>& gsvI, vector<float>& gsvJ, float h, double c,double a){
  double wt = weightTild(gsvI,gsvJ,h,a);
  return wt/c;
}


/*
 * fonction calcul de la couleur d'un pixel
 */

double filterPixel(CImg<unsigned char>& I, vector<float> gsvI, int patchSize, float h,int x, int y){
  int size = (patchSize-1)/2;
  int search = (patchSize*3)+1;
  double w = 0, somme = 0;
  double c = normaliseConstant(I,gsvI,patchSize,h,x,y);
  vector<float> gsvJ;
  double sommeW = 0;
  float a = 0;
  for(int i= -patchSize; i <= patchSize; i++){
    for(int j = -patchSize; j <= patchSize; j++){
      gsvJ = getGrayScaleVector(I,x+i,y+j,size);
      a = standarDeviationGauss(i,j,h);
      w = weight(gsvI,gsvJ,h,c,a);
      sommeW += w;
      somme += w*I(x+i,y+j);
    }
  }
  //cout << sommeW << " somme poids au pixel : " << x << ", " << y << endl;
  return somme;
}



/*
 * NLMeans algo 
 * pour l'instant test le poids des pixels par rapport a un pixel
 */
CImg<unsigned char> nlMeans(CImg<unsigned char>& I, int patchSize,float h){
  CImg<unsigned char> F(I);
  CImg<unsigned char> It = zeroPadding(I,patchSize);
  int n = F.width();
  int m = F.height();
  int size = (patchSize-1)/2;
  int search = patchSize*3-1;
  vector<float> gsvI;
  for(int x = 0; x < n; x++){
    for(int y = 0; y < m; y++){
      gsvI = getGrayScaleVector(It,x+search/2,y+search/2,size);
      F(x,y) = filterPixel(It,gsvI,patchSize,h,x+search/2,y+search/2);
    }
  }
  return F;
}

/*
 * filtrage moyenneur
 */
CImg<unsigned char> means(CImg<unsigned char>& I, int patchSize){
  CImg<unsigned char> F(I);
  CImg<unsigned char> It = zeroPadding(I,patchSize);
  int n = F.width();
  int m = F.height();
  int size = (patchSize-1)/2;
  int search = patchSize*3-1;
  vector<float> gsvI;
  for(int x = 0; x < n; x++){
    for(int y = 0; y < m; y++){
      gsvI = getGrayScaleVector(It,x+search/2,y+search/2,size);
      F(x,y) = means(gsvI);
    }
  }
  return F;
}

/*
 * filtrage median
 */
CImg<unsigned char> medianFilter(CImg<unsigned char>& I, int patchSize){
  CImg<unsigned char> F(I);
  CImg<unsigned char> It = zeroPadding(I,patchSize);
  int n = F.width();
  int m = F.height();
  int size = (patchSize-1)/2;
  int search = patchSize*3-1;
  vector<float> gsvI;
  for(int x = 0; x < n; x++){
    for(int y = 0; y < m; y++){
      gsvI = getGrayScaleVector(It,x+search/2,y+search/2,size);
      F(x,y) = median(gsvI);
    }
  }
  return F;
}

int main(int argc, char ** argv){
  int patchSize = 5;
  char *im;
  int noi = -1;
  float h = 10;
  double sig = 10;
  for(int i = 1; i < argc; i++){
    string arg = argv[i];
    if (arg.compare("-i") == 0){
      i++;
      im = argv[i];
      cout << "l'image : "<< im << endl;
      continue;
    }
    if (arg.compare("-s") == 0){
      i++;
      patchSize = atoi(argv[i]);
      cout << "la taille du patch est : "<< patchSize << endl;
      continue;
    }
    if (arg.compare("-n") == 0){
      i++;
      noi = atoi(argv[i]);
      cout << "le bruit choisie est : " ;
      if( noi == 0){
	cout << "gaussien " << endl;
      }
      if( noi == 1){
	cout << "uniforme " << endl;
      }
      if( noi == 2){
	cout << "poivre et sel" << endl;
      }
      if( noi == 3){
	cout << "poisson " << endl;
      }
      if( noi == 4){
	cout << "Rician " << endl;
      }
      continue;
    }
    if (arg.compare("-h") ==0){
      i++;
      h = stof(argv[i]);
      cout << "parametre filtrage : " << h << endl;
      continue;
    }
    if (arg.compare("-sig") == 0){
      i++;
      sig = stof(argv[i]);
      cout << "sigma noise : "<< sig << endl;
      continue;
    }
  }
  CImg<unsigned char> original(im);
  CImg<unsigned char> image(im);
  original.channel(0);
  image.channel(0);
  /* noise(sigma,noise_type) 
   *0 -> gaussian, 1 -> uniform, 2 -> Salt & Pepper,3 -> Poisson, 4 -> Rician
   */
  if( noi >= 0 && noi <= 4){
    image.noise(sig,noi);
  }

  
  CImg<unsigned char> filter = nlMeans(image,patchSize,h);
  CImg<unsigned char> filterM = means(image,patchSize);
  CImg<unsigned char> filterMed = medianFilter(image,patchSize);
  //CImg<float> test = image.get_nlmeans(patchSize,-1,15,h,1);
  CImgList<unsigned char> visu(original,image,filter,filterM,filterMed);
  
  cout << endl;
  cout << "original : "<< original.MSE(original) << endl; 
  cout << "noise : " << image.MSE(original) << endl;
  cout << "NLmeans : " << filter.MSE(original) << endl;
  cout << "means : " << filterM.MSE(original) << endl;
  cout << "median : " << filterMed.MSE(original) << endl;
  //cout << "nl means CImg : " << test.MSE(original) << endl;
  cout << "DB original : " << original.PSNR(original,255) << endl; 
  cout << "DB noise : " << image.PSNR(original,255) << endl;
  cout << "DB NLmeans : " << filter.PSNR(original,255) << endl;
  cout << "DB means : " << filterM.PSNR(original,255) << endl;
  cout << "DB median : " << filterMed.PSNR(original,255) << endl;
  //cout << "DB nl means CImg : " << test.PSNR(original,255) << endl;
  cout << endl;

  /*
   *exemple plot avec sin

  cimg_usage("Simple plotter of mathematical formulas");
  const char *const formula = cimg_option("-f","sin(x/8) % cos(2*x)","Formula to plot");
  const float x0 = cimg_option("-x0",-5.0f,"Minimal X-value");
  const float x1 = cimg_option("-x1",5.0f,"Maximal X-value");
  const int resolution = cimg_option("-r",1024,"Plot resolution");
  const unsigned int nresolution = resolution>1?resolution:1024;
  const unsigned int plot_type = cimg_option("-p",1,"Plot type");
  const unsigned int vertex_type = cimg_option("-v",1,"Vertex type");

  // Create plot data.
  CImg<double> values(4,nresolution,1,1,0);
  const unsigned int r = nresolution - 1;
  cimg_forY(values,X) values(0,X) = x0 + X*(x1-x0)/r;
  cimg::eval(formula,values).move_to(values);

  // Display interactive plot window.
  values.display_graph(formula,plot_type,vertex_type,"X-axis",x0,x1,"Y-axis");

  */
  /*
  CImgDisplay main_disp(visu,"affichage filtrage NL/means/median");
  while(!main_disp.is_closed()){
    main_disp.wait();
  }
  */
  visu.display();
  return 0;
}
