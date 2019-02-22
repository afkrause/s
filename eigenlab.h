#pragma once

#include <Eigen/Eigen>

namespace EL
{
	
// eigen helpers
using namespace Eigen;

template<class T = float> Matrix<T, -1, 1> rand(size_t n)
{
	return Matrix<T, -1, 1>::Random(n);
}

template<class T = float> Matrix<T, -1, -1> rand(size_t rows, size_t cols)
{
	return Matrix<T, -1, -1>::Random(rows, cols);
}


// returns a Vector of size n with all elements equal to zero
template<class T=float> Matrix<T,-1,1> zeros(size_t n)
{
	return Matrix<T, -1, 1>::Zero(n);
}

// returns a Matrix of size [rows x cols] with all elements equal to zero
template<class T = float> Eigen::Matrix<T, -1, -1> zeros(size_t rows, size_t cols)
{
	return Matrix<T, -1, -1>::Zero(rows, cols);
}

// returns a Vector of size n with all elements equal to zero
template<class T = float> Matrix<T, -1, 1> ones(size_t n)
{
	return Matrix<T, -1, 1>::Ones(n);
}

// returns a Matrix of size [rows x cols] with all elements equal to zero
template<class T = float> Matrix<T, -1, -1> ones(size_t rows, size_t cols)
{
	return Matrix<T, -1, -1>::ones(rows, cols);
}


// elementwise Vector + Scalar and Matrix + Scalar
/*
// Matrix<T, -1, 1> operator+()(Matrix<T, -1, 1> v, T s)
inline Matrix<float, -1, 1> operator+(const Matrix<float, -1, 1>& v, float s)
{
	return (v.array() + s).matrix();
}


template<class T, int d1, int d2, int d3, int d4, int d5> Matrix<T, d1, d2, d3, d4, d5> operator+(const Matrix<T, d1, d2, d3, d4, d5>& v, float s)
{
	return (v.array() + s).matrix();
}
*/

}
