
///@module mymodule.test2

#include <iostream>

#include "../itf1/test1.h"

class Myclass2 : public mymodule::Player<T1, T2<V1, v2, V3> >, private otherClasdb, public XAz<T> {

    /*!
    doc goes here
    */
    constructor();

    constructor(int a, const float b);

    /*! \brief attr doxy doc */
	ns::int a2;
	float c3;

} ;