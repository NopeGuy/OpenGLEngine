#ifndef MATH_FUNCTIONS_HPP
#define MATH_FUNCTIONS_HPP

// Function to multiply a matrix by a vector
void multMatrixVector(float* m, float* v, float* res);

void getCatmullRomPoint(float t, float* p0, float* p1, float* p2, float* p3, float* pos, float* deriv);

void getGlobalCatmullRomPoint(float gt, vector<float> px, vector<float> py, vector<float> pz, float* pos, float* deriv);

void renderCatmullRomCurve(vector<float> px, vector<float> py, vector<float> pz);

#endif /* MATH_FUNCTIONS_HPP */
