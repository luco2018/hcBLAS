#include <iostream>
#include "hcblaslib.h"
#include <cstdlib> 
#include "cblas.h"
using namespace std;
int main(int argc, char** argv)
{   
    /* HCBLAS implementation */
    Hcblaslibrary hc;
    if (argc < 3){
        cout<<"No sufficient commandline arguments specified"<<"argc :"<<argc<<endl;
        return -1;
    }
    int N = atoi(argv[1]);
    int Imple_type = atoi(argv[2]);
    float asumhcblas;
    int incX = 1;
    long xOffset = 0;
    hcblasStatus status;
    int batchSize = 128;
    long X_batchOffset = N;
    if(N > 10000)
	batchSize = 50;
    /* CBLAS implementation */
    bool ispassed = 1;
    float asumcblas = 0.0;
    float *asumcblastemp = (float*)calloc(batchSize, sizeof(float));
    /* CBLAS implementation */
    long lenx = 1 + (N-1) * abs(incX);
    float *X = (float*)calloc(lenx, sizeof(float));
    float *Xbatch = (float*)calloc(lenx * batchSize, sizeof(float));
    std::vector<hc::accelerator>acc = hc::accelerator::get_all();
    accelerator_view accl_view = (acc[1].create_view());

/* Implementation type I - Inputs and Outputs are HCC float array containers */
      
    if (Imple_type == 1) {
        hc::array<float> xView(lenx, X);
        std::vector<float> HostX(lenx);
        for(int i = 0;i < lenx;i++){
            HostX[i] = rand() % 10;
            X[i] = HostX[i];
        }
        hc::copy(begin(HostX), end(HostX), xView);
        status = hc.hcblas_sasum(accl_view, N, xView, incX, xOffset, asumhcblas);
        asumcblas = cblas_sasum( N, X, incX);
        if (asumhcblas != asumcblas) {
            ispassed = 0;
            cout <<" HCSASUM " << asumhcblas << " does not match with CBLASSASUM "<< asumcblas << endl;
        }
        if(!ispassed) cout << "TEST FAILED" << endl; 
        if(status) cout << "TEST FAILED" << endl; 
     }

/* Implementation type II - Inputs and Outputs are HCC float array containers with batch processing */

    else{
        hc::array<float> xbatchView(lenx * batchSize, Xbatch);
        std::vector<float> HostX_batch(lenx * batchSize);
        for(int i = 0;i < lenx * batchSize;i++) {
            HostX_batch[i] = rand() % 10;
            Xbatch[i] = HostX_batch[i];
         }
        hc::copy(begin(HostX_batch), end(HostX_batch), xbatchView);
        status= hc.hcblas_sasum(accl_view, N, xbatchView, incX, xOffset, asumhcblas, X_batchOffset, batchSize);
        for(int i = 0; i < batchSize; i++) {
        	asumcblastemp[i] = cblas_sasum( N, Xbatch + i * N, incX);
                asumcblas += asumcblastemp[i];
        }
        if (asumhcblas != asumcblas) {
            ispassed = 0;
            cout <<" HCSASUM " << asumhcblas << " does not match with CBLASSASUM "<< asumcblas << endl;
        }
        if(!ispassed) cout << "TEST FAILED" << endl; 
        if(status) cout << "TEST FAILED" << endl; 
    }
    return 0;
}
