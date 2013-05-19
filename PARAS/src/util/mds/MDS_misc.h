#ifndef MDS_TYPES_HH
#define MDS_TYPES_HH


#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>


#include "util/mds/IP_Vector.h"
#include "util/mds/IP_Matrix.h"


//
// tyepdefs
//
typedef	IP_Vector<double>		MDS_Vector;
typedef IP_Matrix<double>		MDS_Matrix;

#endif // MDS_TYPES_HH
