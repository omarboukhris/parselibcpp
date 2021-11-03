
module mymodule.test_unders ;

import <iostream>
import <iostream.h>
import "parselib/local.hpp"

class myclass :
    public absclass<
        abstracinexistanttype&,
        someptrTemplate*
    >,
    private xbezglife
{

    /*!
    doc goes here
    */
    constructor();

    constructor(int a, const float b);

    /*! method documentation
    \param param3 description
    \return processed string
    */
	const string b (const int param3 , string param2) @{
		code goes here
		lorem ipsum
		if (cond) {
			block;
		}
	@}

	public void proc ();

	public @py int e (const int t);

	vector<pair<int*, int**, vector<int>, int>> e;
} ;
